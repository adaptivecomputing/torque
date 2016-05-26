#ifndef _SVR_FUNC_H
#define _SVR_FUNC_H
#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute, batch_op */
#include "list_link.h" /* tlist_head */
#include "pbs_job.h" /* job */

int encode_svrstate(pbs_attribute *pattr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm);

void set_resc_assigned(job *pjob, enum batch_op op);

int ck_checkpoint(pbs_attribute *pattr, void *pobject, int mode);

int decode_null(pbs_attribute *patr, const char *name, const char *rn, const char *val, int perm);

int set_null(pbs_attribute *pattr, pbs_attribute *new_attr, enum batch_op op);

int poke_scheduler(pbs_attribute *pattr, void *pobj, int actmode);

int keep_completed_val_check(pbs_attribute *pattr, void *pobject, int actmode);
#endif /* _SVR_FUNC_H */
