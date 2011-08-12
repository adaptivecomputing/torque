#ifndef _RESCCOST_H
#define _RESCCOST_H
#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute */
#include "list_link.h" /* tlist_head, attribute, batch_op */
#include "pbs_job.h" /* job */


/* static struct resource_cost *add_cost_entry(attribute *patr, resource_def *prdef); */

int decode_rcost(attribute *patr, char *name, char *rescn, char *val, int perm);
 
int encode_rcost(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm);

int set_rcost(struct attribute *old, struct attribute *new, enum batch_op op);

void free_rcost(attribute *pattr);

long calc_job_cost(job *pjob);

#endif /* _RESCCOST_H */
