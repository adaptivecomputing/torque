#include "license_pbs.h" /* See here for the software license */

#include <time.h> /* timeval */
#include "attribute.h" /* attribute */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y);

int decode_tv(attribute *patr, char *name, char *rescn, char *val, int perm); 

int encode_tv(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm); 

int set_tv(struct attribute *attr, struct attribute *new, enum batch_op op);

int comp_tv(struct attribute *attr, struct attribute *with);

int job_radix_action (attribute *new, void *pobj, int actmode);

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y);

