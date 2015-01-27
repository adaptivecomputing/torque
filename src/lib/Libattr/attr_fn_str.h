#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */

int decode_str(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm);

int encode_str(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm);

int set_str(struct pbs_attribute *attr, struct pbs_attribute *newAttr, enum batch_op op);
int set_str_csv(struct pbs_attribute *attr, struct pbs_attribute *newAttr, enum batch_op op);

int comp_str(struct pbs_attribute *attr, struct pbs_attribute *with);

int comp_nodestr(struct pbs_attribute *attr, struct pbs_attribute *with);

void free_str(struct pbs_attribute *attr);

void replace_attr_string(struct pbs_attribute *attr, char *newval);

