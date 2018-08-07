/*
	MIT License
	
	Copyright (c) 2016 Jorge Bellon Castro
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "utils.h"

#include <assert.h>
#include <stdbool.h>

typedef struct {
	ALIGN64 volatile unsigned char _locked;
	char _padding[64 - sizeof(char)];
} spinlock_t;

static inline void spinlock_initialize(spinlock_t *spinlock)
{
	__atomic_clear(&(spinlock->_locked), __ATOMIC_RELAXED);
}

static inline void spinlock_finalize(spinlock_t *spinlock)
{
	assert(!spinlock->_locked);
	UNUSED(spinlock);
}

static inline bool spinlock_trylock(spinlock_t *spinlock) {
	return !__atomic_test_and_set(&(spinlock->_locked), __ATOMIC_ACQUIRE);
}

static inline void spinlock_lock(spinlock_t *spinlock)
{
	while (!spinlock_trylock(spinlock)) {
		delay();
	}
}

static inline void spinlock_unlock(spinlock_t *spinlock) {
	__atomic_clear(&(spinlock->_locked), __ATOMIC_RELEASE);
}

#endif // SPINLOCK_H

