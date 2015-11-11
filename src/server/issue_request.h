#ifndef _ISSUE_REQUEST_H
#define _ISSUE_REQUEST_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "work_task.h" /* work_task */
#include "batch_request.h" /* batch_request */

/* static void reissue_to_svr(struct work_task *pwt); */

void reissue_to_svr(struct work_task *);

int handle_local_request(int conn, batch_request *request);

void release_req(struct work_task *pwt);

#endif /* _ISSUE_REQUEST_H */
