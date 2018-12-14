/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>
#include <xmmintrin.h>

#ifdef NDEBUG
#define UNUSED_VARIABLE(_var) (void)(_var)
#else
#define UNUSED_VARIABLE(_var)
#endif

#define delay() _mm_pause()

#define MASK_BITS(a)    (sizeof(a)*8)
#define MASK_SET(a,b)   ((a) |= (1ULL<<(b)))
#define MASK_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define MASK_ISSET(a,b) ((a) & (1ULL<<(b)))
#define MASK_RESET(a)   ((a) = (0ULL))
#define MASK_COUNT(a)   (__builtin_popcountll(a))

typedef size_t mask_t;

#endif /* UTILS_HPP */
