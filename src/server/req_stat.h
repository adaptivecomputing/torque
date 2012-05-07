#ifndef _REQ_STAT_H
#define _REQ_STAT_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "work_task.h" /* work_task */
#include "batch_request.h" /* batch_request */
#include "pbs_nodes.h" /* pbsnode */
#include "list_link.h" /* tlist_head */
#include "svrfunc.h" /* stat_cntl */

int req_stat_job(struct batch_request *preq);

int stat_to_mom(char *job_id, struct stat_cntl *cntl);

void stat_mom_job(char *jobid);

void stat_update(struct batch_request *preq, struct stat_cntl *cntl);

void poll_job_task(struct work_task *ptask);

int req_stat_que(struct batch_request *preq);

/* static int status_que(pbs_queue *pque, struct batch_request *preq, tlist_head *pstathd); */

int get_numa_statuses(struct pbsnode *pnode, struct batch_request *preq, int *bad, tlist_head *pstathd);

int req_stat_node(struct batch_request *preq);

int req_stat_svr(struct batch_request *preq);

/* static void update_state_ct(pbs_attribute *pattr, int *ct_array, char *buf); */

#endif /* _REQ_STAT_H */
