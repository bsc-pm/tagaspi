/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2020 Barcelona Supercomputing Center (BSC)
*/

#ifndef POLLING_HPP
#define POLLING_HPP

#include <GASPI.h>

#include "TaskingModel.hpp"
#include "util/EnvironmentVariable.hpp"
#include "util/SpinLock.hpp"

#include <cstdint>


class Polling {
private:
	//! Number of requests checked per gaspi_request_wait call
	static const unsigned int NREQ = 64;

	//! Number of queues per polling instance
	static const unsigned int QPPI = 4;

	//! Determine the polling frequency when the TAGASPI polling is
	//! implemented with tasks that are paused periodically. That is
	//! the frequency in time (microseconds) at which the in-flight
	//! GASPI requests are checked in TAGASPI. This environment variable
	//! is called TAGASPI_POLLING_FREQUENCY and the default value is
	//! 500 microseconds. Note that this frequency is ignored when
	//! using polling services; instead the environment variable
	//! NANOS6_POLLING_FREQUENCY should be used for OmpSs-2
	static EnvironmentVariable<uint64_t> _pollingFrequency;

	//! The handle to the polling instance that periodically checks
	//! the queues of GASPI requests in TAGASPI
	static TaskingModel::polling_handle_t *_queuesPollingHandles;

	//! The handle to the polling instance that periodically checks
	//! the GASPI notifications in TAGASPI
	static TaskingModel::polling_handle_t _notificationsPollingHandle;

public:
	static void initialize();

	static void finalize();

	static void pollQueues(void *data);

	static void pollNotifications(void *data);
};

#endif // POLLING_HPP
