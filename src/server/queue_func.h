#ifndef _QUEUE_FUNC_H
#define _QUEUE_FUNC_H
#include "license_pbs.h" /* See here for the software license */

#include "queue.h" /* pbs_queue, all_queues */
#include "pbs_job.h" /* all_jobs */

int lock_queue( struct pbs_queue *the_queue, const char *id, const char *msg, int logging);

int unlock_queue( struct pbs_queue *the_queue, const char *id, const char *msg, int logging);

void que_free(pbs_queue *pq, int sv_qs_mutex_held);

int que_purge(pbs_queue *pque);

pbs_queue *get_dfltque(void);

int insert_queue(all_queues *aq, pbs_queue *pque);

int remove_queue(all_queues *aq, pbs_queue *pque);

pbs_queue *next_queue(all_queues *aq, all_queues_iterator *iter);

#endif /* _QUEUE_FUNC_H */
