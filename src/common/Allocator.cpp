/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2019 Barcelona Supercomputing Center (BSC)
*/

#include "Allocator.hpp"
#include "WaitingRange.hpp"

#include <cstdio>
#include <cstdlib>


template<>
Allocator<WaitingRange>::queue_t* Allocator<WaitingRange>::_queue = nullptr;

template<>
WaitingRange* Allocator<WaitingRange>::_objects = nullptr;

#if !defined(NDEBUG)
namespace boost {
	void assertion_failed_msg(char const * expr, char const * msg, char const * function, char const * file, long line)
	{
		fprintf(stderr, "%s:%ld %s Boost assertion failure: %s when evaluating %s\n", file, line, function, msg, expr);
		abort();
	}

	void assertion_failed(char const * expr, char const * function, char const * file, long line)
	{
		fprintf(stderr, "%s:%ld %s Boost assertion failure when evaluating %s\n", file, line, function, expr);
		abort();
	}
}
#endif
