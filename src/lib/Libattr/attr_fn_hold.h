#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute */
#include "list_link.h" /* tlist_head */

int decode_hold( attribute *patr, char *name, char *rescn, char *val, int perm); 

int encode_hold( attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm); 

int comp_hold(struct attribute *attr, struct attribute *with);

