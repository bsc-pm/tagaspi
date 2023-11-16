/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2023 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <GASPI_Lowlevel.h>

#include "Allocator.hpp"
#include "Environment.hpp"
#include "HardwareInfo.hpp"
#include "Polling.hpp"
#include "TaskingModel.hpp"
#include "WaitingRange.hpp"
#include "util/SpinLock.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>

namespace tagaspi {

Environment _env;

std::vector<int> HardwareInfo::_cpuToNUMANode;
std::vector<bool> HardwareInfo::_numaNodeAvailability;
size_t HardwareInfo::_numAvailableNUMANodes;

void Environment::initialize()
{
	assert(!_env.enabled);

	HardwareInfo::initialize();
	TaskingModel::initialize();

	gaspi_queue_max(&_env.maxQueues);
	gaspi_segment_max(&_env.maxSegments);
	assert(_env.maxQueues > 0);
	assert(_env.maxSegments > 0);

	_env.waitingRangeQueues = new WaitingRangeQueue[_env.maxSegments];
	assert(_env.waitingRangeQueues != nullptr);

	_env.waitingRangeLists = new WaitingRangeList[_env.maxSegments];
	assert(_env.waitingRangeLists != nullptr);

	_env.maxQueueGroups = MaxQueueGroups;
	_env.numQueueGroups = 0;

	gaspi_operation_get_num_requests(GASPI_OP_READ, 1, &_env.numRequests[Operation::READ]);
	gaspi_operation_get_num_requests(GASPI_OP_WRITE, 1, &_env.numRequests[Operation::WRITE]);
	gaspi_operation_get_num_requests(GASPI_OP_NOTIFY, 1, &_env.numRequests[Operation::NOTIFY]);
	gaspi_operation_get_num_requests(GASPI_OP_WRITE_NOTIFY, 1, &_env.numRequests[Operation::WRITE_NOTIFY]);

	_env.queueGroups = new QueueGroup*[_env.maxQueueGroups]();
	assert(_env.queueGroups != nullptr);

	_env.queuePollingLocks = new SpinLock[_env.maxQueues];
	assert(_env.queuePollingLocks != nullptr);

	Allocator<WaitingRange>::initialize();

	_env.enabled = true;
	std::atomic_thread_fence(std::memory_order_seq_cst);

	Polling::initialize();
}

void Environment::finalize()
{
	assert(_env.enabled);
	assert(_env.queuePollingLocks != nullptr);
	assert(_env.waitingRangeQueues != nullptr);
	assert(_env.waitingRangeLists != NULL);
	assert(_env.queueGroups != NULL);

	Polling::finalize();

	Allocator<WaitingRange>::finalize();

	delete [] _env.queuePollingLocks;
	delete [] _env.waitingRangeQueues;
	delete [] _env.waitingRangeLists;
	delete [] _env.queueGroups;

	_env.enabled = false;
	std::atomic_thread_fence(std::memory_order_seq_cst);

	HardwareInfo::finalize();
}

} // namespace tagaspi

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

