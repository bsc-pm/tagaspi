/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef UTILS_H
#define UTILS_H

#include <assert.h>
#include <stddef.h>
#include <xmmintrin.h>

#define MIN(a,b)  (((a)>(b)) ? (b) : (a))
#define MAX(a,b)  (((a)>(b)) ? (a) : (b))

#ifdef NDEBUG
#define UNUSED(_var) (void)(_var)
#else
#define UNUSED(_var)
#endif

#define delay() _mm_pause()

#define MASK_BITS(a)    (sizeof(a)*8)
#define MASK_SET(a,b)   ((a) |= (1ULL<<(b)))
#define MASK_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define MASK_ISSET(a,b) ((a) & (1ULL<<(b)))
#define MASK_RESET(a)   ((a) = (0ULL))
#define MASK_COUNT(a)   (__builtin_popcountll(a))

typedef size_t mask_t;

#define ALIGN64 __attribute__ ((aligned (64)))

#define CHECK_SIZE(a, b) static_assert(sizeof(a) == sizeof(b), "Types have different size")

#endif /* UTILS_H */
