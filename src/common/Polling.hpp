/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef POLLING_HPP
#define POLLING_HPP

#include <GASPI.h>

#include "RuntimeAPI.hpp"

#include <cstdint>


class Polling {
private:
	static const unsigned int NUM_REQUESTS = 16;
	static const unsigned int QUEUES_PER_SERVICE = 4;
	
public:
	static void initialize();
	
	static void finalize();
	
	static int pollQueues(void *data);
	
	static int pollNotifications(void *data);
};

#endif // POLLING_HPP
