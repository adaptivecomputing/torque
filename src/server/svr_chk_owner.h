#ifndef _SVR_CHK_OWNER_H
#define _SVR_CHK_OWNER_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */
#include "credential.h" /* credential */

int svr_chk_owner(struct batch_request *preq, job *pjob);

int svr_chk_owner_generic(struct batch_request *preq, char *owner, char *submit_host);

int svr_authorize_jobreq(struct batch_request *preq, job *pjob);

int svr_authorize_req(struct batch_request *preq, char *owner, char *submit_host);

int svr_get_privilege(char *user, char *host);

int authenticate_user(struct batch_request *preq, struct credential *pcred, char **autherr);

void chk_job_req_permissions(job **pjob_ptr, struct batch_request *preq);

job *chk_job_request(char *jobid, struct batch_request *preq);

#endif /* _SVR_CHK_OWNER_H */
