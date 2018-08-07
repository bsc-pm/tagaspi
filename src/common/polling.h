/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef POLLING_H
#define POLLING_H

#define NUM_REQUESTS ((unsigned int) 16)
#define QUEUES_PER_SERVICE ((unsigned int) 4)

void polling_initialize();
void polling_finalize();

#endif /* POLLING_H */
