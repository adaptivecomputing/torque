#ifndef _JOB_FUNC_H
#define _JOB_FUNC_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job, job_atr, all_jobs */
#include "array.h" /* job_array */
#include "work_task.h" /* work_task */
#include "batch_request.h" /* batch_request */

void send_qsub_delmsg(job *pjob, char *text);

int remtree(char *dirname);

int job_abt(job **pjobp, char *text);

int conn_qsub(char *hostname, long port, char *EMsg);

job *job_alloc(void);

void job_free(job *pj, int use_recycle);

job *job_clone(job *template_job, job_array *pa, int taskid);

void *job_clone_wt(void *vp);

/* static void job_init_wattr(job *pj); */

struct batch_request *cpy_checkpoint(struct batch_request *preq, job *pjob, enum job_atr ati, int direction);

void remove_checkpoint(job **pjob);

void cleanup_restart_file(job *pjob);

int record_jobinfo(job *pjob);

int svr_job_purge(job *pjob);

char *get_correct_jobname(const char *jobid);

#ifndef PBS_MOM
void initialize_all_jobs_array(struct all_jobs *aj);

int insert_job(struct all_jobs *aj, job *pjob);

int insert_job_after(struct all_jobs *aj, job *already_in, job *pjob);

int insert_job_first(struct all_jobs *aj, job *pjob);

int has_job(struct all_jobs *aj, job *pjob);

int remove_job(struct all_jobs *aj, job *pjob);

job *next_job(struct all_jobs *aj, int *iter);

job *next_job_from_back(struct all_jobs *aj, int *iter);

int swap_jobs(struct all_jobs *aj, job *job1, job *job2);
#endif
job_array *get_jobs_array(job **pjob);

#endif /* _JOB_FUNC_H */
