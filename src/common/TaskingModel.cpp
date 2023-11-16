/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023 Barcelona Supercomputing Center (BSC)
*/

#include "Symbol.hpp"
#include "TaskingModel.hpp"
#include "util/ErrorHandler.hpp"

namespace tagaspi {

void TaskingModel::initialize()
{
	_alpi_version_check.load();
	_alpi_version_get.load();
	_alpi_task_self.load();
	_alpi_task_spawn.load();
	_alpi_task_waitfor_ns.load();
	_alpi_task_events_increase.load();
	_alpi_task_events_decrease.load();

	int expected[2] = { ALPI_VERSION_MAJOR, ALPI_VERSION_MINOR };
	int provided[2];

	if (int err = _alpi_version_get(&provided[0], &provided[1]))
		ErrorHandler::fail("Failed alpi_version_get: ", getError(err));

	int err = _alpi_version_check(expected[0], expected[1]);
	if (err == ALPI_ERR_VERSION)
		ErrorHandler::fail("Incompatible ALPI tasking interface versions:\n",
			"\tExpected: ", expected[0], ".", expected[1], "\n",
			"\tProvided: ", provided[0], ".", provided[1]);
	else if (err)
		ErrorHandler::fail("Failed alpi_version_check: ", getError(err));
}

Symbol<TaskingModel::alpi_error_string_t>
TaskingModel::_alpi_error_string("alpi_error_string");

Symbol<TaskingModel::alpi_version_check_t>
TaskingModel::_alpi_version_check("alpi_version_check");

Symbol<TaskingModel::alpi_version_get_t>
TaskingModel::_alpi_version_get("alpi_version_get");

Symbol<TaskingModel::alpi_task_self_t>
TaskingModel::_alpi_task_self("alpi_task_self");

Symbol<TaskingModel::alpi_task_events_increase_t>
TaskingModel::_alpi_task_events_increase("alpi_task_events_increase");

Symbol<TaskingModel::alpi_task_events_decrease_t>
TaskingModel::_alpi_task_events_decrease("alpi_task_events_decrease");

Symbol<TaskingModel::alpi_task_waitfor_ns_t>
TaskingModel::_alpi_task_waitfor_ns("alpi_task_waitfor_ns");

Symbol<TaskingModel::alpi_task_spawn_t>
TaskingModel::_alpi_task_spawn("alpi_task_spawn");

} // namespace tagaspi
