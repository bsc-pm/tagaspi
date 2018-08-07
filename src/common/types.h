#ifndef TYPES_H
#define TYPES_H

#include <GASPI.h>
#include <TAGASPI.h>

#include "mpsc_queue.h"
#include "spinlock.h"

#include <stdbool.h>

typedef struct waiting_range_t waiting_range_t;
struct waiting_range_t {
	gaspi_segment_id_t segment_id;
	gaspi_notification_id_t first_id;
	gaspi_number_t num_ids;
	gaspi_notification_t *notified_values;
	gaspi_number_t remaining;
	void *event_counter;
	waiting_range_t *next;
};

typedef mpsc_queue_t waiting_range_queue_t;

typedef waiting_range_t * waiting_range_list_t;

typedef struct {
	gaspi_queue_id_t first_id;
	gaspi_number_t num_ids;
	gaspi_queue_group_policy_t policy;
	void *data;
} queue_group_t;

typedef struct {
	bool enabled;
	gaspi_number_t max_queues;
	gaspi_number_t max_segments;
	gaspi_number_t max_queue_groups;
	gaspi_number_t num_queue_groups;
	waiting_range_queue_t *waiting_range_queues;
	waiting_range_list_t *waiting_range_lists;
	queue_group_t *queue_groups;
	spinlock_t *queue_polling_locks;
	spinlock_t notification_polling_lock;
	spinlock_t queue_groups_lock;
} tagaspi_environment_t;

#endif /* TYPES_H */
