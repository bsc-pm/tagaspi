/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <GASPI_Lowlevel.h>

#include "common/Environment.hpp"
#include "common/RuntimeAPI.hpp"

#include <cassert>

#ifdef __cplusplus
extern "C" {
#endif

gaspi_return_t
tagaspi_notify(const gaspi_segment_id_t segment_id_remote,
		const gaspi_rank_t rank,
		const gaspi_notification_id_t notification_id,
		const gaspi_notification_t notification_value,
		const gaspi_queue_id_t queue,
		const gaspi_timeout_t timeout_ms)
{
	assert(_env.enabled);
	assert(!nanos6_in_serial_context());
	gaspi_return_t eret;
	
	void *counter = nanos6_get_current_event_counter();
	assert(counter != NULL);
	
	gaspi_tag_t tag = (gaspi_tag_t) counter;
	
	gaspi_number_t numRequests = 0;
	eret = gaspi_operation_get_num_requests(GASPI_OP_NOTIFY, 0, &numRequests);
	assert(eret == GASPI_SUCCESS);
	assert(numRequests > 0);
	
	nanos6_increase_current_task_event_counter(counter, numRequests);
	
	eret = gaspi_operation_submit(GASPI_OP_NOTIFY, tag,
				0, 0, rank, segment_id_remote, 0, 0,
				notification_id, notification_value,
				queue, timeout_ms);
	
	if (eret != GASPI_SUCCESS) {
		nanos6_decrease_task_event_counter(counter, numRequests);
	}
	
	return eret;
}

#ifdef __cplusplus
}
#endif
