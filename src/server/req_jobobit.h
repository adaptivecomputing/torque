#include "license_pbs.h" /* See here for the software license */
#ifndef _REQ_JOBOBIT_H
#define _REQ_JOBOBIT_H

#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job, job_atr */
#include "work_task.h" /* work_task */
#include "attribute.h" /* svrattrl */
#include "list_link.h" /* tlist_head */
#include "mutex_mgr.hpp"



/* static char *setup_from(job *pjob, char *suffix); */

struct batch_request *setup_cpyfiles(struct batch_request *preq, job *pjob, char *from, char *to, int direction, int tflag, boost::shared_ptr<mutex_mgr>& job_mutex);

/* static int is_joined(job *pjob, enum job_atr ati); */

/* static struct batch_request *return_stdfile(struct batch_request *preq, job *pjob, enum job_atr ati); */

/* static struct batch_request *cpy_stdfile(struct batch_request *preq, job *pjob, enum job_atr ati); */

struct batch_request *cpy_stage(struct batch_request *preq, job *pjob, enum job_atr ati, int direction, boost::shared_ptr<mutex_mgr>& job_mutex);

void rel_resc(job *pjob,  boost::shared_ptr<mutex_mgr>& job_mutex);

int check_if_checkpoint_restart_failed(job *pjob, boost::shared_ptr<mutex_mgr>& job_mutex);

int handle_exiting_or_abort_substate(job *pjob, boost::shared_ptr<mutex_mgr>& job_mutex);

int handle_returnstd(job *pjob, struct batch_request *preq, int type, boost::shared_ptr<mutex_mgr>& job_mutex);

int handle_stageout(job *pjob, int type, struct batch_request *preq);

int handle_stagedel(job *pjob, int type, struct batch_request *preq, boost::shared_ptr<mutex_mgr>& job_mutex);

int handle_exited(job *pjob);

int handle_complete_first_time(job *pjob, boost::shared_ptr<mutex_mgr>& job_mutex);

void on_job_rerun(batch_request *preq, char *jobid);

#ifdef USESAVEDRESOURCES
void encode_job_used(job *pjob, tlist_head *phead);
#endif /* USESAVEDRESOURCES */

int req_jobobit(struct batch_request *preq);

#endif /* _REQ_JOBOBIT_H */
