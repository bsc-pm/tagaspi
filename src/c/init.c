/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <GASPI.h>

#include "environment.h"

gaspi_return_t
tagaspi_proc_init(const gaspi_timeout_t timeout_ms)
{
	assert(!glb_env.enabled);
	
	gaspi_return_t eret;
	eret = gaspi_proc_init(timeout_ms);
	if (eret == GASPI_SUCCESS) initialize();
	return eret;
}

gaspi_return_t
tagaspi_proc_term(const gaspi_timeout_t timeout_ms)
{
	if (glb_env.enabled) finalize();
	return gaspi_proc_term(timeout_ms);
}
