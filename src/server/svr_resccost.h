#ifndef _RESCCOST_H
#define _RESCCOST_H
#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute */
#include "list_link.h" /* tlist_head, batch_op */
#include "pbs_job.h" /* job */


/* static struct resource_cost *add_cost_entry(pbs_attribute *patr, resource_def *prdef); */

int decode_rcost(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm);
 
int encode_rcost(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm);

int set_rcost(pbs_attribute *attr_old, pbs_attribute *attr_new, enum batch_op op);

void free_rcost(pbs_attribute *pattr);

long calc_job_cost(job *pjob);

#endif /* _RESCCOST_H */
