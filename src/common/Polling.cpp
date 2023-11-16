/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2023 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <GASPI_Lowlevel.h>

#include "Allocator.hpp"
#include "Environment.hpp"
#include "Polling.hpp"
#include "TaskingModel.hpp"
#include "WaitingRange.hpp"
#include "WaitingRangeList.hpp"
#include "WaitingRangeQueue.hpp"
#include "util/Utils.hpp"

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

namespace tagaspi {

uint64_t Polling::_period = 100;
std::vector<Polling::QueuePollingInfo> Polling::_queuePollingInfos;
TaskingModel::PollingInstance *Polling::_notificationsPollingInstance;

void Polling::initialize()
{
	assert(_env.maxQueues > 0);
	assert(queuePollingInstances > 0);

	// The TAGASPI_QUEUE_CHECKERS envar determines the number of polling instances
	// to check progress and completion of queues
	EnvironmentVariable<uint64_t> queuePollingInstances("TAGASPI_QUEUE_CHECKERS", 1);

	_queuePollingInfos.resize(queuePollingInstances);

	gaspi_number_t qppi = _env.maxQueues / queuePollingInstances;
	gaspi_number_t remq = _env.maxQueues % queuePollingInstances;

	// The TAGASPI_POLLING_PERIOD envar determines the period in which TAGASPI
	// will check its internal requests. If not defined, the period is 100us
	// as a default value. The TAGASPI_POLLING_FREQUENCY is deprecated now
	EnvironmentVariable<uint64_t> periodEnvar("TAGASPI_POLLING_PERIOD");
	EnvironmentVariable<uint64_t> frequencyEnvar("TAGASPI_POLLING_FREQUENCY");

	// Give always priority to TAGASPI_POLLING_PERIOD
	if (periodEnvar.isPresent())
		_period = periodEnvar.getValue();
	else if (frequencyEnvar.isPresent())
		_period = frequencyEnvar.getValue();

	gaspi_queue_id_t queue = 0;
	for (gaspi_number_t ins = 0; ins < queuePollingInstances; ++ins) {
		QueuePollingInfo *info = &_queuePollingInfos[ins];

		info->numQueues = qppi + (ins < remq);
		if (info->numQueues > 0) {
			info->firstQueue = queue;

			std::string name = std::string("TAGASPI QUEUES ") + std::to_string(ins);
			info->pollingInstance =
				TaskingModel::registerPolling(name.c_str(), pollQueues, info);
			queue += info->numQueues;
		}
	}

	_notificationsPollingInstance =
		TaskingModel::registerPolling("TAGASPI NOTIFICATIONS", pollNotifications, nullptr);
}

void Polling::finalize()
{
	for (QueuePollingInfo &info : _queuePollingInfos) {
		if (info.numQueues)
			TaskingModel::unregisterPolling(info.pollingInstance);
	}
	TaskingModel::unregisterPolling(_notificationsPollingInstance);

	_queuePollingInfos.clear();
}

uint64_t Polling::pollQueues(void *data)
{
	QueuePollingInfo *info = (QueuePollingInfo *) data;
	assert(info != nullptr);

	gaspi_queue_id_t queue = info->firstQueue;
	gaspi_number_t numQueues = queue + info->numQueues;

	assert(numQueues > 0);
	assert(queue < _env.maxQueues);
	assert(numQueues <= _env.maxQueues);

	gaspi_number_t completedReqs, r;
	gaspi_status_t statuses[BatchSize];
	gaspi_tag_t tags[BatchSize];
	gaspi_return_t eret;

	for (; queue < numQueues; ++queue) {
		do {
			eret = gaspi_request_wait(queue, BatchSize, &completedReqs, tags, statuses, GASPI_TEST);
			if (eret != GASPI_SUCCESS && eret != GASPI_TIMEOUT) {
				// We are probably cheking queues that are not created
				if (eret != GASPI_ERR_INV_QUEUE) {
					fprintf(stderr, "Error: Return code %d from gaspi_request_wait\n", eret);
					abort();
				}
				completedReqs = 0;
				continue;
			}
			assert(completedReqs <= BatchSize);

			for (r = 0; r < completedReqs; ++r) {
				if (statuses[r].error != GASPI_SUCCESS) {
					fprintf(stderr, "Error: GASPI operation with tag %lld failed\n", tags[r]);
					abort();
				}

				if (tags[r] != GASPI_TAG_NULL) {
					TaskingModel::task_handle_t task = (TaskingModel::task_handle_t) tags[r];
					assert(task != nullptr);

					TaskingModel::decreaseTaskEvents(task, 1);
				}
			}
		} while (completedReqs == BatchSize);
	}

	return _period;
}

uint64_t Polling::pollNotifications(void *)
{
	std::vector<WaitingRange*> completeRanges;

	gaspi_segment_id_t seg;
	for (seg = 0; seg < _env.maxSegments; ++seg) {
		WaitingRangeQueue &queue = _env.waitingRangeQueues[seg];
		WaitingRangeList &list = _env.waitingRangeLists[seg];

		queue.dequeueAll(list);
		list.checkNotifications(completeRanges);

		if (completeRanges.empty())
			continue;

		for (WaitingRange *range : completeRanges) {
			assert(range != nullptr);

			range->complete();

			Allocator<WaitingRange>::free(range);
		}
		completeRanges.clear();
	}

	return _period;
}

} // namespace tagaspi
