/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2020 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <GASPI_Lowlevel.h>

#include "Allocator.hpp"
#include "Environment.hpp"
#include "Polling.hpp"
#include "TaskingModel.hpp"
#include "WaitingRange.hpp"
#include "WaitingRangeList.hpp"
#include "WaitingRangeQueue.hpp"
#include "util/Utils.hpp"

#include <algorithm>
#include <cassert>


EnvironmentVariable<uint64_t> Polling::_pollingFrequency("TAGASPI_POLLING_FREQUENCY", 500);
TaskingModel::polling_handle_t *Polling::_queuesPollingHandles;
TaskingModel::polling_handle_t Polling::_notificationsPollingHandle;


void Polling::initialize()
{
	assert(_env.maxQueues > 0);

	const gaspi_number_t numQueuePollingHandles = 1 + ((_env.maxQueues - 1) / QPPI);

	_queuesPollingHandles = new TaskingModel::polling_handle_t[numQueuePollingHandles];
	assert(_queuesPollingHandles != nullptr);

	for (gaspi_number_t q = 0; q < _env.maxQueues; q += QPPI) {
		_queuesPollingHandles[q / QPPI] = TaskingModel::registerPolling(
			"TAGASPI QUEUES",
			pollQueues, (void *)(uintptr_t) q,
			_pollingFrequency
		);
	}

	_notificationsPollingHandle = TaskingModel::registerPolling(
		"TAGASPI NOTIFICATIONS",
		pollNotifications, nullptr,
		_pollingFrequency
	);
}

void Polling::finalize()
{
	for (gaspi_number_t q = 0; q < _env.maxQueues; q += QPPI) {
		TaskingModel::unregisterPolling(_queuesPollingHandles[q / QPPI]);
	}
	TaskingModel::unregisterPolling(_notificationsPollingHandle);

	delete [] _queuesPollingHandles;
}

void Polling::pollQueues(void *data)
{
	gaspi_queue_id_t queue = (uintptr_t) data;
	assert(queue < _env.maxQueues);

	gaspi_number_t completedReqs, numQueues, r;
	gaspi_status_t statuses[NREQ];
	gaspi_tag_t tags[NREQ];
	gaspi_return_t eret;

	numQueues = std::min(queue + QPPI, _env.maxQueues);

	for (; queue < numQueues; ++queue) {
		SpinLock &mutex = _env.queuePollingLocks[queue];
		if (!mutex.trylock()) {
			// This should not happen since polling services
			// are called from a single thread at a time
			continue;
		}

		do {
			eret = gaspi_request_wait(queue, NREQ, &completedReqs, tags, statuses, GASPI_TEST);
			if (eret != GASPI_SUCCESS && eret != GASPI_TIMEOUT) {
				// We are probably cheking queues that are not created
				if (eret != GASPI_ERR_INV_QUEUE) {
					fprintf(stderr, "Error: Return code %d from gaspi_request_wait\n", eret);
					abort();
				}
				completedReqs = 0;
				continue;
			}
			assert(completedReqs <= NREQ);

			for (r = 0; r < completedReqs; ++r) {
				void *eventCounter = (void *) tags[r];
				assert(eventCounter != nullptr);

				if (statuses[r].error != GASPI_SUCCESS) {
					fprintf(stderr, "Error: TAGASPI operation with tag %lld failed\n", (gaspi_tag_t) eventCounter);
					abort();
				}

				TaskingModel::decreaseTaskEventCounter(eventCounter, 1);
			}
		} while (completedReqs == NREQ);

		mutex.unlock();
	}
}

void Polling::pollNotifications(void *)
{
	SpinLock &mutex = _env.notificationPollingLock;
	if (!mutex.trylock()) {
		// This should not happen since polling services
		// are called from a single thread at a time
		return;
	}

	std::list<WaitingRange*> completeRanges;
	std::list<WaitingRange*> pendingRanges;

	gaspi_segment_id_t seg;
	bool repeat;

	for (seg = 0; seg < _env.maxSegments; ++seg) {
		WaitingRangeQueue &queue = _env.waitingRangeQueues[seg];
		WaitingRangeList &list = _env.waitingRangeLists[seg];

		do {
			repeat = queue.dequeueSome(pendingRanges, completeRanges, 64);
			list.checkNotifications(completeRanges);
			list.splice(pendingRanges);

			for (WaitingRange *range : completeRanges) {
				assert(range != nullptr);

				range->complete();

				if (range->isAckWaitingRange()) {
					Allocator<AckWaitingRange>::free((AckWaitingRange *) range);
				} else {
					Allocator<WaitingRange>::free(range);
				}
			}
			completeRanges.clear();
		} while (repeat);
	}

	mutex.unlock();
}

