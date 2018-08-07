/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <TAGASPI.h>

#include "environment.h"
#include "queue_groups.h"
#include "runtime_api.h"
#include "spinlock.h"
#include "types.h"
#include "utils.h"

#include <assert.h>
#include <stdio.h>

gaspi_return_t
tagaspi_queue_group_create(const gaspi_queue_group_id_t queue_group_id,
			const gaspi_queue_id_t queue_begin,
			const gaspi_number_t queue_num,
			const gaspi_queue_group_policy_t policy)
{
	assert(queue_group_id < glb_env.max_queue_groups);
	assert(queue_begin < glb_env.max_queues);
	assert(queue_begin + queue_num < glb_env.max_queues);
	assert(queue_num > 0);
	
	gaspi_return_t eret = GASPI_SUCCESS;

	spinlock_lock(&glb_env.queue_groups_lock);
	if (glb_env.num_queue_groups >= glb_env.max_queue_groups) {
		fprintf(stderr, "Error: Too many queue groups created\n");
		eret = GASPI_ERROR;
		goto endL;
	}
	
	queue_group_t *queue_group = &glb_env.queue_groups[queue_group_id];
	if (queue_group->data != NULL) {
		fprintf(stderr, "Error: Queue group (%d) already exists\n", queue_group_id);
		eret = GASPI_ERROR;
		goto endL;
	}
	
	queue_group->first_id = queue_begin;
	queue_group->num_ids = queue_num;
	queue_group->policy = policy;
	
	eret = queue_group_setup_data(queue_group);
	if (eret == GASPI_SUCCESS) {
		++glb_env.num_queue_groups;
	}
	
endL:
	spinlock_unlock(&glb_env.queue_groups_lock);
	return eret;
}

gaspi_return_t
tagaspi_queue_group_delete(const gaspi_queue_group_id_t queue_group_id)
{
	assert(queue_group_id < glb_env.max_queue_groups);
	
	spinlock_lock(&glb_env.queue_groups_lock);
	
	queue_group_t *queue_group = &glb_env.queue_groups[queue_group_id];
	if (queue_group->data != NULL) {
		free(queue_group->data);
		queue_group->data = NULL;
	}
	
	--glb_env.num_queue_groups;
	
	spinlock_unlock(&glb_env.queue_groups_lock);
	return GASPI_SUCCESS;
}

gaspi_return_t
tagaspi_queue_group_get_queue(const gaspi_queue_group_id_t queue_group_id,
			gaspi_queue_id_t * const queue)
{
	assert(queue_group_id < glb_env.max_queue_groups);
	assert(queue != NULL);
	
	gaspi_return_t eret;
	
	queue_group_t *queue_group = &glb_env.queue_groups[queue_group_id];
	eret = queue_group_get_queue(queue_group, queue);
	
	return eret;
}

gaspi_return_t
tagaspi_queue_group_max(gaspi_number_t * const queue_group_max)
{
	assert(queue_group_max != NULL);
	
	*queue_group_max = glb_env.max_queue_groups;
	
	return GASPI_SUCCESS;
}
