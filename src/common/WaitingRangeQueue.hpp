/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef WAITING_RANGE_QUEUE_HPP
#define WAITING_RANGE_QUEUE_HPP

#include "MPSCQueue.hpp"
#include "Utils.hpp"
#include "WaitingRange.hpp"

#ifndef WR_QUEUE_CAPACITY
#define WR_QUEUE_CAPACITY (64*1024)
#endif


class WaitingRangeQueue {
private:
	MPSCQueue<WaitingRange*> _queue;
	
public:
	WaitingRangeQueue(int capacity = WR_QUEUE_CAPACITY) :
		_queue(capacity)
	{
		assert(capacity > 0);
	}
	
	~WaitingRangeQueue()
	{
		assert(_queue.empty());
	}
	
	void enqueue(WaitingRange *waitingRange)
	{
		assert(waitingRange != nullptr);
		while (!_queue.enqueue(waitingRange)) {
			delay();
		}
	}
	
	bool dequeueSome(std::list<WaitingRange*> &pendingRanges, std::list<WaitingRange*> &completeRanges, int maximum)
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

#endif /* WAITING_RANGE_QUEUE_HPP */
