#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */

int decode_str(attribute *patr, char *name, char *rescn, char *val, int perm); 

int encode_str(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm); 

int set_str(struct attribute *attr, struct attribute *new, enum batch_op op);

int comp_str(struct attribute *attr, struct attribute *with);

void free_str(struct attribute *attr);

void replace_attr_string(struct attribute *attr, char *newval);

