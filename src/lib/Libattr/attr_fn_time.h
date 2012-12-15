#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute */
#include "list_link.h" /* tlist_head */

int decode_time(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm);

int encode_time(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm);


