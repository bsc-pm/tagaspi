/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2019 Barcelona Supercomputing Center (BSC)
*/

#ifndef WAITING_RANGE_QUEUE_HPP
#define WAITING_RANGE_QUEUE_HPP

#include <boost/lockfree/spsc_queue.hpp>
#include <boost/lockfree/queue.hpp>

#include "WaitingRange.hpp"
#include "WaitingRangeList.hpp"
#include "util/SpinLock.hpp"
#include "util/Utils.hpp"

#ifndef WR_QUEUE_CAPACITY
#define WR_QUEUE_CAPACITY (63*1024)
#endif

namespace tagaspi {

class WaitingRangeQueue {
private:
	typedef boost::lockfree::spsc_queue<WaitingRange*, boost::lockfree::capacity<WR_QUEUE_CAPACITY> > add_queue_t;

	add_queue_t _queue;

	SpinLock _lock;

public:
	inline WaitingRangeQueue() :
		_queue(),
		_lock()
	{
	}

	inline ~WaitingRangeQueue()
	{
		assert(_queue.empty());
	}

	inline void enqueue(WaitingRange *waitingRange)
	{
		assert(waitingRange != nullptr);

		_lock.lock();
		while (!_queue.push(waitingRange)) {
			util::spinWait();
		}
		_lock.unlock();
	}

	inline void dequeueAll(WaitingRangeList &pendingRanges)
	{
		if (!_queue.empty()) {
			_queue.consume_all(
				[&](WaitingRange *range) {
					assert(range != nullptr);
					pendingRanges.add(range);
				}
			);
		}
	}
};

} // namespace tagaspi

#endif // WAITING_RANGE_QUEUE_HPP
