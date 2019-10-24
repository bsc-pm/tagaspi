/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2018-2019 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <GASPI_Lowlevel.h>

#include "common/Allocator.hpp"
#include "common/Environment.hpp"
#include "common/RuntimeAPI.hpp"
#include "common/WaitingRange.hpp"
#include "common/WaitingRangeQueue.hpp"

#include <cassert>

#ifdef __cplusplus
extern "C" {
#endif

gaspi_return_t
tagaspi_notify_async_waitall_reset(const gaspi_segment_id_t seg_id,
		const gaspi_notification_id_t noti_begin,
		const gaspi_number_t num,
		gaspi_notification_t noti_values[])
{
	assert(_env.enabled);
	assert(!nanos6_in_serial_context());
	assert(seg_id < _env.maxSegments);
	
	if (num == 0) return GASPI_SUCCESS;
	assert(num > 0);
	
	void *counter = nanos6_get_current_event_counter();
	assert(counter != NULL);
	nanos6_increase_current_task_event_counter(counter, 1);
	
	WaitingRange *waitingRange = Allocator<WaitingRange>::allocate(seg_id, noti_begin, num, noti_values, counter);
	assert(waitingRange != nullptr);
	
	_env.waitingRangeQueues[seg_id].enqueue(waitingRange);
	
	return GASPI_SUCCESS;
}

#ifdef __cplusplus
}
#endif

