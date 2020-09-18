/*
	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2018-2019 Barcelona Supercomputing Center (BSC)
*/

#ifndef TAGASPI_H
#define TAGASPI_H

#include <GASPI.h>

#pragma GCC visibility push(default)

#ifdef __cplusplus
extern "C" {
#endif

#define GASPI_NOTIFICATION_IGNORE (gaspi_notification_t *)0

typedef unsigned char gaspi_queue_group_id_t;

typedef enum
{
	/* Distribution of the queues using round-robin.
	 * Each call gets a different queue. */
	GASPI_QUEUE_GROUP_POLICY_DEFAULT = 0,
	/* Distribution of the queues across CPUs using
	 * round-robin. Each CPU will always get the same
	 * queue. If the number of CPUs exceeds the number of
	 * queues, the queues are distributed in a round-robin
	 * way. This policy avoids assigning the same queue to
	 * CPUs that are in different NUMA nodes.
	 */
	GASPI_QUEUE_GROUP_POLICY_CPU_RR = 1
} gaspi_queue_group_policy_t;

gaspi_return_t
tagaspi_proc_init(const gaspi_timeout_t timeout_ms);

gaspi_return_t
tagaspi_proc_term(const gaspi_timeout_t timeout_ms);

gaspi_return_t
tagaspi_write(const gaspi_segment_id_t segment_id_local,
		const gaspi_offset_t offset_local,
		const gaspi_rank_t rank,
		const gaspi_segment_id_t segment_id_remote,
		const gaspi_offset_t offset_remote,
		const gaspi_size_t size,
		const gaspi_queue_id_t queue,
		const gaspi_timeout_t timeout_ms);

gaspi_return_t
tagaspi_read(const gaspi_segment_id_t segment_id_local,
		const gaspi_offset_t offset_local,
		const gaspi_rank_t rank,
		const gaspi_segment_id_t segment_id_remote,
		const gaspi_offset_t offset_remote,
		const gaspi_size_t size,
		const gaspi_queue_id_t queue,
		const gaspi_timeout_t timeout_ms);

gaspi_return_t
tagaspi_write_list(const gaspi_number_t num,
		gaspi_segment_id_t * const segment_id_local,
		gaspi_offset_t * const offset_local,
		const gaspi_rank_t rank,
		gaspi_segment_id_t * const segment_id_remote,
		gaspi_offset_t * const offset_remote,
		gaspi_size_t * const size,
		const gaspi_queue_id_t queue,
		const gaspi_timeout_t timeout_ms);

gaspi_return_t
tagaspi_read_list(const gaspi_number_t num,
		gaspi_segment_id_t * const segment_id_local,
		gaspi_offset_t * const offset_local,
		const gaspi_rank_t rank,
		gaspi_segment_id_t * const segment_id_remote,
		gaspi_offset_t * const offset_remote,
		gaspi_size_t * const size,
		const gaspi_queue_id_t queue,
		const gaspi_timeout_t timeout_ms);

gaspi_return_t
tagaspi_notify(const gaspi_segment_id_t segment_id_remote,
		const gaspi_rank_t rank,
		const gaspi_notification_id_t notification_id,
		const gaspi_notification_t notification_value,
		const gaspi_queue_id_t queue,
		const gaspi_timeout_t timeout_ms);

gaspi_return_t
tagaspi_write_notify(const gaspi_segment_id_t segment_id_local,
		const gaspi_offset_t offset_local,
		const gaspi_rank_t rank,
		const gaspi_segment_id_t segment_id_remote,
		const gaspi_offset_t offset_remote,
		const gaspi_size_t size,
		const gaspi_notification_id_t notification_id,
		const gaspi_notification_t notification_value,
		const gaspi_queue_id_t queue,
		const gaspi_timeout_t timeout_ms);

gaspi_return_t
tagaspi_write_list_notify(const gaspi_number_t num,
		gaspi_segment_id_t * const segment_id_local,
		gaspi_offset_t * const offset_local,
		const gaspi_rank_t rank,
		gaspi_segment_id_t * const segment_id_remote,
		gaspi_offset_t * const offset_remote,
		gaspi_size_t * const size,
		const gaspi_segment_id_t segment_id_notification,
		const gaspi_notification_id_t notification_id,
		const gaspi_notification_t notification_value,
		const gaspi_queue_id_t queue,
		const gaspi_timeout_t timeout_ms);

gaspi_return_t
tagaspi_notify_async_waitall(const gaspi_segment_id_t segment_id_local,
		const gaspi_notification_id_t notification_begin,
		const gaspi_number_t num,
		gaspi_notification_t old_notification_values[]);

gaspi_return_t
tagaspi_ack_write(const gaspi_segment_id_t ack_segment_id,
		const gaspi_notification_id_t ack_notification_begin,
		const gaspi_number_t ack_notification_num,
		gaspi_notification_t ack_notification_values[],
		const gaspi_segment_id_t segment_id_local,
		const gaspi_offset_t offset_local,
		const gaspi_rank_t rank,
		const gaspi_segment_id_t segment_id_remote,
		const gaspi_offset_t offset_remote,
		const gaspi_size_t size,
		const gaspi_queue_id_t queue);

gaspi_return_t
tagaspi_ack_read(const gaspi_segment_id_t ack_segment_id,
		const gaspi_notification_id_t ack_notification_begin,
		const gaspi_number_t ack_notification_num,
		gaspi_notification_t ack_notification_values[],
		const gaspi_segment_id_t segment_id_local,
		const gaspi_offset_t offset_local,
		const gaspi_rank_t rank,
		const gaspi_segment_id_t segment_id_remote,
		const gaspi_offset_t offset_remote,
		const gaspi_size_t size,
		const gaspi_queue_id_t queue);

gaspi_return_t
tagaspi_ack_notify(const gaspi_segment_id_t ack_segment_id,
		const gaspi_notification_id_t ack_notification_begin,
		const gaspi_number_t ack_notification_num,
		gaspi_notification_t ack_notification_values[],
		const gaspi_segment_id_t segment_id_remote,
		const gaspi_rank_t rank,
		const gaspi_notification_id_t notification_id,
		const gaspi_notification_t notification_value,
		const gaspi_queue_id_t queue);

gaspi_return_t
tagaspi_ack_write_notify(const gaspi_segment_id_t ack_segment_id,
		const gaspi_notification_id_t ack_notification_begin,
		const gaspi_number_t ack_notification_num,
		gaspi_notification_t ack_notification_values[],
		const gaspi_segment_id_t segment_id_local,
		const gaspi_offset_t offset_local,
		const gaspi_rank_t rank,
		const gaspi_segment_id_t segment_id_remote,
		const gaspi_offset_t offset_remote,
		const gaspi_size_t size,
		const gaspi_notification_id_t notification_id,
		const gaspi_notification_t notification_value,
		const gaspi_queue_id_t queue);

gaspi_return_t
tagaspi_queue_group_create(const gaspi_queue_group_id_t queue_group,
		const gaspi_queue_id_t queue_begin,
		const gaspi_number_t queue_num,
		const gaspi_queue_group_policy_t policy);

gaspi_return_t
tagaspi_queue_group_delete(const gaspi_queue_group_id_t queue_group);

gaspi_return_t
tagaspi_queue_group_get_queue(const gaspi_queue_group_id_t queue_group,
		gaspi_queue_id_t * const queue);

gaspi_return_t
tagaspi_queue_group_max(gaspi_number_t * const queue_group_max);

#ifdef __cplusplus
}
#endif

#pragma GCC visibility pop

#endif /* TAGASPI_H */
