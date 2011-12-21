#include "license_pbs.h" /* See here for the software license */
#ifndef _QUEUE_RECYCLER_H
#define _QUEUE_RECYCLER_H

#include "queue.h" /* queue */

#define MAX_RECYCLE_QUEUES          2

void initialize_queue_recycler();

pbs_queue *next_queue_from_recycler(struct all_queues *aj, int *iter);

void *remove_some_recycle_queues(void *vp);

int insert_into_queue_recycler(pbs_queue *pq);

void update_queue_recycler_next_id();

#endif /* _QUEUE_RECYCLER_H */
