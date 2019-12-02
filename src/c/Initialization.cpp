/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2019 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>

#include "common/Environment.hpp"

#ifdef __cplusplus
extern "C" {
#endif

gaspi_return_t
tagaspi_proc_init(const gaspi_timeout_t timeout_ms)
{
	assert(!_env.enabled);

	gaspi_return_t eret;
	eret = gaspi_proc_init(timeout_ms);
	if (eret == GASPI_SUCCESS) {
		Environment::initialize();
	}
	return eret;
}

gaspi_return_t
tagaspi_proc_term(const gaspi_timeout_t timeout_ms)
{
	if (_env.enabled) {
		Environment::finalize();
	}
	return gaspi_proc_term(timeout_ms);
}

#ifdef __cplusplus
}
#endif

