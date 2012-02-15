#ifndef _REQ_SIGNAL_H
#define _REQ_SIGNAL_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */
#include "work_task.h" /* work_task */

void *req_signaljob(void *preq);

int issue_signal(job **pjob, char *signame, void (*func)(struct work_task *), void *extra);

/* static void post_signal_req(struct work_task *pwt); */

#endif /* _REQ_SIGNAL_H */
