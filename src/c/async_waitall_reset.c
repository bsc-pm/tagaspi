/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <GASPI_Lowlevel.h>

#include "environment.h"
#include "runtime_api.h"
#include "utils.h"
#include "waiting_ranges.h"

#include <assert.h>

gaspi_return_t
tagaspi_notify_async_waitall_reset(const gaspi_segment_id_t segment_id,
		const gaspi_notification_id_t notification_begin,
		const gaspi_number_t num,
		gaspi_notification_info_t * const notification_info,
		gaspi_notification_t old_notification_values[])
{
	CHECK_SIZE(waiting_range_t, gaspi_notification_info_t);
	assert(glb_env.enabled);
	assert(!nanos6_in_serial_context());
	assert(segment_id < glb_env.max_segments);
	
	if (num == 0) return GASPI_SUCCESS;
	assert(num > 0);
	
	waiting_range_t *wr = (waiting_range_t *) notification_info;
	assert(wr != NULL);
	
	void *counter = nanos6_get_current_event_counter();
	assert(counter != NULL);
	
	// Initialize the waiting range
	wr->segment_id = segment_id;
	wr->first_id = notification_begin;
	wr->num_ids = num;
	wr->notified_values = old_notification_values;
	wr->remaining = num;
	wr->event_counter = counter;
	wr->next = NULL;
	
	nanos6_increase_current_task_event_counter(counter, num);
	
	waiting_range_queue_enqueue(&glb_env.waiting_range_queues[segment_id], wr);
	
	return GASPI_SUCCESS;
}
