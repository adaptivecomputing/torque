#ifndef _ISSUE_REQUEST_H
#define _ISSUE_REQUEST_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */
#include "work_task.h" /* work_task */

/* static void reissue_to_svr(struct work_task *pwt); */

int issue_to_svr(char *servern, struct batch_request *preq, void (*replyfunc)(struct work_task *));

void release_req(struct work_task *pwt);

#endif /* _ISSUE_REQUEST_H */
