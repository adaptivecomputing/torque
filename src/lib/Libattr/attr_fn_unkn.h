#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */

int decode_unkn(attribute *patr, char *name, char *rescn, char *value, int perm); 

int encode_unkn(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm); 

int set_unkn(struct attribute *old, struct attribute *new, enum batch_op op);

int comp_unkn(struct attribute *attr, struct attribute *with);

void free_unkn(attribute *pattr);
