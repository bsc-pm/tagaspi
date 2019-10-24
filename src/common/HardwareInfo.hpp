/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2019 Barcelona Supercomputing Center (BSC)
*/

#ifndef HARDWARE_INFO_HPP
#define HARDWARE_INFO_HPP

#include <cassert>
#include <vector>

#include <numa.h>
#include <sched.h>


class HardwareInfo {
private:
	static std::vector<int> _cpuToNUMANode;
	static std::vector<bool> _numaNodeAvailability;
	static size_t _numAvailableNUMANodes;
	
	HardwareInfo() = delete;
	
public:
	static inline void initialize()
	{
		assert(_cpuToNUMANode.empty());
		assert(_numaNodeAvailability.empty());
		
		const size_t maxNUMAs = getMaxNUMANodes();
		const size_t maxCPUs = getMaxCPUs();
		
		_cpuToNUMANode.resize(maxCPUs, -1);
		_numaNodeAvailability.resize(maxNUMAs, false);
		_numAvailableNUMANodes = 0;
		
		int maxAvailableCPU = -1;
		int maxAvailableNUMA = -1;
		for (size_t c = 0; c < maxCPUs; ++c) {
			if (numa_bitmask_isbitset(numa_all_cpus_ptr, c)) {
				int numa = numa_node_of_cpu(c);
				assert(numa >= 0);
				assert(numa < maxNUMAs);
				
				_cpuToNUMANode[c] = numa;
				
				if (!_numaNodeAvailability[numa]) {
					_numaNodeAvailability[numa] = true;
					++_numAvailableNUMANodes;
				}
				
				maxAvailableCPU = c;
				if (maxAvailableNUMA < numa) {
					maxAvailableNUMA = numa;
				}
			}
		}
		assert(_numAvailableNUMAs > 0);
		assert(maxAvailableCPU >= 0);
		assert(maxAvailableCPU < maxCPUs);
		assert(maxAvailableCPU + 1 == numa_num_task_cpus());
		assert(maxAvailableNUMA >= 0);
		assert(maxAvailableNUMA < maxNUMAs);
		
		_cpuToNUMANode.resize(maxAvailableCPU + 1);
		_numaNodeAvailability.resize(maxAvailableNUMA + 1);

		// Try to save space
		_cpuToNUMANode.shrink_to_fit();
		_numaNodeAvailability.shrink_to_fit();
	}
	
	static inline void finalize()
	{
	}
	
	static inline size_t getMaxCPUs()
	{
		return _cpuToNUMANode.size();
	}
	
	static inline size_t getNumAvailableCPUs()
	{
		return numa_num_task_cpus();
	}
	
	static inline size_t getCurrentCPU()
	{
		size_t currentCPU = sched_getcpu();
		assert(currentCPU < getMaxCPUs());
		return currentCPU;
	}
	
	static inline size_t getMaxNUMANodes()
	{
		return _numaNodeAvailability.size();
	}
	
	static inline size_t getNumAvailableNUMANodes()
	{
		return _numAvailableNUMANodes;
	}
	
	static inline const std::vector<int> &getCPUToNUMANode()
	{
		return _cpuToNUMANode;
	}
	
	static inline const std::vector<bool> &getNUMANodeAvailability()
	{
		return _numaNodeAvailability;
	}
};


#endif // HARDWARE_INFO_HPP
