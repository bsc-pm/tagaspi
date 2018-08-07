/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <GASPI.h>

#include "types.h"
#include "utils.h"

tagaspi_environment_t glb_env;

/* Function declarations */
void initialize();
void finalize();

#endif /* ENVIRONMENT_H */
