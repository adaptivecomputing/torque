#ifndef _STAT_JOB_H
#define _STAT_JOB_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */
#include "attribute.h" /* pbs_attribute, svrattrl, attribute_def */
#include "list_link.h" /* tlist_head */



int status_job(job *pjob, struct batch_request *preq, svrattrl *pal, tlist_head *pstathd, int *bad);

int status_attrib(svrattrl *pal, attribute_def *padef, pbs_attribute *pattr, int limit, int priv, tlist_head *phead, int *bad, int IsOwner);

#endif /* _STAT_JOB_H */
