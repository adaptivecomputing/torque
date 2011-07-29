#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */

int decode_l( attribute *patr, char *name, char *rescn, char *val, int perm);

int encode_l( attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm);

int set_l(struct attribute *attr, struct attribute *new, enum batch_op op);

int comp_l(struct attribute *attr, struct attribute *with);

