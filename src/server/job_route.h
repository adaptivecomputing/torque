#ifndef _JOB_ROUTE_H
#define _JOB_ROUTE_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "queue.h" /* pbs_queue */

void add_dest(job *jobp);

int default_router(job *jobp, struct pbs_queue *qp, long retry_time);

int job_route(job *jobp);

int remove_procct(job *pjob);

int initialize_procct(job *pjob);

void *queue_route(void *pque);

#endif /* _JOB_ROUTE_H */
