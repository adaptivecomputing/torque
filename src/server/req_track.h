#ifndef _REQ_TRACK_H
#define _REQ_TRACK_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "work_task.h" /* work_task */
#include "pbs_job.h" /* job */

int req_track(struct batch_request *preq);

void track_save(struct work_task *pwt);
    
void issue_track(job *pjob);

#endif /* _REQ_TRACK_H */
