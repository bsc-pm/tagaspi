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
	/* Number of requests checked per gaspi_request_wait call */
	static const unsigned int NREQ = 16;
	
	/* Number of queues per polling service */
	static const unsigned int QPPS = 4;
	
public:
	static void initialize();
	
	static void finalize();
	
	static int pollQueues(void *data);
	
	static int pollNotifications(void *data);
};

#endif // POLLING_HPP