#ifndef _SVR_MOVEJOB_H
#define _SVR_MOVEJOB_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */

int svr_movejob(svr_job *jobp, char *destination, int *, struct batch_request *req);

void finish_routing_processing(svr_job *pjob, int status);

void finish_moving_processing(svr_job *pjob, struct batch_request *req, int status);

void finish_move_process(char *jobid, struct batch_request *preq, long time, char *node_name, int status, int type, int mom_err);

void *send_job(void *vp);

int net_move(svr_job *jobp, struct batch_request *req);

#endif /* _SVR_MOVEJOB_H */
