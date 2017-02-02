#ifndef _JOB_FUNC_H
#define _JOB_FUNC_H
#include "license_pbs.h" /* See here for the software license */
#include "pbs_job.h"

struct batch_request;

int job_abt(svr_job **pjobp, const char *text, bool depedentjob=0);

svr_job *job_clone(svr_job *template_job, job_array *pa, int taskid);

void *job_clone_wt(void *vp);

struct batch_request *cpy_checkpoint(struct batch_request *preq, svr_job *pjob, enum job_atr ati, int direction);

void remove_checkpoint(svr_job **pjob);

void cleanup_restart_file(svr_job *pjob);

int record_jobinfo(svr_job *pjob);

int svr_job_purge(svr_job *pjob, int keepSpoolFiles=0);

job_array *get_jobs_array(svr_job **pjob);

struct pbs_queue *get_jobs_queue(svr_job **pjob_ptr);

int fix_external_exec_hosts(svr_job *pjob);

int fix_cray_exec_hosts(svr_job *pjob);

int change_external_job_name(svr_job *pjob);

void handle_aborted_job(svr_job **job_ptr, bool  dependentjob, long KeepSeconds, const char *text);

#endif /* _JOB_FUNC_H */
