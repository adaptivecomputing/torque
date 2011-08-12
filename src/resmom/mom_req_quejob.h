#ifndef _REQ_QUEJOB_H
#define _REQ_QUEJOB_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */

void req_quejob(struct batch_request *preq);

void req_jobcredential(struct batch_request *preq);

void req_jobscript(struct batch_request *preq);

void req_mvjobfile(struct batch_request *preq);

void req_rdytocommit(struct batch_request *preq);

void reply_sid(struct batch_request *preq, long sid, int which);

void req_commit(struct batch_request *preq);

/* static job *locate_new_job(int sock, char *jobid); */

#endif /* _REQ_QUEJOB_H */
