#ifndef RUNTIME_API_H
#define RUNTIME_API_H

/* Polling service API */
typedef int (*nanos_polling_service_t)(void *data);
void nanos_register_polling_service(char const *name, nanos_polling_service_t service, void *data);
void nanos_unregister_polling_service(char const *name, nanos_polling_service_t service, void *data);

/* Pause/Resume API */
void *nanos_get_current_blocking_context();
void nanos_block_current_task(void *blocking_context);
void nanos_unblock_task(void *blocking_context);

/* Events API */
void *nanos_get_current_event_counter();
void nanos_increase_current_task_event_counter(void *event_counter, unsigned int increment);
void nanos_decrease_task_event_counter(void *event_counter, unsigned int decrement);

/* Additional API routines */
int nanos_in_serial_context();

/* System info API */
unsigned int nanos_get_num_cpus();
unsigned int nanos_get_current_virtual_cpu();
void *nanos_cpus_begin();
void *nanos_cpus_end();
void *nanos_cpus_advance(void *cpuIterator);
long nanos_cpus_get_numa(void *cpuIterator);

#endif // RUNTIME_API_H

