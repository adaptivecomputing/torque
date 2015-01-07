#include "license_pbs.h" /* See here for the software license */
#ifndef _REQ_DELETE_H
#define _REQ_DELETE_H

#include "pbs_job.h" /* job */
#include "work_task.h" /* work_task */
#include "batch_request.h" /* batch_request */

void remove_stagein(job **pjob);

void ensure_deleted(struct work_task *ptask);

int execute_job_delete(job *pjob, char *Msg, struct batch_request *preq);

int req_deletejob(struct batch_request *preq);

void change_restart_comment_if_needed(struct job *pjob);

int has_job_delete_nanny(struct job *pjob);

void purge_completed_jobs(struct batch_request *preq);

int is_ms_on_server(const job *pjob);

void setup_apply_job_delete_nanny(job *pjob, time_t  time_now);

#endif /* _REQ_DELETE_H */
