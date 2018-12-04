/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <GASPI.h>

#include "WaitingRangeList.hpp"
#include "WaitingRangeQueue.hpp"
#include "QueueGroup.hpp"
#include "SpinLock.hpp"

class Environment {
public:
	bool enabled;
	gaspi_number_t maxQueues;
	gaspi_number_t maxSegments;
	gaspi_number_t maxQueueGroups;
	gaspi_number_t numQueueGroups;
	WaitingRangeQueue *waitingRangeQueues;
	WaitingRangeList *waitingRangeLists;
	QueueGroup **queueGroups;
	SpinLock *queuePollingLocks;
	SpinLock notificationPollingLock;
	SpinLock queueGroupsLock;
	
	Environment() :
		enabled(false),
		maxQueues(0),
		maxSegments(0),
		maxQueueGroups(0),
		numQueueGroups(0),
		waitingRangeQueues(nullptr),
		waitingRangeLists(nullptr),
		queueGroups(nullptr),
		queuePollingLocks(nullptr),
		notificationPollingLock(),
		queueGroupsLock()
	{}
	
	static void initialize();
	
	static void finalize();
};

extern Environment _env;

#endif /* ENVIRONMENT_HPP */
