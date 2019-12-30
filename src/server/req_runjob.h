#ifndef _REQ_RUNJOB_H
#define _REQ_RUNJOB_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */

int req_runjob(struct batch_request *preq);

int req_stagein(struct batch_request *preq);

int svr_startjob(svr_job *pjob, struct batch_request *preq, char *FailHost, std::string& EMsg);

void finish_sendmom(const char *job_id, struct batch_request *preq, long start_time, const char *node_name, int status, int);

#endif /* _REQ_RUNJOB_H */
