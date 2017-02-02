#ifndef _JOB_ROUTE_H
#define _JOB_ROUTE_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "queue.h" /* pbs_queue */

void add_dest(svr_job *jobp);

int default_router(svr_job *jobp, struct pbs_queue *qp, long retry_time);

int job_route(svr_job *jobp);

int remove_procct(svr_job *pjob);

int initialize_procct(svr_job *pjob);

void *queue_route(void *pque);

#endif /* _JOB_ROUTE_H */
