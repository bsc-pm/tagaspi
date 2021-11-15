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

namespace tagaspi {

class Polling {
private:
	struct QueuePollingInfo {
		gaspi_queue_id_t firstQueue;
		gaspi_number_t numQueues;
		TaskingModel::polling_handle_t pollingHandle;
	};

	//! Number of requests checked per gaspi_request_wait call
	static const unsigned int NREQ = 64;

	//! Number of polling instances to check queues' completion. This
	//! environment variable is called TAGASPI_QUEUE_CHECKERS and the
	//! default value is 4
	static EnvironmentVariable<uint64_t> _queuePollingInstances;

	//! Determine the polling frequency when the TAGASPI polling is
	//! implemented with tasks that are paused periodically. That is
	//! the frequency in time (microseconds) at which the in-flight
	//! GASPI requests are checked in TAGASPI. This environment variable
	//! is called TAGASPI_POLLING_FREQUENCY and the default value is
	//! 500 microseconds. Note that this frequency is ignored when
	//! using polling services; instead the environment variable
	//! NANOS6_POLLING_FREQUENCY should be used for OmpSs-2
	static EnvironmentVariable<uint64_t> _pollingFrequency;

	//! The information for each GASPI queues polling instance
	static QueuePollingInfo *_queuePollingInfos;

	//! The handle to the polling instance that periodically checks
	//! the GASPI notifications in TAGASPI
	static TaskingModel::polling_handle_t _notificationsPollingHandle;

public:
	static void initialize();

	static void finalize();

	static void pollQueues(void *data);

	static void pollNotifications(void *data);
};

} // namespace tagaspi

#endif // POLLING_HPP
