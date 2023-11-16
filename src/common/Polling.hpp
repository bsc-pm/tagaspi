/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2023 Barcelona Supercomputing Center (BSC)
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
		TaskingModel::PollingInstance *pollingInstance;
	};

	//! Number of requests checked per gaspi_request_wait call
	static constexpr unsigned int BatchSize = 64;

	//! The polling period used by polling instances
	static uint64_t _period;

	//! The information for each GASPI queues polling instance
	static std::vector<QueuePollingInfo> _queuePollingInfos;

	//! The handle to the polling instance that periodically checks
	//! the GASPI notifications in TAGASPI
	static TaskingModel::PollingInstance *_notificationsPollingInstance;

public:
	static void initialize();

	static void finalize();

	static uint64_t pollQueues(void *data);

	static uint64_t pollNotifications(void *data);
};

} // namespace tagaspi

#endif // POLLING_HPP
