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
	gaspi_request_t requests[NREQ];
	gaspi_status_t status;
	void *eventCounter;
	
	gaspi_return_t eret;
	UNUSED_VARIABLE(eret);
	
	numQueues = std::min(queue + QPPS, _env.maxQueues);
	
	for (; queue < numQueues; ++queue) {
		SpinLock &mutex = _env.queuePollingLocks[queue];
		if (!mutex.trylock()) {
			continue;
		}
		
		do {
			eret = gaspi_request_wait(queue, NREQ, &completedReqs, requests, GASPI_TEST);
			assert(eret == GASPI_SUCCESS || eret == GASPI_TIMEOUT);
			assert(completedReqs <= NREQ);
			
			for (r = 0; r < completedReqs; ++r) {
				eret = gaspi_request_get_tag(&requests[r], (gaspi_tag_t *) &eventCounter);
				assert(eret == GASPI_SUCCESS);
				assert(eventCounter != nullptr);
				
				eret = gaspi_request_get_status(&requests[r], &status);
				assert(eret == GASPI_SUCCESS);
				assert(status == GASPI_SUCCESS);
				
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

