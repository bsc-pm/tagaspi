/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef RUNTIME_INFO_HPP
#define RUNTIME_INFO_HPP

#include "common/RuntimeAPI.hpp"


namespace util {
#if HAVE_RUNTIME_CPU_INFO
	static inline void getSystemUsageInfo(size_t *numCPUs, size_t *numaIDs)
	{
		assert(numCPUs != nullptr);
		assert(numaIDs != nullptr);
		
		*numCPUs = nanos6_get_num_cpus();
		assert(*numCPUs > 0);
		
		size_t cpu = 0;
		void *cpuIt = nanos6_cpus_begin();
		while (cpuIt != nanos6_cpus_end()) {
			numaIDs[cpu] = nanos6_cpus_get_numa(cpuIt);
			cpuIt = nanos6_cpus_advance(cpuIt);
			++cpu;
		}
		assert(*numCPUs == cpu);
	}
#endif
};


#endif // RUNTIME_INFO_HPP
