#ifndef _REQ_RERUN_H
#define _REQ_RERUN_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */

/* static void post_rerun(struct work_task *pwt); */

int req_rerunjob(struct batch_request *preq);

#endif /* _REQ_RERUN_H */
