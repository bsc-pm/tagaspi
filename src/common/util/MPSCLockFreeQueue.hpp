/* 2015 Daniel Bittman <danielbittman1@gmail.com>: http://dbittman.github.io/ */

/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2019-2021 Barcelona Supercomputing Center (BSC)
*/


#ifndef MPSC_LOCKFREE_QUEUE_HPP
#define MPSC_LOCKFREE_QUEUE_HPP

#include "Utils.hpp"

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <sys/types.h>

namespace tagaspi {
namespace util {

template <typename T>
class alignas(64) MPSCLockFreeQueue {
private:
	std::atomic<size_t> _count;

	std::atomic<size_t> _head;

	size_t _tail;

	size_t _capacity;

	std::atomic<T> *_buffer;

public:
	inline MPSCLockFreeQueue(size_t capacity) :
		_count(0),
		_head(0),
		_tail(0),
		_capacity(capacity),
		_buffer(nullptr)
	{
		assert(capacity > 0);
		_buffer = new std::atomic<T>[capacity];
		assert(_buffer != nullptr);

		for (size_t i = 0; i < capacity; ++i) {
			std::atomic_init(&_buffer[i], (T) 0);
		}
		std::atomic_thread_fence(std::memory_order_seq_cst);
	}

	inline ~MPSCLockFreeQueue()
	{
		assert(!_count.load());
		assert(_buffer != nullptr);
		delete [] _buffer;
	}

	inline bool enqueue(T obj)
	{
		size_t count = std::atomic_fetch_add_explicit(&_count, (size_t)1, std::memory_order_seq_cst);
		if (count >= _capacity) {
			/* Back off since the queue is full */
			std::atomic_fetch_sub_explicit(&_count, (size_t)1, std::memory_order_seq_cst);
			return false;
		}

		/* Increment the head, which gives us 'exclusive' access to that element */
		size_t head = std::atomic_fetch_add_explicit(&_head, (size_t)1, std::memory_order_seq_cst);
		assert(_buffer[head % _capacity].load() == 0);

		T rv = std::atomic_exchange_explicit(&_buffer[head % _capacity], obj, std::memory_order_seq_cst);
		UNUSED_VARIABLE(rv);
		assert(rv == 0);

		return true;
	}

	inline T dequeue()
	{
		T ret = std::atomic_exchange_explicit(&_buffer[_tail], (T) 0, std::memory_order_seq_cst);
		if (!ret) {
			/* a thread is adding to the queue, but hasn't done the atomic_exchange yet
			 * to actually put the item in. Act as if nothing is in the queue.
			 * Worst case, other producers write content to tail + 1..n and finish, but
			 * the producer that writes to tail doesn't do it in time, and we get here.
			 * But that's okay, because once it DOES finish, we can get at all the data
			 * that has been filled in. */
			return 0;
		}

		if (++_tail >= _capacity) {
			_tail = 0;
		}

		size_t r = std::atomic_fetch_sub_explicit(&_count, (size_t)1, std::memory_order_seq_cst);
		UNUSED_VARIABLE(r);
		assert(r > 0);

		return ret;
	}

	inline size_t size() const
	{
		return std::atomic_load_explicit(&_count, std::memory_order_seq_cst);
	}

	inline bool empty() const
	{
		return size() == 0;
	}

	inline size_t capacity() const
	{
		return _capacity;
	}
};

} // namespace util
} // namespace tagaspi

#endif // MPSC_LOCKFREE_QUEUE_HPP
