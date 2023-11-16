/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2023 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <GASPI_Lowlevel.h>

#include "common/Environment.hpp"
#include "common/TaskingModel.hpp"

#include <cassert>

using namespace tagaspi;

#pragma GCC visibility push(default)

#ifdef __cplusplus
extern "C" {
#endif

gaspi_return_t
tagaspi_read_list(const gaspi_number_t num,
		gaspi_segment_id_t * const segment_id_local,
		gaspi_offset_t * const offset_local,
		const gaspi_rank_t rank,
		gaspi_segment_id_t * const segment_id_remote,
		gaspi_offset_t * const offset_remote,
		gaspi_size_t * const size,
		const gaspi_queue_id_t queue)
{
	assert(_env.enabled);
	gaspi_return_t eret;

	TaskingModel::task_handle_t task = TaskingModel::getCurrentTask();
	assert(task != NULL);

	gaspi_tag_t tag = (gaspi_tag_t) task;

	gaspi_number_t numRequests = 0;
	eret = gaspi_operation_get_num_requests(GASPI_OP_READ_LIST, num, &numRequests);
	assert(eret == GASPI_SUCCESS);
	assert(numRequests > 0);

	TaskingModel::increaseCurrentTaskEvents(task, numRequests);

	eret = gaspi_operation_list_submit(GASPI_OP_READ_LIST, tag,
				num, segment_id_local, offset_local, rank,
				segment_id_remote, offset_remote, size,
				0, 0, 0, queue, GASPI_BLOCK);
	assert(eret != GASPI_TIMEOUT);

	if (eret != GASPI_SUCCESS) {
		TaskingModel::decreaseTaskEvents(task, numRequests);
	}

	return eret;
}

#ifdef __cplusplus
}
#endif

#pragma GCC visibility pop
