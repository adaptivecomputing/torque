#ifndef _JOB_FUNC_H
#define _JOB_FUNC_H
#include "license_pbs.h" /* See here for the software license */
#include "pbs_job.h"

struct job_array;
struct batch_request;

int job_abt(struct job **pjobp, const char *text, bool depedentjob=0);

int conn_qsub(char *hostname, long port, char *EMsg);

struct job *copy_job(struct job *parent);

struct job *job_clone(struct job *template_job, struct job_array *pa, int taskid);

void *job_clone_wt(void *vp);

struct batch_request *cpy_checkpoint(struct batch_request *preq, struct job *pjob, enum job_atr ati, int direction);

void remove_checkpoint(struct job **pjob);

void cleanup_restart_file(struct job *pjob);

int record_jobinfo(struct job *pjob);

int svr_job_purge(struct job *pjob, int keepSpoolFiles=0);

struct job_array *get_jobs_array(struct job **pjob);

struct pbs_queue *get_jobs_queue(struct job **pjob_ptr);

int hostname_in_externals(char *hostname, char *externals);

int fix_external_exec_hosts(struct job *pjob);

int fix_cray_exec_hosts(struct job *pjob);

int change_external_job_name(struct job *pjob);

void handle_aborted_job(job **job_ptr, bool  dependentjob, long KeepSeconds, const char *text);

#endif /* _JOB_FUNC_H */
