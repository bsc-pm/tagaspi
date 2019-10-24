/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2018-2019 Barcelona Supercomputing Center (BSC)
*/

#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <config.h>

#include <boost/lockfree/queue.hpp>

template <typename T>
class Allocator {
private:
	static const int NUM_ENTRIES = (64*1000);
	
public:
	typedef boost::lockfree::queue<T*, boost::lockfree::capacity<NUM_ENTRIES> > queue_t;
	
private:
	static T *_objects;
	
	static queue_t *_queue;
	
public:
	static inline void initialize()
	{
		assert(!initialized());
		
		_objects = (T *) std::malloc(NUM_ENTRIES * sizeof(T));
		assert(_objects != nullptr);
		
		_queue = (queue_t *) std::malloc(sizeof(queue_t));
		assert(_queue != nullptr);
		
		new (_queue) queue_t();
		for (int i = 0; i < NUM_ENTRIES; ++i) {
			_queue->push(&_objects[i]);
		}
	}

	static inline void finalize()
	{
		assert(initialized());
		
		_queue->~queue_t();
		std::free(_queue);
		std::free(_objects);
	}
	
	static inline bool initialized()
	{
		return (_objects != nullptr && _queue != nullptr);
	}
	
	template<typename... Args>
	static inline T *allocate(Args &&... args)
	{
		assert(initialized());
		T *object = nullptr;
		
		while (!_queue->pop(object)) {
#ifdef X86_64_ARCH
			__asm__("pause" ::: "memory");
#endif
		}
		assert(object != nullptr);
		
		new (object) T(std::forward<Args>(args)...);
		return object;
	}
	
	static inline void free(T *object)
	{
		assert(initialized());
		assert(object != nullptr);
		
		object->~T();
		bool pushed __attribute__((unused));
		pushed = _queue->push(object);
		assert(pushed);
	}
};

#endif // ALLOCATOR_HPP
