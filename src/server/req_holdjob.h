#ifndef _REQ_HOLDJOB_H
#define _REQ_HOLDJOB_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "array.h" /* job_array */
#include "list_link.h" /* tlist_head */
#include "attribute.h" /* pbs_attribute */

int chk_hold_priv(long val, int perm);

int req_holdjob(struct batch_request *preq);

int req_checkpointjob(struct batch_request *preq);

int release_job(struct batch_request *preq, void *j);

int req_releasejob(struct batch_request *preq);

int release_whole_array(job_array *pa, struct batch_request *preq);

int req_releasearray(struct batch_request *preq);

int get_hold(tlist_head *phead, char **pset, pbs_attribute *temphold);

/* static void process_hold_reply(struct work_task *pwt); */

/* static void process_checkpoint_reply(struct work_task *pwt); */

#endif /* _REQ_HOLDJOB_H */
