/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2019 Barcelona Supercomputing Center (BSC)
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


void Polling::initialize()
{
	for (gaspi_number_t q = 0; q < _env.maxQueues; q += QPPS) {
		TaskingModel::registerPollingService("TAGASPI QUEUES", pollQueues, (void*)(uintptr_t)q);
	}
	TaskingModel::registerPollingService("TAGASPI NOTIFICATIONS", pollNotifications, nullptr);
}

void Polling::finalize()
{
	for (gaspi_number_t q = 0; q < _env.maxQueues; q += QPPS) {
		TaskingModel::unregisterPollingService("TAGASPI QUEUES", pollQueues, (void*)(uintptr_t)q);
	}
	TaskingModel::unregisterPollingService("TAGASPI NOTIFICATIONS", pollNotifications, nullptr);
}

int Polling::pollQueues(void *data)
{
	gaspi_queue_id_t queue = (uintptr_t) data;
	assert(queue < _env.maxQueues);

	gaspi_number_t completedReqs, numQueues, r;
	gaspi_status_t statuses[NREQ];
	gaspi_tag_t tags[NREQ];
	gaspi_return_t eret;

	numQueues = std::min(queue + QPPS, _env.maxQueues);

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
					fprintf(stderr, "Error: Unexpected return code from gaspi_request_wait\n");
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
	return 0;
}

int Polling::pollNotifications(void *)
{
	SpinLock &mutex = _env.notificationPollingLock;
	if (!mutex.trylock()) {
		// This should not happen since polling services
		// are called from a single thread at a time
		return 0;
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

				void *eventCounter = range->getEventCounter();
				assert(eventCounter != nullptr);

				TaskingModel::decreaseTaskEventCounter(eventCounter, 1);
				Allocator<WaitingRange>::free(range);
			}
			completeRanges.clear();
		} while (repeat);
	}

	mutex.unlock();
	return 0;
}

