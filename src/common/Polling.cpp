/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <GASPI_Lowlevel.h>

#include "Allocator.hpp"
#include "Environment.hpp"
#include "Polling.hpp"
#include "SpinLock.hpp"
#include "Utils.hpp"
#include "WaitingRange.hpp"
#include "WaitingRangeList.hpp"
#include "WaitingRangeQueue.hpp"

#include <algorithm>
#include <cassert>


void Polling::initialize()
{
	for (gaspi_number_t q = 0; q < _env.maxQueues; q += QUEUES_PER_SERVICE) {
		nanos6_register_polling_service("TAGASPI QUEUES", Polling::pollQueues, (void*)(uintptr_t)q);
	}
	nanos6_register_polling_service("TAGASPI NOTIFICATIONS", Polling::pollNotifications, nullptr);
}

void Polling::finalize()
{
	for (gaspi_number_t q = 0; q < _env.maxQueues; q += QUEUES_PER_SERVICE) {
		nanos6_unregister_polling_service("TAGASPI QUEUES", Polling::pollQueues, (void*)(uintptr_t)q);
	}
	nanos6_unregister_polling_service("TAGASPI NOTIFICATIONS", Polling::pollNotifications, nullptr);
}

int Polling::pollQueues(void *data)
{
	gaspi_queue_id_t queue = (uintptr_t) data;
	assert(queue < _env.maxQueues);
	
	gaspi_number_t completedReqs, numQueues, r;
	gaspi_request_t requests[NUM_REQUESTS];
	gaspi_status_t status;
	void *eventCounter;
	
	gaspi_return_t eret;
	UNUSED(eret);
	
	numQueues = std::min(queue + QUEUES_PER_SERVICE, _env.maxQueues);
	
	for (; queue < numQueues; ++queue) {
		SpinLock &mutex = _env.queuePollingLocks[queue];
		if (!mutex.trylock()) {
			continue;
		}
		
		do {
			eret = gaspi_request_wait(queue, NUM_REQUESTS, &completedReqs, requests, GASPI_TEST);
			assert(eret == GASPI_SUCCESS || eret == GASPI_TIMEOUT);
			assert(completedReqs <= NUM_REQUESTS);
			
			for (r = 0; r < completedReqs; ++r) {
				eret = gaspi_request_get_tag(&requests[r], (gaspi_tag_t *) &eventCounter);
				assert(eret == GASPI_SUCCESS);
				assert(eventCounter != nullptr);
				
				eret = gaspi_request_get_status(&requests[r], &status);
				assert(eret == GASPI_SUCCESS);
				assert(status == GASPI_SUCCESS);
				
				nanos6_decrease_task_event_counter(eventCounter, 1);
			}
		} while (completedReqs == NUM_REQUESTS);
		
		mutex.unlock();
	}
	return 0;
}

int Polling::pollNotifications(void *data)
{
	assert(data == NULL);
	UNUSED(data);
	
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
				nanos6_decrease_task_event_counter(range->_eventCounter, range->_numIds);
				Allocator<WaitingRange>::free(range);
			}
			completeRanges.clear();
		} while (repeat);
	}
	
	mutex.unlock();
	return 0;
}

