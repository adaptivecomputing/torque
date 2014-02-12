#ifndef _ISSUE_REQUEST_H
#define _ISSUE_REQUEST_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "work_task.h" /* work_task */
#include "batch_request.h" /* batch_request */

/* static void reissue_to_svr(struct work_task *pwt); */

void reissue_to_svr(struct work_task *);

int issue_to_svr(char *servern, struct batch_request *preq, void (*replyfunc)(struct work_task *));

int send_request_to_remote_server(int conn, batch_request *request);

int handle_local_request(int conn, batch_request *request);

int issue_Drequest(int  conn, struct batch_request *request);

void release_req(struct work_task *pwt);

#endif /* _ISSUE_REQUEST_H */
