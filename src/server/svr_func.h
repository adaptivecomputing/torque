#ifndef _SVR_FUNC_H
#define _SVR_FUNC_H
#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute, batch_op */
#include "list_link.h" /* tlist_head */
#include "pbs_job.h" /* job */

int encode_svrstate(pbs_attribute *pattr, tlist_head *phead, char *atname, char *rsname, int mode, int perm);

void set_resc_assigned(job *pjob, enum batch_op op);

int ck_checkpoint(pbs_attribute *pattr, void *pobject, int mode);

int decode_null(pbs_attribute *patr, char *name, char *rn, char *val, int perm);

int set_null(pbs_attribute *pattr, pbs_attribute *new, enum batch_op op);

int poke_scheduler(pbs_attribute *pattr, void *pobj, int actmode);

int get_svr_attr_l(int attr_index, long *i);

int get_svr_attr_c(int attr_index, char *c);

int get_svr_attr_str(int attr_index, char **str);

int get_svr_attr_arst(int attr_index, struct array_strings **arst);


#endif /* _SVR_FUNC_H */
