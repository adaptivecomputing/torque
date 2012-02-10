#ifndef _QUEUE_FUNC_H
#define _QUEUE_FUNC_H
#include "license_pbs.h" /* See here for the software license */

#include "queue.h" /* pbs_queue, all_queues */
#include "pbs_job.h" /* all_jobs */

int lock_queue( struct pbs_queue *the_queue, const char *id, char *msg, int logging);

int unlock_queue( struct pbs_queue *the_queue, const char *id, char *msg, int logging);

pbs_queue *que_alloc(char *name, int sv_qs_mutex_held);

void que_free(pbs_queue *pq, int sv_qs_mutex_held);

int que_purge(pbs_queue *pque);

pbs_queue *find_queuebyname(char *quename);

pbs_queue *get_dfltque(void);

void initialize_allques_array(all_queues *aq);

void free_alljobs_array(struct all_jobs *aj);

int insert_queue(all_queues *aq, pbs_queue *pque);

int remove_queue(all_queues *aq, pbs_queue *pque);

pbs_queue *next_queue(all_queues *aq, int *iter);

int get_parent_dest_queues(char *queue_parent_name, char *queue_dest_name, pbs_queue **parent, pbs_queue **dest, job **pjob_ptr);

#endif /* _QUEUE_FUNC_H */
