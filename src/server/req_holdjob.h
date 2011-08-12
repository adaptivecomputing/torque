#ifndef _REQ_HOLDJOB_H
#define _REQ_HOLDJOB_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "array.h" /* job_array */
#include "list_link.h" /* tlist_head */
#include "attribute.h" /* attribute */

int chk_hold_priv(long val, int perm);

void *req_holdjob(void *vp);

void req_checkpointjob(struct batch_request *preq);

int release_job(struct batch_request *preq, void *j);

void *req_releasejob(void *vp);

int release_whole_array(job_array *pa, struct batch_request *preq);

void *req_releasearray(void *vp);

int get_hold(tlist_head *phead, char **pset, attribute *temphold);

/* static void process_hold_reply(struct work_task *pwt); */

/* static void process_checkpoint_reply(struct work_task *pwt); */

#endif /* _REQ_HOLDJOB_H */
