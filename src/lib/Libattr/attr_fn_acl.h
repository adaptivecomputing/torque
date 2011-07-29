#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute */
#include "pbs_ifl.h" /* batch_op */

int set_uacl(struct attribute *attr, struct attribute *new, enum batch_op op);

int set_hostacl(struct attribute *attr, struct attribute *new, enum batch_op op);

int acl_check(attribute *pattr, char *name, int type);

int acl_check_range(const char **pm_ptr, const char **pc_ptr);
