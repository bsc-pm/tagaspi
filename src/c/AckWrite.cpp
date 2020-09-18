/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2020 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <GASPI_Lowlevel.h>

#include "common/Allocator.hpp"
#include "common/Environment.hpp"
#include "common/TaskingModel.hpp"

#include <cassert>
#include <cstdio>


static void triggerWrite(const AckWaitingRange &waitingRange)
{
	gaspi_tag_t tag = (gaspi_tag_t) waitingRange.getEventCounter();

	const AckWaitingRange::AckActionArgs &args = waitingRange.getAckActionArgs();

	gaspi_return_t eret = gaspi_operation_submit(
		GASPI_OP_WRITE, tag,
		args.segment_id_local, args.offset_local,
		args.rank, args.segment_id_remote,
		args.offset_remote, args.size,
		0, 0, args.queue, GASPI_BLOCK);

	if (eret != GASPI_SUCCESS) {
		fprintf(stderr, "Error: Return code %d from gaspi_operation_submit\n", eret);
		abort();
	}
}


gaspi_return_t
tagaspi_ack_write(const gaspi_segment_id_t ack_segment_id,
		const gaspi_notification_id_t ack_notification_begin,
		const gaspi_number_t ack_notification_num,
		gaspi_notification_t ack_notification_values[],
		const gaspi_segment_id_t segment_id_local,
		const gaspi_offset_t offset_local,
		const gaspi_rank_t rank,
		const gaspi_segment_id_t segment_id_remote,
		const gaspi_offset_t offset_remote,
		const gaspi_size_t size,
		const gaspi_queue_id_t queue)
{
	assert(_env.enabled);
	assert(ack_segment_id < _env.maxSegments);

	if (ack_notification_num == 0)
		return GASPI_SUCCESS;

	gaspi_number_t numRequests = 0;
	gaspi_return_t eret = gaspi_operation_get_num_requests(GASPI_OP_WRITE, 0, &numRequests);
	assert(eret == GASPI_SUCCESS);
	assert(numRequests > 0);

	void *counter = TaskingModel::getCurrentEventCounter();
	assert(counter != NULL);

	TaskingModel::increaseCurrentTaskEventCounter(counter, numRequests);

	gaspi_number_t remaining = WaitingRange::checkNotifications(
		ack_segment_id, ack_notification_begin, ack_notification_num,
		ack_notification_values, ack_notification_num);

	if (remaining == 0) {
		gaspi_tag_t tag = (gaspi_tag_t) counter;

		eret = gaspi_operation_submit(GASPI_OP_WRITE,
			tag, segment_id_local, offset_local, rank,
			segment_id_remote, offset_remote, size,
			0, 0, queue, GASPI_BLOCK);

		if (eret != GASPI_SUCCESS) {
			fprintf(stderr, "Error: Return code %d from gaspi_operation_submit\n", eret);
			abort();
		}
		return GASPI_SUCCESS;
	}

	AckWaitingRange *waitingRange =
		Allocator<AckWaitingRange>::allocate(
			ack_segment_id, ack_notification_begin,
			ack_notification_num, ack_notification_values,
			remaining, counter);
	assert(waitingRange != nullptr);

	waitingRange->setAckActionCallback(triggerWrite);

	AckWaitingRange::AckActionArgs &args = waitingRange->getAckActionArgs();
	args.segment_id_local = segment_id_local;
	args.offset_local = offset_local;
	args.rank = rank;
	args.segment_id_remote = segment_id_remote;
	args.offset_remote = offset_remote;
	args.size = size;
	args.queue = queue;

	_env.waitingRangeQueues[ack_segment_id].enqueue(waitingRange);

	return GASPI_SUCCESS;
}

