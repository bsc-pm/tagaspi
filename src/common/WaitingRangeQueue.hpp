/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2018-2019 Barcelona Supercomputing Center (BSC)
*/

#ifndef WAITING_RANGE_QUEUE_HPP
#define WAITING_RANGE_QUEUE_HPP

#include "WaitingRange.hpp"
#include "util/MPSCLockFreeQueue.hpp"
#include "util/Utils.hpp"

#ifndef WR_QUEUE_CAPACITY
#define WR_QUEUE_CAPACITY (64*1024)
#endif


class WaitingRangeQueue {
private:
	util::MPSCLockFreeQueue<WaitingRange*> _queue;
	
public:
	inline WaitingRangeQueue(int capacity = WR_QUEUE_CAPACITY) :
		_queue(capacity)
	{
		assert(capacity > 0);
	}
	
	inline ~WaitingRangeQueue()
	{
		assert(_queue.empty());
	}
	
	inline void enqueue(WaitingRange *waitingRange)
	{
		assert(waitingRange != nullptr);
		while (!_queue.enqueue(waitingRange)) {
			util::spinWait();
		}
	}
	
	inline bool dequeueSome(std::list<WaitingRange*> &pendingRanges, std::list<WaitingRange*> &completeRanges, int maximum)
	{
		assert(maximum > 0);
		
		WaitingRange *range = nullptr;
		int dequeued = 0;
		
		do {
			range = _queue.dequeue();
			if (range != 0) {
				if (range->checkNotifications()) {
					completeRanges.push_back(range);
				} else {
					pendingRanges.push_back(range);
				}
				++dequeued;
			}
		} while (range != 0 && dequeued < maximum);
		
		if (dequeued == maximum) {
			return !_queue.empty();
		}
		return false;
	}
};

#endif // WAITING_RANGE_QUEUE_HPP
