/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2021 Barcelona Supercomputing Center (BSC)
*/

#ifndef POLLING_HPP
#define POLLING_HPP

#include <GASPI.h>

#include "TaskingModel.hpp"
#include "util/EnvironmentVariable.hpp"
#include "util/SpinLock.hpp"

#include <cstdint>
#include <vector>

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

	//! The information for each GASPI queues polling instance
	static std::vector<QueuePollingInfo> _queuePollingInfos;

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
