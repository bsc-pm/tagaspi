/* 2015 Daniel Bittman <danielbittman1@gmail.com>: http://dbittman.github.io/ */

#ifndef MPSC_QUEUE_HPP
#define MPSC_QUEUE_HPP

#include "Utils.hpp"

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <sys/types.h>


template <typename T>
class alignas(64) MPSCQueue {
private:
	std::atomic<size_t> _count;
	
	std::atomic<size_t> _head;
	
	size_t _tail;
	
	size_t _capacity;
	
	std::atomic<T> *_buffer;
	
public:
	MPSCQueue(size_t capacity) :
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
	
	~MPSCQueue()
	{
		assert(!_count.load());
		delete [] _buffer;
	}
	
	bool enqueue(T obj)
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
		assert(rv == 0);
		UNUSED(rv);
		
		return true;
	}
	
	T dequeue()
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
		assert(r > 0);
		UNUSED(r);
		
		return ret;
	}
	
	size_t size() const
	{
		return std::atomic_load_explicit(&_count, std::memory_order_seq_cst);
	}
	
	bool empty() const
	{
		return size() == 0;
	}
	
	size_t capacity() const
	{
		return _capacity;
	}
};

#endif /* MPSC_QUEUE_HPP */
