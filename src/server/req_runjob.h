#ifndef _REQ_RUNJOB_H
#define _REQ_RUNJOB_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */

int req_runjob(struct batch_request *preq);

/* static int is_checkpoint_restart(job *pjob); */

/* static void post_checkpointsend(struct work_task *pwt); */

/* static int svr_send_checkpoint(job *pjob, struct batch_request *preq, int state, int substate); */

int req_stagein(struct batch_request *preq);

/* static void post_stagein(struct work_task *pwt); */

/* static int svr_stagein(job *pjob, struct batch_request *preq, int state, int substate); */

int svr_startjob(job *pjob, struct batch_request *preq, char *FailHost, char *EMsg);

/* static int svr_strtjob2(job *pjob, struct batch_request *preq); */

void finish_sendmom(char *job_id, struct batch_request *preq, long start_time, char *node_name, int status, int);

/* static job *chk_job_torun(struct batch_request *preq, int setnn); */

/* static int assign_hosts(job *pjob, char *given, int set_exec_host, char *FailHost, char *EMsg); */

#endif /* _REQ_RUNJOB_H */
