/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2020 Barcelona Supercomputing Center (BSC)
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
tagaspi_write(const gaspi_segment_id_t segment_id_local,
		const gaspi_offset_t offset_local,
		const gaspi_rank_t rank,
		const gaspi_segment_id_t segment_id_remote,
		const gaspi_offset_t offset_remote,
		const gaspi_size_t size,
		const gaspi_queue_id_t queue)
{
	assert(_env.enabled);
	gaspi_return_t eret;

	void *counter = NULL;
	if (AckWaitingRange::_currentWaitingRange != nullptr) {
		counter = AckWaitingRange::_currentWaitingRange->getEventCounter();
	} else {
		counter = TaskingModel::getCurrentEventCounter();
	}
	assert(counter != NULL);

	gaspi_tag_t tag = (gaspi_tag_t) counter;

	// TODO: Use gaspi_operation_get_num_requests
	gaspi_number_t numRequests = 1;
	TaskingModel::increaseCurrentTaskEventCounter(counter, numRequests);

	eret = gaspi_operation_submit(GASPI_OP_WRITE, tag,
				segment_id_local, offset_local, rank,
				segment_id_remote, offset_remote, size,
				0, 0, queue, GASPI_BLOCK);
	assert(eret != GASPI_TIMEOUT);

	if (eret != GASPI_SUCCESS) {
		TaskingModel::decreaseTaskEventCounter(counter, numRequests);
	}

	return eret;
}

#ifdef __cplusplus
}
#endif

