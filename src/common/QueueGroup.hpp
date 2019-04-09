/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef QUEUE_GROUP_HPP
#define QUEUE_GROUP_HPP

#include <GASPI.h>
#include <TAGASPI.h>

#include <config.h>

#include "RuntimeAPI.hpp"
#include "util/RuntimeInfo.hpp"
#include "util/Utils.hpp"

#include <atomic>
#include <cassert>
#include <cstddef>
#include <sys/sysinfo.h>


class QueueGroup {
private:
	typedef gaspi_queue_id_t queue_id_t;
	typedef gaspi_number_t number_t;
	typedef gaspi_queue_group_policy_t policy_t;
	
	queue_id_t _firstQueue;
	number_t _numQueues;
	policy_t _policy;
	void *_data;
	
public:
	inline QueueGroup(queue_id_t first, number_t num) :
		_firstQueue(first),
		_numQueues(num),
		_data(nullptr)
	{
		assert(num > 0);
	}
	
	inline ~QueueGroup()
	{
		if (_data != nullptr) {
			cleanupPolicyData();
		}
	}
	
	inline queue_id_t getQueue()
	{
		queue_id_t queue = _firstQueue;
		if (_policy == GASPI_QUEUE_GROUP_POLICY_DEFAULT) {
			if (_numQueues > 1) {
				assert(_data != nullptr);
				std::atomic<number_t> &counter = *((std::atomic<number_t> *)_data);
				
				number_t offset = counter.load();
				number_t nextOffset = (offset < _numQueues - 1) ? offset + 1 : 0;
				queue = _firstQueue + offset;
				
				/* In case the operation fails, another thread will update it */
				counter.compare_exchange_strong(offset, nextOffset);
			}
		}
#if HAVE_RUNTIME_CPU_INFO
		else {
			size_t cpu = nanos6_get_current_virtual_cpu();
			gaspi_queue_id_t *queues = (gaspi_queue_id_t *)_data;
			assert(queues != nullptr);
			queue = queues[cpu];
		}
#endif
		return queue;
	}
	
	inline void setupPolicy(policy_t policy)
	{
		assert(QueueGroup::isValidPolicy(policy));
		_policy = policy;
		
		if (_policy == GASPI_QUEUE_GROUP_POLICY_DEFAULT) {
			_data = new std::atomic<number_t>(0);
			assert(_data != nullptr);
		}
#if HAVE_RUNTIME_CPU_INFO
		else {
			size_t numCPUs, numNUMAs;
			size_t numaIDs[get_nprocs_conf()];
			util::mask_t numaMask;
			
			/* Get system information from the runtime system */
			util::getSystemUsageInfo(&numCPUs, numaIDs);
			
			_data = new queue_id_t[numCPUs];
			assert(_data != nullptr);
			
			/* Build the mask of the NUMA nodes used */
			MASK_RESET(numaMask);
			for (size_t cpu = 0; cpu < numCPUs; ++cpu) {
				assert(numaIDs[cpu] < MASK_BITS(numaMask));
				MASK_SET(numaMask, numaIDs[cpu]);
			}
			
			numNUMAs = MASK_COUNT(numaMask);
			assert(numNUMAs > 0);
			
			setupCPURoundRobinData(numCPUs, numNUMAs, numaIDs, numaMask);
		}
#endif
	}
	
	static inline bool isValidPolicy(policy_t policy)
	{
#if HAVE_RUNTIME_CPU_INFO
		return (policy == GASPI_QUEUE_GROUP_POLICY_DEFAULT
             || policy == GASPI_QUEUE_GROUP_POLICY_CPU_RR);
#else
		return (policy == GASPI_QUEUE_GROUP_POLICY_DEFAULT);
#endif
	}
	
private:
	struct queue_range_t {
		queue_id_t first;
		number_t num;
		inline queue_range_t() : first(0), num(0) {}
	};
	
	inline void setupCPURoundRobinData(size_t numCPUs, size_t numNUMAs, const size_t *numaIDs, util::mask_t numaMask)
	{
		const size_t queuesPerNUMA = _numQueues / numNUMAs;
		const size_t remainingQueues = _numQueues % numNUMAs;
		const size_t maxNUMAs = MASK_BITS(numaMask);
		
		queue_range_t numaQueues[maxNUMAs];
		
		/* Assigns distinct ranges of queues to NUMA nodes if possible */
		if (_numQueues >= numNUMAs) {
			queue_id_t queue = _firstQueue;
			for (size_t numa = 0; numa < maxNUMAs; ++numa) {
				/* Assign them to NUMAs which are being used by this rank */
				if (MASK_ISSET(numaMask, numa)) {
					numaQueues[numa].first = queue;
					numaQueues[numa].num = queuesPerNUMA + (numa < remainingQueues);
					queue += numaQueues[numa].num;
				}
	    	}
		} else {
			/* Assign one-to-one all queues, except the last, to the first NUMAs */
			size_t numAssigned = 0, numa = 0;
			while (numAssigned < remainingQueues - 1) {
				if (MASK_ISSET(numaMask, numa)) {
					numaQueues[numa].first = (queue_id_t) numAssigned;
					numaQueues[numa].num = 1;
					++numAssigned;
				}
			}
			assert(numAssigned < numNUMAs);
			
			/* Assign the last queue to the rest of NUMAs */
			for (size_t numa = remainingQueues - 1; numa < maxNUMAs; ++numa) {
				if (MASK_ISSET(numaMask, numa)) {
					numaQueues[numa].first = (queue_id_t) (remainingQueues - 1);
					numaQueues[numa].num = 1;
				}
			}
		}
		
		queue_id_t *assignedQueue = (queue_id_t *) _data;
		assert(assignedQueue != nullptr);
		
		/* Assigns queues to CPUs in Round-Robin */
		for (size_t numa = 0; numa < maxNUMAs; ++numa) {
			if (MASK_ISSET(numaMask, numa)) {
				const number_t firstQueue = numaQueues[numa].first;
				const number_t numQueues  = numaQueues[numa].num;
				number_t offset = 0;
				
				for (size_t cpu = 0; cpu < numCPUs; ++cpu) {
					if (numaIDs[cpu] == numa) {
						assignedQueue[cpu] = firstQueue + offset;
						offset = (offset + 1) % numQueues;
					}
				}
			}
		}
	}
	
	inline void cleanupPolicyData()
	{
		assert(_data != nullptr);
		
		if (_policy == GASPI_QUEUE_GROUP_POLICY_DEFAULT) {
			std::atomic<number_t> *counter = (std::atomic<number_t> *)_data;
			assert(counter != nullptr);
			delete counter;
		}
#if HAVE_RUNTIME_CPU_INFO
		else {
			queue_id_t *queueIDs = (queue_id_t *)_data;
			assert(queueIDs != nullptr);
			delete [] queueIDs;
		}
#endif
	}
};

#endif // QUEUE_GROUP_HPP
