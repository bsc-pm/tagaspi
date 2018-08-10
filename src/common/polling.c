/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <GASPI_Lowlevel.h>

#include "environment.h"
#include "polling.h"
#include "runtime_api.h"
#include "waiting_ranges.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

static int poll_queues(void *data);
static int poll_notifications(void *data);

void polling_initialize()
{
	gaspi_number_t queue;
	for (queue = 0; queue < glb_env.max_queues; queue += QUEUES_PER_SERVICE) {
		nanos_register_polling_service("TAGASPI QUEUE POLLING", poll_queues, (void*)(uintptr_t)queue);
	}
	nanos_register_polling_service("TAGASPI NOTIFICATION POLLING", poll_notifications, NULL);
}

void polling_finalize()
{
	gaspi_number_t queue;
	for (queue = 0; queue < glb_env.max_queues; queue += QUEUES_PER_SERVICE) {
		nanos_unregister_polling_service("TAGASPI QUEUE POLLING", poll_queues, (void*)(uintptr_t)queue);
	}
	nanos_unregister_polling_service("TAGASPI NOTIFICATION POLLING", poll_notifications, NULL);
}

int poll_queues(void *data)
{
	gaspi_queue_id_t queue = (uintptr_t) data;
	assert(queue < glb_env.max_queues);
	
	gaspi_request_t requests[NUM_REQUESTS];
	gaspi_number_t completed_reqs, i;
	gaspi_status_t status;
	gaspi_return_t eret;
	
	bool repeat_queue;
	void *event_counter;
	
	gaspi_number_t num_queues = MIN(queue + QUEUES_PER_SERVICE, glb_env.max_queues);
	for (; queue < num_queues; ++queue) {
		if (!spinlock_trylock(&glb_env.queue_polling_locks[queue]))
			continue;
		
		do {
			repeat_queue = false;
			completed_reqs = 0;
			
			eret = gaspi_request_wait(queue, NUM_REQUESTS, &completed_reqs, requests, GASPI_TEST);
			assert(eret == GASPI_SUCCESS || eret == GASPI_TIMEOUT);
			assert(completed_reqs <= NUM_REQUESTS);
			
			for (i = 0; i < completed_reqs; ++i) {
				eret = gaspi_request_get_tag(&requests[i], (gaspi_tag_t *) &event_counter);
				assert(eret == GASPI_SUCCESS);
				assert(event_counter != NULL);
				
				eret = gaspi_request_get_status(&requests[i], &status);
				assert(eret == GASPI_SUCCESS);
				assert(status == GASPI_SUCCESS);
				
				nanos_decrease_task_event_counter(event_counter, 1);
			}
			repeat_queue = (completed_reqs == NUM_REQUESTS);
		} while (repeat_queue);
		
		spinlock_unlock(&glb_env.queue_polling_locks[queue]);
	}
	
	return 0;
}

int poll_notifications(void *data)
{
	assert(data == NULL);
	UNUSED(data);
	
	if (!spinlock_trylock(&glb_env.notification_polling_lock)) {
		return 0;
	}
	
	waiting_range_queue_t *queue;
	waiting_range_list_t *list;
	gaspi_segment_id_t seg_id;
	bool repeat_seg;
	
	for (seg_id = 0; seg_id < glb_env.max_segments; ++seg_id) {
		queue = &glb_env.waiting_range_queues[seg_id];
		list = &glb_env.waiting_range_lists[seg_id];
		
		do {
			waiting_range_t *pending = NULL;
			waiting_range_t *completed = NULL;
			
			repeat_seg = waiting_range_queue_dequeue_and_check(queue, 64, &pending, &completed);
			waiting_range_list_check_and_insert(list, pending, &completed);
			
			while (completed != NULL) {
				waiting_range_t *aux = completed->next;
				nanos_decrease_task_event_counter(completed->event_counter, completed->num_ids);
				free(completed);
				completed = aux;
			}
		} while (repeat_seg);
	}
	
	spinlock_unlock(&glb_env.notification_polling_lock);
	
	return 0;
}

