/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef SPINLOCK_HPP
#define SPINLOCK_HPP

#include <atomic>
#include <cassert>

#ifndef SPIN_LOCK_READS_BETWEEN_CMPXCHG
#define SPIN_LOCK_READS_BETWEEN_CMPXCHG 100
#endif


class alignas(64) SpinLock {
private:
	std::atomic<bool> _lock;
	
	char _padding[64 - sizeof(std::atomic<bool>)];
	
public:
	SpinLock() :
		_lock(false),
		_padding()
	{}
	
	SpinLock(const SpinLock &) = delete;
	SpinLock(SpinLock &&) = delete;
	
	~SpinLock()
	{
		assert(!_lock.load());
	}
	
	void lock()
	{
		bool expected = false;
		while (!_lock.compare_exchange_weak(expected, true, std::memory_order_acquire)) {
			int spinsLeft = SPIN_LOCK_READS_BETWEEN_CMPXCHG;
			do {
				expected = _lock.load(std::memory_order_relaxed);
				spinsLeft--;
			} while (expected && (spinsLeft > 0));
			expected = false;
		}
	}
	
	bool trylock()
	{
		bool expected = false;
		return _lock.compare_exchange_strong(expected, true, std::memory_order_acquire);
	}
	
	void unlock()
	{
		assert(_lock.load());
		_lock.store(false, std::memory_order_release);
	}
};

#endif // SPINLOCK_HPP

