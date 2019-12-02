/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2019 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <GASPI_Lowlevel.h>

#include "common/Environment.hpp"
#include "common/TaskingModel.hpp"

#include <cassert>

#ifdef __cplusplus
extern "C" {
#endif

gaspi_return_t
tagaspi_write_notify(const gaspi_segment_id_t segment_id_local,
		const gaspi_offset_t offset_local,
		const gaspi_rank_t rank,
		const gaspi_segment_id_t segment_id_remote,
		const gaspi_offset_t offset_remote,
		const gaspi_size_t size,
		const gaspi_notification_id_t notification_id,
		const gaspi_notification_t notification_value,
		const gaspi_queue_id_t queue,
		const gaspi_timeout_t timeout_ms)
{
	assert(_env.enabled);
	gaspi_return_t eret;

	void *counter = TaskingModel::getCurrentEventCounter();
	assert(counter != NULL);

	gaspi_tag_t tag = (gaspi_tag_t) counter;

	gaspi_number_t numRequests = 0;
	eret = gaspi_operation_get_num_requests(GASPI_OP_WRITE_NOTIFY, 0, &numRequests);
	assert(eret == GASPI_SUCCESS);
	assert(numRequests > 0);

	TaskingModel::increaseCurrentTaskEventCounter(counter, numRequests);

	eret = gaspi_operation_submit(GASPI_OP_WRITE_NOTIFY, tag,
				segment_id_local, offset_local, rank,
				segment_id_remote, offset_remote, size,
				notification_id, notification_value,
				queue, timeout_ms);

	if (eret != GASPI_SUCCESS) {
		TaskingModel::decreaseTaskEventCounter(counter, numRequests);
	}

	return eret;
}

#ifdef __cplusplus
}
#endif
