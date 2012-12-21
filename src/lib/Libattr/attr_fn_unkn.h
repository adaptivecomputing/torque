#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */

int encode_unkn(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm);

int set_unkn(struct pbs_attribute *old, struct pbs_attribute *newAttr, enum batch_op op);

int comp_unkn(struct pbs_attribute *attr, struct pbs_attribute *with);

void free_unkn(pbs_attribute *pattr);
