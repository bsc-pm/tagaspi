/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <GASPI_Lowlevel.h>

#include "environment.h"
#include "runtime_api.h"

#include <assert.h>

gaspi_return_t
tagaspi_write_list_notify(const gaspi_number_t num,
		gaspi_segment_id_t * const segment_id_local,
		gaspi_offset_t * const offset_local,
		const gaspi_rank_t rank,
		gaspi_segment_id_t * const segment_id_remote,
		gaspi_offset_t * const offset_remote,
		gaspi_size_t * const size,
		const gaspi_segment_id_t segment_id_notification,
		const gaspi_notification_id_t notification_id,
		const gaspi_notification_t notification_value,
		const gaspi_queue_id_t queue,
		const gaspi_timeout_t timeout_ms)
{
	assert(glb_env.enabled);
	assert(!nanos_in_serial_context());
	gaspi_return_t eret;
	
	void *counter = nanos_get_current_event_counter();
	assert(counter != NULL);
	
	gaspi_tag_t tag = (gaspi_tag_t) counter;
	
	gaspi_number_t num_requests = 0;
	eret = gaspi_operation_get_num_requests(GASPI_OP_WRITE_LIST_NOTIFY, num, &num_requests);
	assert(eret == GASPI_SUCCESS);
	assert(num_requests > 0);
	
	nanos_increase_current_task_event_counter(counter, num_requests);
	
	eret = gaspi_operation_list_submit(GASPI_OP_WRITE_LIST_NOTIFY,
				tag, num, segment_id_local, offset_local, rank,
				segment_id_remote, offset_remote, size,
				segment_id_notification, notification_id,
				notification_value, queue, timeout_ms);
	
	if (eret != GASPI_SUCCESS) {
		nanos_decrease_task_event_counter(counter, num_requests);
	}
	
	return eret;
}
