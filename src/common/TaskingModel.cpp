/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2019-2020 Barcelona Supercomputing Center (BSC)
*/

#include "TaskingModel.hpp"
#include "util/SymbolResolver.hpp"

namespace tagaspi {

//! Enable/disable polling services
EnvironmentVariable<bool> TaskingModel::_usePollingServices("TAGASPI_POLLING_SERVICES", false);


void TaskingModel::initialize()
{
	// Try to load the functions required by polling tasks
	_waitFor = (wait_for_t *) SymbolResolver::load(_waitForName, false);
	_spawnFunction = (spawn_function_t *) SymbolResolver::load(_spawnFunctionName, false);

	// Switch to polling services if polling tasks are not available
	if (!_usePollingServices && (!_waitFor || !_spawnFunction)) {
		_usePollingServices.setValue(true);
	}

	// Load the polling service functions if needed
	if (_usePollingServices) {
		_registerPollingService = (register_polling_service_t *)
			SymbolResolver::load(_registerPollingServiceName);
		_unregisterPollingService = (unregister_polling_service_t *)
			SymbolResolver::load(_unregisterPollingServiceName);
	}

	// Load the task events API functions
	_getCurrentEventCounter = (get_current_event_counter_t *)
		SymbolResolver::load(_getCurrentEventCounterName);
	_increaseCurrentTaskEventCounter = (increase_current_task_event_counter_t *)
		SymbolResolver::load(_increaseCurrentTaskEventCounterName);
	_decreaseTaskEventCounter = (decrease_task_event_counter_t *)
		SymbolResolver::load(_decreaseTaskEventCounterName);

	_notifyTaskEventCounterAPI = (notify_task_event_counter_api_t *)
		SymbolResolver::load(_notifyTaskEventCounterAPIName, false);

	// Notify the tasking runtime that the event counters
	// API may be used during the execution. This function
	// is not required so call it only if defined
	if (_notifyTaskEventCounterAPI) {
		(*_notifyTaskEventCounterAPI)();
	}
}

//! The pointers to the tasking model API functions
register_polling_service_t *TaskingModel::_registerPollingService = nullptr;
unregister_polling_service_t *TaskingModel::_unregisterPollingService = nullptr;
get_current_event_counter_t *TaskingModel::_getCurrentEventCounter = nullptr;
increase_current_task_event_counter_t *TaskingModel::_increaseCurrentTaskEventCounter = nullptr;
decrease_task_event_counter_t *TaskingModel::_decreaseTaskEventCounter = nullptr;
notify_task_event_counter_api_t *TaskingModel::_notifyTaskEventCounterAPI = nullptr;
spawn_function_t *TaskingModel::_spawnFunction = nullptr;
wait_for_t *TaskingModel::_waitFor = nullptr;

//! Actual names of the tasking model API functions
const std::string TaskingModel::_registerPollingServiceName("nanos6_register_polling_service");
const std::string TaskingModel::_unregisterPollingServiceName("nanos6_unregister_polling_service");
const std::string TaskingModel::_getCurrentEventCounterName("nanos6_get_current_event_counter");
const std::string TaskingModel::_increaseCurrentTaskEventCounterName("nanos6_increase_current_task_event_counter");
const std::string TaskingModel::_decreaseTaskEventCounterName("nanos6_decrease_task_event_counter");
const std::string TaskingModel::_notifyTaskEventCounterAPIName("nanos6_notify_task_event_counter_api");
const std::string TaskingModel::_spawnFunctionName("nanos6_spawn_function");
const std::string TaskingModel::_waitForName("nanos6_wait_for");

} // namespace tagaspi
