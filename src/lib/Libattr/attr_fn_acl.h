#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute */
#include "pbs_ifl.h" /* batch_op */

int set_uacl(struct pbs_attribute *attr, struct pbs_attribute *newAttr, enum batch_op op);

int set_hostacl(struct pbs_attribute *attr, struct pbs_attribute *newAttr, enum batch_op op);

int acl_check(pbs_attribute *pattr, char *name, int type);

int acl_check_range(const char **pm_ptr, const char **pc_ptr);
