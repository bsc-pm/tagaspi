/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef QUEUE_GROUPS_H
#define QUEUE_GROUPS_H

#include <GASPI.h>
#include <TAGASPI.h>

#include "runtime_api.h"
#include "types.h"
#include "utils.h"

#include <assert.h>
#include <sys/sysinfo.h>

struct queue_range_t {
	gaspi_queue_id_t begin;
	gaspi_number_t num;
};

static inline void
queue_group_get_system_usage_info(size_t max_cpus, size_t *num_cpus, size_t numa_ids[max_cpus])
{
	assert(num_cpus != NULL);
	assert(numa_ids != NULL);
	
	*num_cpus = nanos_get_num_cpus();
	assert(*num_cpus <= max_cpus);
	assert(*num_cpus > 0);
	
	size_t cpu = 0;
	void *cpu_it = nanos_cpus_begin();
	while (cpu_it != nanos_cpus_end()) {
		size_t numa = nanos_cpus_get_numa(cpu_it);
		numa_ids[cpu] = numa;
		cpu_it = nanos_cpus_advance(cpu_it);
		++cpu;
	}
	assert(*num_cpus == cpu);
}

static inline gaspi_return_t
queue_group_setup_cpu_round_robin(queue_group_t *group,
		size_t num_cpus, size_t max_numas, size_t num_numas,
		const size_t numa_ids[num_cpus], mask_t numa_mask)
{
	size_t numa, cpu;
	const size_t queues_per_numa = group->num_ids / num_numas;
	const size_t remaining_queues = group->num_ids % num_numas;
	
	struct queue_range_t numa_queues[max_numas];
	
	/* Assigns distinct ranges of queues to NUMA nodes if possible */
	if (group->num_ids >= num_numas) {
		gaspi_queue_id_t queue = group->first_id;
		for (numa = 0; numa < max_numas; ++numa) {
			/* Assign them to NUMAs which are being used by this rank */
			if (MASK_ISSET(numa_mask, numa)) {
				numa_queues[numa].begin = queue;
				numa_queues[numa].num = queues_per_numa + (numa < remaining_queues);
				queue += numa_queues[numa].num;
			}
    	}
	} else {
		/* Assign one-to-one all queues, except the last, to the first NUMAs */
		size_t num_assigned = 0; numa = 0;
		while (num_assigned < remaining_queues - 1) {
			if (MASK_ISSET(numa_mask, numa)) {
				numa_queues[numa].begin = (gaspi_queue_id_t) num_assigned;
				numa_queues[numa].num = 1;
				++num_assigned;
			}
		}
		assert(num_assigned < num_numas);
		
		/* Assign the last queue to the rest of NUMAs */
		for (numa = remaining_queues - 1; numa < max_numas; ++numa) {
			if (MASK_ISSET(numa_mask, numa)) {
				numa_queues[numa].begin = (gaspi_queue_id_t) (remaining_queues - 1);
				numa_queues[numa].num = 1;
			}
		}
	}
	
	gaspi_queue_id_t *assigned_queue = (gaspi_queue_id_t *) group->data;
	assert(assigned_queue != NULL);
	
	/* Assigns queues to CPUs in Round-Robin */
	for (numa = 0; numa < max_numas; ++numa) {
		if (MASK_ISSET(numa_mask, numa)) {
			const gaspi_number_t queue_begin = numa_queues[numa].begin;
			const gaspi_number_t queue_num = numa_queues[numa].num;
			gaspi_number_t offset = 0;
			
			for (cpu = 0; cpu < num_cpus; ++cpu) {
				if (numa_ids[cpu] == numa) {
					assigned_queue[cpu] = queue_begin + offset;
					offset = (offset + 1) % queue_num;
				}
			}
		}
	}
	return GASPI_SUCCESS;
}

static inline gaspi_return_t
queue_group_setup_data(queue_group_t *group)
{
	assert(group != NULL);
	gaspi_return_t eret = GASPI_SUCCESS;
	
	if (group->policy == GASPI_QUEUE_GROUP_POLICY_DEFAULT) {
		group->data = calloc(1, sizeof(gaspi_number_t));
		assert(group->data != NULL);
	} else if (group->policy == GASPI_QUEUE_GROUP_POLICY_CPU_RR) {
		size_t num_cpus, cpu, max_numas, num_numas, numa;
		size_t max_cpus = get_nprocs_conf();
		size_t numa_ids[max_cpus];
		mask_t numa_mask;
		
		/* Get system information from the runtime system */
		queue_group_get_system_usage_info(max_cpus, &num_cpus, numa_ids);
		
		group->data = calloc(num_cpus, sizeof(gaspi_queue_id_t));
		assert(group->data != NULL);
		
		/* Build the mask of the numa nodes used */
		MASK_RESET(numa_mask);
		for (cpu = 0; cpu < num_cpus; ++cpu) {
			size_t numa = numa_ids[cpu];
			assert(numa < MASK_BITS(numa_mask));
			MASK_SET(numa_mask, numa);
		}
		
		num_numas = MASK_COUNT(numa_mask);
		assert(num_numas > 0);
		
		max_numas = 0;
		for (numa = 0; numa < MASK_BITS(numa_mask); ++numa) {
			if (MASK_ISSET(numa_mask, numa)) {
				max_numas = numa + 1;
			}
		}
		assert(max_numas >= num_numas);
		assert(max_numas > 0);
		
		eret = queue_group_setup_cpu_round_robin(
				group, num_cpus, max_numas,
				num_numas, numa_ids, numa_mask);
	} else {
		eret = GASPI_ERROR;
	}
	return eret;
}

static inline gaspi_return_t
queue_group_get_queue(queue_group_t *group, gaspi_queue_id_t *queue)
{
	assert(queue != NULL);
	assert(group != NULL);
	assert(group->data != NULL);
	
	gaspi_return_t eret = GASPI_SUCCESS;
	
	if (group->policy == GASPI_QUEUE_GROUP_POLICY_DEFAULT) {
		gaspi_number_t offset = 0;
		/* Optimize the case of a single queue */
		if (group->num_ids > 1) {
			gaspi_number_t *counter = (gaspi_number_t *) group->data;
			offset = __sync_fetch_and_add(counter, 0);
			gaspi_number_t next_offset = (offset < group->num_ids - 1) ? offset + 1 : 0;
			
			/* In case the operation fails, another thread will update it */
			__sync_bool_compare_and_swap(counter, offset, next_offset);
		}
		*queue = group->first_id + offset;
	} else if (group->policy == GASPI_QUEUE_GROUP_POLICY_CPU_RR) {
		size_t cpu = nanos_get_current_virtual_cpu();
		gaspi_queue_id_t * queues = (gaspi_queue_id_t *)group->data;
		*queue = queues[cpu];
	} else {
		eret = GASPI_ERROR;
	}
	return eret;
}

#endif // QUEUE_GROUPS_H
