#ifndef _REQ_MODIFY_H
#define _REQ_MODIFY_H
#include "license_pbs.h" /* See here for the software license */

#include "work_task.h" /* work_task */
#include "attribute.h" /* svrattrl */
#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */
#include "array.h" /* job_array */

/* static void post_modify_req(struct work_task *pwt); */

void mom_cleanup_checkpoint_hold(struct work_task *ptask);

void chkpt_xfr_done(struct work_task *ptask);

int modify_job(void **j, svrattrl *plist, struct batch_request *preq, int checkpoint_req, int flag);

int copy_batchrequest(struct batch_request **newreq, struct batch_request *preq, int type, int jobid);

int modify_whole_array(job_array *pa, svrattrl *plist, struct batch_request *preq, int checkpoint_req);

int req_modifyarray(struct batch_request *preq);

int req_modifyjob(struct batch_request *preq);

int modify_job_attr(job *pjob, svrattrl *plist, int perm, int *bad);

void post_modify_arrayreq(struct work_task *pwt);

#endif /* _REQ_MODIFY_H */
