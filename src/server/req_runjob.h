#ifndef _REQ_RUNJOB_H
#define _REQ_RUNJOB_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */

void *req_runjob(void *vp);

/* static int is_checkpoint_restart(job *pjob); */

/* static void post_checkpointsend(struct work_task *pwt); */

/* static int svr_send_checkpoint(job *pjob, struct batch_request *preq, int state, int substate); */

void *req_stagein(void *vp);

/* static void post_stagein(struct work_task *pwt); */

/* static int svr_stagein(job *pjob, struct batch_request *preq, int state, int substate); */

int svr_startjob(job *pjob, struct batch_request *preq, char *FailHost, char *EMsg);

/* static int svr_strtjob2(job *pjob, struct batch_request *preq); */

void finish_sendmom(job *pjob, struct batch_request *preq, long start_time, char *node_name, int status, int);

/* static job *chk_job_torun(struct batch_request *preq, int setnn); */

int set_mother_superior_ports(job *pjob, char *list);

/* static int assign_hosts(job *pjob, char *given, int set_exec_host, char *FailHost, char *EMsg); */

#endif /* _REQ_RUNJOB_H */
