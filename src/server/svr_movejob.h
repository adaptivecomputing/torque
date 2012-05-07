#ifndef _SVR_MOVEJOB_H
#define _SVR_MOVEJOB_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */

int svr_movejob(job *jobp, char *destination, int *, struct batch_request *req, int);

/* static int local_move(job *jobp, struct batch_request *req, int); */

void finish_routing_processing(job *pjob, int status);

void finish_moving_processing(job *pjob, struct batch_request *req, int status);

void finish_move_process(char *jobid, struct batch_request *preq, long time, char *node_name, int status, int type, int mom_err);

int send_job_work(char *job_id, char *node_name, int type, int *my_err, struct batch_request *preq);

void *send_job(void *vp);

int net_move(job *jobp, struct batch_request *req);

/* static int should_retry_route(int err); */

/* static int move_job_file(int conn, job *pjob, enum job_file which); */

#endif /* _SVR_MOVEJOB_H */
