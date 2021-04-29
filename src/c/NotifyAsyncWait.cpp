/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2020 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <GASPI_Lowlevel.h>

#include "common/Allocator.hpp"
#include "common/Environment.hpp"
#include "common/TaskingModel.hpp"
#include "common/WaitingRange.hpp"
#include "common/WaitingRangeQueue.hpp"

#include <cassert>

#ifdef __cplusplus
extern "C" {
#endif


gaspi_return_t
tagaspi_notify_async_wait(const gaspi_segment_id_t segment_id,
		const gaspi_notification_id_t notification_id,
		gaspi_notification_t *notification_value)
{
	assert(_env.enabled);
	assert(segment_id < _env.maxSegments);

	gaspi_number_t remaining = WaitingRange::checkNotification(
		segment_id, notification_id, notification_value);

	if (remaining == 0)
		return GASPI_SUCCESS;

	void *counter = TaskingModel::getCurrentEventCounter();
	assert(counter != NULL);

	TaskingModel::increaseCurrentTaskEventCounter(counter, 1);

	WaitingRange *waitingRange =
		Allocator<WaitingRange>::allocate(
			segment_id, notification_id, 1,
			notification_value, 1, counter);
	assert(waitingRange != nullptr);

	_env.waitingRangeQueues[segment_id].enqueue(waitingRange);

	return GASPI_SUCCESS;
}

gaspi_return_t
tagaspi_notify_async_waitall(const gaspi_segment_id_t segment_id,
		const gaspi_notification_id_t notification_begin,
		const gaspi_number_t notification_num,
		gaspi_notification_t notification_values[])
{
	assert(_env.enabled);
	assert(segment_id < _env.maxSegments);

	if (notification_num == 0)
		return GASPI_SUCCESS;

	gaspi_number_t remaining = WaitingRange::checkNotifications(
		segment_id, notification_begin, notification_num,
		notification_values, notification_num);

	if (remaining == 0)
		return GASPI_SUCCESS;

	void *counter = TaskingModel::getCurrentEventCounter();
	assert(counter != NULL);

	TaskingModel::increaseCurrentTaskEventCounter(counter, 1);

	WaitingRange *waitingRange =
		Allocator<WaitingRange>::allocate(
			segment_id, notification_begin,
			notification_num, notification_values,
			remaining, counter);
	assert(waitingRange != nullptr);

	_env.waitingRangeQueues[segment_id].enqueue(waitingRange);

	return GASPI_SUCCESS;
}

#ifdef __cplusplus
}
#endif

