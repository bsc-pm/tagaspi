/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2021 Barcelona Supercomputing Center (BSC)
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

EnvironmentVariable<uint64_t> Polling::_queuePollingInstances("TAGASPI_QUEUE_CHECKERS", 1);
EnvironmentVariable<uint64_t> Polling::_pollingFrequency("TAGASPI_POLLING_FREQUENCY", 100);
Polling::QueuePollingInfo *Polling::_queuePollingInfos;
TaskingModel::polling_handle_t Polling::_notificationsPollingHandle;

void Polling::initialize()
{
	assert(_env.maxQueues > 0);
	assert(_queuePollingInstances > 0);

	_queuePollingInfos = new QueuePollingInfo[_queuePollingInstances];
	assert(_queuePollingInfos != nullptr);

	gaspi_number_t qppi = _env.maxQueues / _queuePollingInstances;
	gaspi_number_t remq = _env.maxQueues % _queuePollingInstances;

	gaspi_queue_id_t queue = 0;
	for (gaspi_number_t ins = 0; ins < _queuePollingInstances; ++ins) {
		QueuePollingInfo *info = &_queuePollingInfos[ins];

		info->numQueues = qppi + (ins < remq);
		if (info->numQueues > 0) {
			info->firstQueue = queue;

			std::string name = std::string("TAGASPI QUEUES ") + std::to_string(ins);
			info->pollingHandle = TaskingModel::registerPolling(
				name.c_str(), pollQueues, info,
				_pollingFrequency
			);
			queue += info->numQueues;
		}
	}

	_notificationsPollingHandle = TaskingModel::registerPolling(
		"TAGASPI NOTIFICATIONS", pollNotifications, nullptr,
		_pollingFrequency
	);
}

void Polling::finalize()
{
	for (gaspi_number_t ins = 0; ins < _queuePollingInstances; ++ins) {
		if (_queuePollingInfos[ins].numQueues) {
			TaskingModel::unregisterPolling(_queuePollingInfos[ins].pollingHandle);
		}
	}
	TaskingModel::unregisterPolling(_notificationsPollingHandle);

	delete [] _queuePollingInfos;
}

void Polling::pollQueues(void *data)
{
	QueuePollingInfo *info = (QueuePollingInfo *) data;
	assert(info != nullptr);

	gaspi_queue_id_t queue = info->firstQueue;
	gaspi_number_t numQueues = queue + info->numQueues;

	assert(numQueues > 0);
	assert(queue < _env.maxQueues);
	assert(numQueues <= _env.maxQueues);

	gaspi_number_t completedReqs, r;
	gaspi_status_t statuses[NREQ];
	gaspi_tag_t tags[NREQ];
	gaspi_return_t eret;

	for (; queue < numQueues; ++queue) {
		do {
			eret = gaspi_request_wait(queue, NREQ, &completedReqs, tags, statuses, GASPI_TEST);
			if (eret != GASPI_SUCCESS && eret != GASPI_TIMEOUT) {
				// We are probably cheking queues that are not created
				if (eret != GASPI_ERR_INV_QUEUE) {
					fprintf(stderr, "Error: Return code %d from gaspi_request_wait\n", eret);
					abort();
				}
				completedReqs = 0;
				continue;
			}
			assert(completedReqs <= NREQ);

			for (r = 0; r < completedReqs; ++r) {
				if (statuses[r].error != GASPI_SUCCESS) {
					fprintf(stderr, "Error: GASPI operation with tag %lld failed\n", tags[r]);
					abort();
				}

				if (tags[r] != GASPI_TAG_NULL) {
					void *eventCounter = (void *) tags[r];
					assert(eventCounter != nullptr);

					TaskingModel::decreaseTaskEventCounter(eventCounter, 1);
				}
			}
		} while (completedReqs == NREQ);
	}
}

void Polling::pollNotifications(void *)
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
}

} // namespace tagaspi
