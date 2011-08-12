#ifndef _REQ_SELECT_H
#define _REQ_SELECT_H
#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute */
#include "batch_request.h" /* batch_request */

/* static int order_checkpoint(attribute *attr); */

int comp_checkpoint(attribute *attr, attribute *with);

/* static int comp_state(attribute *state, attribute *selstate); */

void req_selectjobs(struct batch_request *preq);

/* static void sel_step2(struct stat_cntl *cntl); */

/* static void sel_step3(struct stat_cntl *cntl); */

/* static int select_job(job *pjob, struct select_list *psel); */

/* static int sel_attr(attribute *jobat, struct select_list *pselst); */

/* static void free_sellist(struct select_list *pslist); */

/* static int build_selentry(svrattrl *plist, attribute_def *pdef, int perm, struct select_list **rtnentry); */

/* static int build_selist(svrattrl *plist, int perm, struct select_list **pselist, pbs_queue **pque, int *bad); */

#endif /* _REQ_SELECT_H */
