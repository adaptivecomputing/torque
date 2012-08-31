#ifndef _REQ_SIGNAL_H
#define _REQ_SIGNAL_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */
#include "work_task.h" /* work_task */

int req_signaljob(struct batch_request *preq);

/* static void post_signal_req(struct work_task *pwt); */

#endif /* _REQ_SIGNAL_H */
