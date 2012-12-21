#include "license_pbs.h" /* See here for the software license */

#include <time.h> /* timeval */
#include "attribute.h" /* pbs_attribute */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y);

int decode_tv(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm);

int encode_tv(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm);

int set_tv(struct pbs_attribute *attr, struct pbs_attribute *newAttr, enum batch_op op);

int comp_tv(struct pbs_attribute *attr, struct pbs_attribute *with);

int job_radix_action (pbs_attribute *newAttr, void *pobj, int actmode);

