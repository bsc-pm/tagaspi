/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2019 Barcelona Supercomputing Center (BSC)
*/

#ifndef POLLING_HPP
#define POLLING_HPP

#include <GASPI.h>

#include "util/SpinLock.hpp"

#include <cstdint>


class Polling {
private:
	typedef util::SpinLock<> SpinLock;

	/* Number of requests checked per gaspi_request_wait call */
	static const unsigned int NREQ = 64;

	/* Number of queues per polling service */
	static const unsigned int QPPS = 4;

public:
	static void initialize();

	static void finalize();

	static int pollQueues(void *data);

	static int pollNotifications(void *data);
};

#endif // POLLING_HPP
