#include "license_pbs.h" /* See here for the software license */
#ifndef _REQ_DELETE_H
#define _REQ_DELETE_H

#include "pbs_job.h" /* job */
#include "work_task.h" /* work_task */
#include "batch_request.h" /* batch_request */

void remove_stagein(job **pjob);

void ensure_deleted(struct work_task *ptask);

int execute_job_delete(job *pjob, char *Msg, struct batch_request *preq);

struct batch_request *duplicate_request(struct batch_request *preq);

int req_deletejob(struct batch_request *preq);

void change_restart_comment_if_needed(struct job *pjob);

/* static void post_delete_route(struct work_task *pwt); */

/* static void post_delete_mom1(struct work_task *pwt); */

/* static void post_delete_mom2(struct work_task *pwt); */

/* static int forced_jobpurge(job *pjob, struct batch_request *preq); */

int has_job_delete_nanny(struct job *pjob);

void remove_job_delete_nanny(struct job *pjob);

struct work_task *apply_job_delete_nanny(struct job *pjob, int delay);

/* static void job_delete_nanny(struct work_task *pwt); */

/* static void post_job_delete_nanny(struct work_task *pwt); */

void purge_completed_jobs(struct batch_request *preq);

#endif /* _REQ_DELETE_H */
