#ifndef _REQ_STAT_H
#define _REQ_STAT_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "work_task.h" /* work_task */
#include "batch_request.h" /* batch_request */
#include "pbs_nodes.h" /* pbsnode */
#include "list_link.h" /* tlist_head */
#define STAT_CNTL
#include "svrfunc.h" /* stat_cntl */

void *req_stat_job(void *vp);

/* static void req_stat_job_step2(struct stat_cntl *cntl); */

int stat_to_mom(job *pjob, struct stat_cntl *cntl);

/* static void stat_update(struct work_task *pwt); */

void stat_mom_job(job *pjob);

void poll_job_task(struct work_task *ptask);

void *req_stat_que(void *vp);

/* static int status_que(pbs_queue *pque, struct batch_request *preq, tlist_head *pstathd); */

int get_numa_statuses(struct pbsnode *pnode, struct batch_request *preq, tlist_head *pstathd);

void *req_stat_node(void *vp);

/* static int status_node(struct pbsnode *pnode, struct batch_request *preq, tlist_head *pstathd); */

void req_stat_svr(struct batch_request *preq);

/* static void update_state_ct(attribute *pattr, int *ct_array, char *buf); */

#endif /* _REQ_STAT_H */
