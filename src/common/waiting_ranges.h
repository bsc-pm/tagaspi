/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef WAITING_RANGES_H
#define WAITING_RANGES_H

#include <GASPI.h>
#include <TAGASPI.h>

#include "environment.h"
#include "mpsc_queue.h"
#include "types.h"
#include "utils.h"

#include <stdbool.h>


/** Waiting Range methods **/

static inline bool
waiting_range_check(waiting_range_t *wr)
{
	assert(wr != NULL);
	
	gaspi_return_t eret;
	gaspi_notification_id_t notified_id;
	gaspi_notification_t notified_value;
	
	eret = gaspi_notify_waitsome(wr->segment_id, wr->first_id, wr->num_ids, &notified_id, GASPI_TEST);
	assert(eret == GASPI_SUCCESS || eret == GASPI_TIMEOUT);
	
	bool completed = false;
	if (eret == GASPI_SUCCESS) {
		assert(notified_id >= wr->first_id);
		assert(notified_id < wr->first_id + wr->num_ids);
		
		eret = gaspi_notify_reset(wr->segment_id, notified_id, &notified_value);
		assert(eret == GASPI_SUCCESS);
		
		if (notified_value != 0) {
			if (wr->notified_values != GASPI_NOTIFICATION_IGNORE) {
				wr->notified_values[notified_id - wr->first_id] = notified_value;
			}
			
			completed = (--wr->remaining == 0);
		}
	}
	return completed;
}


/** Waiting Range Queue methods **/

static inline void
waiting_range_queue_initialize(waiting_range_queue_t *queue)
{
	assert(queue != NULL);
	mpsc_queue_t *mpsc_queue = (mpsc_queue_t *)queue;
	mpsc_queue_initialize(mpsc_queue, 1024);
}

static inline void
waiting_range_queue_finalize(waiting_range_queue_t *queue)
{
	assert(queue != NULL);
	mpsc_queue_t *mpsc_queue = (mpsc_queue_t *)queue;
	mpsc_queue_finalize(mpsc_queue);
}

static inline void
waiting_range_queue_enqueue(waiting_range_queue_t *queue, waiting_range_t *wr)
{
	assert(queue != NULL);
	assert(wr != NULL);
	
	mpsc_queue_t *mpsc_queue = (mpsc_queue_t *) queue;
	while (mpsc_queue_enqueue(mpsc_queue, wr)) {
		delay();
	}
}

static inline bool
waiting_range_queue_dequeue_and_check(waiting_range_queue_t *queue, int max_dequeues, waiting_range_t **pending_ptr, waiting_range_t **completed_ptr)
{
	assert(queue != NULL);
	assert(max_dequeues > 0);
	assert(pending_ptr != NULL);
	assert(completed_ptr != NULL);
	
	mpsc_queue_t *mpsc_queue = (mpsc_queue_t *)queue;
	waiting_range_t *completed = NULL;
	waiting_range_t *pending = NULL;
	int num_dequeued = 0;
	waiting_range_t *wr;
	
	do {
		wr = mpsc_queue_dequeue(mpsc_queue);
		if (wr != NULL) {
			if (waiting_range_check(wr)) {
				wr->next = completed;
				completed = wr;
			} else {
				wr->next = pending;
				pending = wr;
			}
			++num_dequeued;
		}
	} while (wr != NULL && num_dequeued < max_dequeues);
	
	*pending_ptr = pending;
	*completed_ptr = completed;
	
	if (num_dequeued == max_dequeues) {
		return !mpsc_queue_empty(mpsc_queue);
	}
	return false;
}


/** Waiting Range List methods */

static inline void
waiting_range_list_initialize(waiting_range_list_t *list)
{
	assert(list != NULL);
	waiting_range_t **wr_ptr = (waiting_range_t **)list;
	*wr_ptr = NULL;
}

static inline void
waiting_range_list_finalize(waiting_range_list_t *list)
{
	assert(list != NULL);
	waiting_range_t **wr_ptr = (waiting_range_t **)list;
	assert(*wr_ptr == NULL);
	UNUSED(list);
	UNUSED(wr_ptr);
}

static inline void
waiting_range_list_check_and_insert(waiting_range_list_t *list, waiting_range_t *new_pending, waiting_range_t **completed_ptr)
{
	assert(list != NULL);
	assert(completed_ptr != NULL);
	
	waiting_range_t *pending_head = NULL;
	waiting_range_t *pending_tail = NULL;
	waiting_range_t *completed = *completed_ptr;
	waiting_range_t *wr = (waiting_range_t *) *list;
	
	while (wr != NULL) {
		waiting_range_t *next = wr->next;
		
		if (waiting_range_check(wr)) {
			if (pending_tail != NULL) {
				pending_tail->next = next;
			}
			wr->next = completed;
			completed = wr;
		} else {
			pending_tail = wr;
			if (pending_head == NULL) {
				pending_head = wr;
			}
		}
		wr = next;
	}
	
	*list = (pending_head) ? pending_head : new_pending;
	if (pending_tail != NULL) {
		pending_tail->next = new_pending;
	}
	*completed_ptr = completed;
}

#endif /* WAITING_RANGES_H */
