#ifndef _QUEUE_RECOV_H
#define _QUEUE_RECOV_H
#include "license_pbs.h" /* See here for the software license */

#include "queue.h" /* pbs_queue */

int que_save(pbs_queue *pque);
 
pbs_queue *que_recov_xml(char *filename);

pbs_queue *que_recov(char *filename);

#endif /* _QUEUE_RECOV_H */
