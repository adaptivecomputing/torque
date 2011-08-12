#ifndef _REQ_SIGNAL_H
#define _REQ_SIGNAL_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */

void req_signaljob(struct batch_request *preq);

int issue_signal(job *pjob, char *signame, void (*func)(struct work_task *), void *extra)

/* static void post_signal_req(struct work_task *pwt); */

#endif /* _REQ_SIGNAL_H */
