/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2019 Barcelona Supercomputing Center (BSC)
*/

#ifndef TASKING_MODEL_HPP
#define TASKING_MODEL_HPP

#include <cassert>

#include "TaskingModelAPI.hpp"
#include "util/SymbolResolver.hpp"


class TaskingModel {
private:
	static register_polling_service_t *_registerPollingService;
	static unregister_polling_service_t *_unregisterPollingService;
	static get_current_event_counter_t *_getCurrentEventCounter;
	static increase_current_task_event_counter_t *_increaseCurrentTaskEventCounter;
	static decrease_task_event_counter_t *_decreaseTaskEventCounter;
	static notify_task_event_counter_api_t *_notifyTaskEventCounterAPI;

public:
	static inline void initialize()
	{
		_registerPollingService = (register_polling_service_t *)
			util::SymbolResolver::loadSymbol("nanos6_register_polling_service");
		_unregisterPollingService = (unregister_polling_service_t *)
			util::SymbolResolver::loadSymbol("nanos6_unregister_polling_service");
		_getCurrentEventCounter = (get_current_event_counter_t *)
			util::SymbolResolver::loadSymbol("nanos6_get_current_event_counter");
		_increaseCurrentTaskEventCounter = (increase_current_task_event_counter_t *)
			util::SymbolResolver::loadSymbol("nanos6_increase_current_task_event_counter");
		_decreaseTaskEventCounter = (decrease_task_event_counter_t *)
			util::SymbolResolver::loadSymbol("nanos6_decrease_task_event_counter");
		_notifyTaskEventCounterAPI = (notify_task_event_counter_api_t *)
			util::SymbolResolver::tryLoadSymbol("nanos6_notify_task_event_counter_api");
	}

	static inline void registerPollingService(const std::string &name, nanos6_polling_service_t service, void *data = nullptr)
	{
		assert(_registerPollingService);
		(*_registerPollingService)(name.c_str(), service, data);
	}

	static inline void unregisterPollingService(const std::string &name, nanos6_polling_service_t service, void *data = nullptr)
	{
		assert(_unregisterPollingService);
		(*_unregisterPollingService)(name.c_str(), service, data);
	}

	static inline void *getCurrentEventCounter()
	{
		assert(_getCurrentEventCounter);
		return (*_getCurrentEventCounter)();
	}

	static inline void increaseCurrentTaskEventCounter(void *counter, unsigned int increment)
	{
		assert(_increaseCurrentTaskEventCounter);
		(*_increaseCurrentTaskEventCounter)(counter, increment);
	}

	static inline void decreaseTaskEventCounter(void *counter, unsigned int decrement)
	{
		assert(_decreaseTaskEventCounter);
		(*_decreaseTaskEventCounter)(counter, decrement);
	}

	static inline void notifyTaskEventCounterAPI()
	{
		if (_notifyTaskEventCounterAPI) {
			(*_notifyTaskEventCounterAPI)();
		}
	}
};

#endif // TASKING_MODEL_HPP

