#ifndef _REQ_MANAGER_H
#define _REQ_MANAGER_H
#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute, attribute_def, svrattrl, batch_op */
#include "batch_request.h" /* batch_request */
#include "work_task.h" /* work_task */

/* static char *check_que_attr(pbs_queue *pque); */

int check_que_enable(pbs_attribute *pattr, void *pque, int mode);

int set_queue_type(pbs_attribute *pattr, void *pque, int mode);

/* static void mgr_log_attr(char *msg, struct svrattrl *plist, int logclass, char *objname); */

/* static int mgr_set_attr(pbs_attribute *pattr, attribute_def *pdef, int limit, svrattrl *plist, int privil, int *bad, void *parent, int mode); */

int mgr_unset_attr(pbs_attribute *pattr, attribute_def *pdef, int limit, svrattrl *plist, int privil, int *bad);

/* static void update_subnode(struct pbsnode *pnode); */

int mgr_set_node_attr(struct pbsnode *pnode, attribute_def *pdef, int limit, svrattrl *plist, int privil, int *bad, void *parent, int mode);

void mgr_queue_create(struct batch_request *preq);

void mgr_queue_delete(struct batch_request *preq);

int hostname_check(char *hostname);

void mgr_server_set(struct batch_request *preq);

void mgr_server_unset(struct batch_request *preq);

void mgr_queue_set(struct batch_request *preq);

void mgr_queue_unset(struct batch_request *preq);

void mgr_node_set(struct batch_request *preq);

/* static void mgr_node_delete(struct batch_request *preq); */

void mgr_node_create(struct batch_request *preq);

int req_manager(struct batch_request *preq);

int manager_oper_chk(pbs_attribute *pattr, void *pobject, int actmode);

int servername_chk(pbs_attribute *pattr, void *pobject, int actmode);

void on_extra_resc(struct work_task *ptask);

int extra_resc_chk(pbs_attribute *pattr, void *pobject, int actmode);

void free_extraresc(struct pbs_attribute *attr);

int disallowed_types_chk(pbs_attribute *pattr, void *pobject, int actmode);

/* static int mgr_long_action_helper(pbs_attribute *pattr, int actmode, long minimum, long maximum); */

int schiter_chk(pbs_attribute *pattr, void *pobject, int actmode);

int nextjobnum_chk(pbs_attribute *pattr, void *pobject, int actmode);

int set_nextjobnum(struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op);

#endif /* _REQ_MANAGER_H */
