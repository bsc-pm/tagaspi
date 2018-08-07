/* 2015 Daniel Bittman <danielbittman1@gmail.com>: http://dbittman.github.io/ */

#ifndef MPSC_QUEUE_H
#define MPSC_QUEUE_H

#include "utils.h"

#include <assert.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

typedef struct {
	_Atomic size_t count;
	_Atomic size_t head;
	size_t tail;
	size_t max;
	void * _Atomic *buffer;
} mpsc_queue_t;


/* create a new mpsc_queue_t. If n == NULL, it will allocate
 * a new one and return it. If n != NULL, it will
 * initialize the structure that was passed in. 
 * capacity must be greater than 1, and it is recommended
 * to be much, much larger than that. */
static inline void mpsc_queue_initialize(mpsc_queue_t *q, size_t capacity)
{
	assert(q != NULL);
	assert(capacity > 0);
	
	q->count = ATOMIC_VAR_INIT(0);
	q->head = ATOMIC_VAR_INIT(0);
	q->tail = 0;
	q->buffer = calloc(capacity, sizeof(void *));
	q->max = capacity;
	atomic_thread_fence(memory_order_seq_cst);
}

/* destroy a mpsc_queue_t. Frees the internal buffer, and
 * frees q if it was created by passing NULL to mpsc_queue_create */
static inline void mpsc_queue_finalize(mpsc_queue_t *q)
{
	assert(q != NULL);
	free(q->buffer);
}

/* enqueue an item into the queue. Returns 0 on success
 * and 1 on failure (queue full). This is safe to call
 * from multiple threads */
static inline int mpsc_queue_enqueue(mpsc_queue_t *q, void *obj)
{
	size_t count = atomic_fetch_add_explicit(&q->count, 1, memory_order_seq_cst);
	if(count >= q->max) {
		/* back off, queue is full */
		atomic_fetch_sub_explicit(&q->count, 1, memory_order_seq_cst);
		return 1;
	}
	
	/* increment the head, which gives us 'exclusive' access to that element */
	size_t head = atomic_fetch_add_explicit(&q->head, 1, memory_order_seq_cst);
	assert(q->buffer[head % q->max] == 0);
	
	void *rv = atomic_exchange_explicit(&q->buffer[head % q->max], obj, memory_order_seq_cst);
	assert(rv == NULL);
	UNUSED(rv);
	return 0;
}

/* dequeue an item from the queue and return it.
 * THIS IS NOT SAFE TO CALL FROM MULTIPLE THREADS.
 * Returns NULL on failure, and the item it dequeued
 * on success */
static inline void *mpsc_queue_dequeue(mpsc_queue_t *q)
{
	void *ret = atomic_exchange_explicit(&q->buffer[q->tail], NULL, memory_order_seq_cst);
	if (!ret) {
		/* a thread is adding to the queue, but hasn't done the atomic_exchange yet
		 * to actually put the item in. Act as if nothing is in the queue.
		 * Worst case, other producers write content to tail + 1..n and finish, but
		 * the producer that writes to tail doesn't do it in time, and we get here.
		 * But that's okay, because once it DOES finish, we can get at all the data
		 * that has been filled in. */
		return NULL;
	}
	if(++q->tail >= q->max) {
		q->tail = 0;
	}
	
	size_t r = atomic_fetch_sub_explicit(&q->count, 1, memory_order_seq_cst);
	assert(r > 0);
	UNUSED(r);
	return ret;
}

/* get the number of items in the queue currently */
static inline size_t mpsc_queue_size(mpsc_queue_t *q)
{
	return atomic_load_explicit(&q->count, memory_order_seq_cst);
}

static inline int mpsc_queue_empty(mpsc_queue_t *q)
{
	return atomic_load_explicit(&q->count, memory_order_seq_cst) == 0;
}

/* get the capacity of the queue */
static inline size_t mpsc_queue_capacity(mpsc_queue_t *q)
{
	return q->max;
}

#endif /* MPSC_QUEUE_H */
