#ifndef _QUEUE_FUNC_H
#define _QUEUE_FUNC_H
#include "license_pbs.h" /* See here for the software license */

#include "queue.h" /* pbs_queue, all_queues */

pbs_queue *que_alloc(char *name);

void que_free(pbs_queue *pq);

int que_purge(pbs_queue *pque);

pbs_queue *find_queuebyname(char *quename);

pbs_queue *get_dfltque(void);

void initialize_allques_array(all_queues *aq);

int insert_queue(all_queues *aq, pbs_queue *pque);

int remove_queue(all_queues *aq, pbs_queue *pque);

pbs_queue *next_queue(all_queues *aq, int *iter);

#endif /* _QUEUE_FUNC_H */
