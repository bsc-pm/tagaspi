/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2019 Barcelona Supercomputing Center (BSC)
*/

#ifndef TASKING_MODEL_API_HPP
#define TASKING_MODEL_API_HPP


extern "C" {
	/* Polling service API */
	
	//! \brief Function that the runtime calls periodically
	//!
	//! \param service_data a pointer to data that the function uses and that
	//! also identifies an instance of this service registration
	//!
	//! \returns true to signal that the purpose of the function has been
	//! achieved and that the function should not be called again with the given
	//! service_data
	typedef int (*nanos6_polling_service_t)(void *data);
	
	//! \brief Register a function and parameter of that function that the runtime
	//! must call periodically to perform operations in a non-blocking fashion
	//!
	//! Registers a function that the runtime will called periodically. The  purpose
	//! is to support operations in a non-blocking way. For instance to check for
	//! certain events that are not possible to check in a blocking way, or that it
	//! is not desirable to do so.
	//!
	//! The service_data parameter is an opaque pointer that is passed to the function
	//! as is.
	//!
	//! The same function can be registered several times with different service_data
	//! parameters, and each combination will be considered as a different
	//! registration. That is, a registered service instance consists of a service
	//! function and a specific service_data.
	//!
	//! The service will remain registered as long as the function returns false and
	//! the service has not been explicitly deregistered through a call to
	//! nanos6_unregister_polling_service.
	//!
	//! \param[in] service_name a string that identifies the kind of service that will
	//! be serviced
	//! \param[in] service the function that the runtime should call periodically
	//! \param service_data an opaque pointer to data that is passed to the service
	//! function
	void nanos6_register_polling_service(char const *name, nanos6_polling_service_t service, void *data);
	
	//! \brief Unregister a function and parameter of that function previously
	//! registered through a call to nanos6_register_polling_service.
	//!
	//! Unregister a service instance identified by the service_function and
	//! service_data previously registered through a call to
	//! nanos6_register_polling_service. The service_function must not have returned
	//! true when called with the given service_data, since that return value is
	//! equivalent to a call to this function.
	//!
	//! \param[in] service_name a string that identifies the kind of service
	//! \param[in] service the function that the runtime should stop calling
	//! periodically with the given service_data
	//! \param service_data an opaque pointer to the data that was passed to the
	//! service function
	void nanos6_unregister_polling_service(char const *name, nanos6_polling_service_t service, void *data);
	
	
	/* External Events API */
	
	//! \brief Get the event counter associated with the current task
	//!
	//! \returns the associated event counter with the executing task
	void *nanos6_get_current_event_counter(void);
	
	//! \brief Increase the counter of events of the current task to prevent the release of dependencies
	//!
	//! This function atomically increases the counter of events of a task
	//!
	//! \param[in] event_counter The event counter according with the current task
	//! \param[in] value The value to be incremented (must be positive or zero)
	void nanos6_increase_current_task_event_counter(void *event_counter, unsigned int increment);
	
	//! \brief Decrease the counter of events of a task and release the dependencies if required
	//!
	//! This function atomically decreases the counter of events of a task and
	//! it releases the depencencies once the number of events becomes zero
	//! and the task has completed its execution
	//!
	//! \param[in] event_counter The event counter of the task
	//! \param[in] value The value to be decremented (must be positive or zero)
	void nanos6_decrease_task_event_counter(void *event_counter, unsigned int decrement);
	
	//! \brief Notify that the external events API could be used
	//!
	//! This function notifies the runtime system as soon as possible
	//! that the external events API could be used from now on. This
	//! function is required when TAMPI is configured with the option
	//! "--enable-external-events-api-notification".
	void nanos6_notify_task_event_counter_api(void);
	
	//! Prototypes of the API functions
	typedef void register_polling_service_t(char const *, nanos6_polling_service_t, void *);
	typedef void unregister_polling_service_t(char const *, nanos6_polling_service_t, void *);
	typedef void*get_current_blocking_context_t(void);
	typedef void block_current_task_t(void*);
	typedef void unblock_task_t(void*);
	typedef void*get_current_event_counter_t(void);
	typedef void increase_current_task_event_counter_t(void*, unsigned int);
	typedef void decrease_task_event_counter_t(void*, unsigned int);
	typedef void notify_task_event_counter_api_t(void);
}

#endif // TASKING_MODEL_API_HPP

