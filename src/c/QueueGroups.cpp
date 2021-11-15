/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2020 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>
#include <TAGASPI.h>

#include "common/Environment.hpp"
#include "common/QueueGroup.hpp"
#include "common/TaskingModel.hpp"
#include "common/util/SpinLock.hpp"

#include <cassert>
#include <cstdio>
#include <mutex>

using namespace tagaspi;

#pragma GCC visibility push(default)

#ifdef __cplusplus
extern "C" {
#endif

gaspi_return_t
tagaspi_queue_group_create(const gaspi_queue_group_id_t queue_group_id,
			const gaspi_queue_id_t queue_begin,
			const gaspi_number_t queue_num,
			const gaspi_queue_group_policy_t policy)
{
	assert(queue_group_id < _env.maxQueueGroups);
	assert(queue_begin < _env.maxQueues);
	assert(queue_begin + queue_num <= _env.maxQueues);
	assert(queue_num > 0);

	std::lock_guard<SpinLock> guard(_env.queueGroupsLock);

	if (_env.numQueueGroups >= _env.maxQueueGroups) {
		fprintf(stderr, "Error: Too many queue groups created\n");
		return GASPI_ERROR;
	}

	if (_env.queueGroups[queue_group_id] != nullptr) {
		fprintf(stderr, "Error: Queue group %d already exists\n", queue_group_id);
		return GASPI_ERROR;
	}

	if (!QueueGroup::isValidPolicy(policy)) {
		fprintf(stderr, "Error: Queue group policy is not valid\n");
		return GASPI_ERROR;
	}

	QueueGroup *queueGroup = new QueueGroup(queue_begin, queue_num);
	assert(queueGroup != nullptr);

	queueGroup->setupPolicy(policy);

	_env.queueGroups[queue_group_id] = queueGroup;
	_env.numQueueGroups += 1;

	return GASPI_SUCCESS;
}

gaspi_return_t
tagaspi_queue_group_delete(const gaspi_queue_group_id_t queue_group_id)
{
	assert(queue_group_id < _env.maxQueueGroups);

	std::lock_guard<SpinLock> guard(_env.queueGroupsLock);

	QueueGroup *queueGroup = _env.queueGroups[queue_group_id];
	if (queueGroup == nullptr) {
		fprintf(stderr, "Error: Queue group %d does not exist\n", queue_group_id);
		return GASPI_ERROR;
	}
	delete queueGroup;

	_env.queueGroups[queue_group_id] = nullptr;
	_env.numQueueGroups -= 1;

	return GASPI_SUCCESS;
}

gaspi_return_t
tagaspi_queue_group_get_queue(const gaspi_queue_group_id_t queue_group_id,
			gaspi_queue_id_t * const queue)
{
	assert(queue_group_id < _env.maxQueueGroups);
	assert(queue != NULL);

	QueueGroup *queueGroup = _env.queueGroups[queue_group_id];
	if (queueGroup == nullptr) {
		fprintf(stderr, "Error: Queue group %d does not exist\n", queue_group_id);
		return GASPI_ERROR;
	}
	*queue = queueGroup->getQueue();

	return GASPI_SUCCESS;
}

gaspi_return_t
tagaspi_queue_group_max(gaspi_number_t * const queue_group_max)
{
	assert(queue_group_max != NULL);

	*queue_group_max = _env.maxQueueGroups;

	return GASPI_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#pragma GCC visibility pop
