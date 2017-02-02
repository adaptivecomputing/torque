#include "license_pbs.h" /* See here for the software license */
#ifndef _REQ_JOBOBIT_H
#define _REQ_JOBOBIT_H

#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job, job_atr */
#include "work_task.h" /* work_task */
#include "attribute.h" /* svrattrl */
#include "list_link.h" /* tlist_head */



struct batch_request *setup_cpyfiles(struct batch_request *preq, svr_job *pjob, char *from, char *to, int direction, int tflag);

struct batch_request *cpy_stage(struct batch_request *preq, svr_job *pjob, enum job_atr ati, int direction);

void rel_resc(svr_job *pjob);

int check_if_checkpoint_restart_failed(svr_job *pjob);

int handle_exiting_or_abort_substate(svr_job *pjob);

int handle_returnstd(svr_job *pjob, struct batch_request *preq, int type);

int handle_stageout(svr_job *pjob, int type, struct batch_request *preq);

int handle_stagedel(svr_job *pjob, int type, struct batch_request *preq);

int handle_exited(svr_job *pjob);

int handle_complete_first_time(svr_job *pjob);

void on_job_rerun(batch_request *preq, char *jobid);

#ifdef USESAVEDRESOURCES
void encode_job_used(svr_job *pjob, tlist_head *phead);
#endif /* USESAVEDRESOURCES */

int req_jobobit(struct batch_request *preq);

#endif /* _REQ_JOBOBIT_H */
