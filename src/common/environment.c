/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>

#include "environment.h"
#include "polling.h"
#include "waiting_ranges.h"

#include <assert.h>

void initialize()
{
	assert(!glb_env.enabled);
	
	gaspi_queue_max(&glb_env.max_queues);
	gaspi_segment_max(&glb_env.max_queues);
	assert(glb_env.max_queues > 0);
	assert(glb_env.max_segments > 0);
	
	glb_env.waiting_range_queues = (waiting_range_queue_t *)
		calloc(glb_env.max_segments, sizeof(waiting_range_queue_t));
	glb_env.waiting_range_lists = (waiting_range_list_t *)
		calloc(glb_env.max_segments, sizeof(waiting_range_list_t));
	assert(glb_env.waiting_range_queues != NULL);
	assert(glb_env.waiting_range_lists != NULL);
	
	gaspi_segment_id_t seg_id;
	for (seg_id = 0; seg_id < glb_env.max_segments; ++seg_id) {
		waiting_range_queue_initialize(&glb_env.waiting_range_queues[seg_id]);
		waiting_range_list_initialize(&glb_env.waiting_range_lists[seg_id]);
	}
	
	glb_env.max_queue_groups = 16;
	glb_env.num_queue_groups = 0;
	glb_env.queue_groups = (queue_group_t *)
		calloc(glb_env.max_queue_groups, sizeof(queue_group_t));
	assert(glb_env.queue_groups != NULL);
	
	glb_env.queue_polling_locks = (spinlock_t *)
		calloc(glb_env.max_queues, sizeof(spinlock_t));
	assert(glb_env.queue_polling_locks != NULL);
	
	gaspi_queue_id_t queue_id;
	for (queue_id = 0; queue_id < glb_env.max_queues; ++queue_id) {
		spinlock_initialize(&glb_env.queue_polling_locks[queue_id]);
	}
	spinlock_initialize(&glb_env.notification_polling_lock);
	spinlock_initialize(&glb_env.queue_groups_lock);
	
	polling_initialize();
	
	glb_env.enabled = true;
}

void finalize()
{
	assert(glb_env.enabled);
	assert(glb_env.queue_polling_locks != NULL);
	assert(glb_env.waiting_range_queues != NULL);
	assert(glb_env.waiting_range_lists != NULL);
	assert(glb_env.queue_groups != NULL);
	
	polling_finalize();
	
	gaspi_segment_id_t seg_id;
	for (seg_id = 0; seg_id < glb_env.max_segments; ++seg_id) {
		waiting_range_queue_finalize(&glb_env.waiting_range_queues[seg_id]);
		waiting_range_list_finalize(&glb_env.waiting_range_lists[seg_id]);
	}
	
	gaspi_queue_id_t queue_id;
	for (queue_id = 0; queue_id < glb_env.max_queues; ++queue_id) {
		spinlock_finalize(&glb_env.queue_polling_locks[queue_id]);
	}
	spinlock_finalize(&glb_env.notification_polling_lock);
	spinlock_finalize(&glb_env.queue_groups_lock);
	
	free(glb_env.queue_polling_locks);
	free(glb_env.waiting_range_queues);
	free(glb_env.waiting_range_lists);
	free(glb_env.queue_groups);
	
	glb_env.enabled = false;
}

