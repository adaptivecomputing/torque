#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute, size_value */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */

int decode_frequency(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm);

int encode_frequency(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm);

int set_frequency(struct pbs_attribute *attr, struct pbs_attribute *newAttr, enum batch_op op);

int comp_frequency(struct pbs_attribute *attr, struct pbs_attribute *with);

int to_frequency(char *val, struct cpu_frequency_value *psize);

void from_frequency(struct cpu_frequency_value *psize, char *cvnbuf);

