#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute */
#include "list_link.h" /* tlist_head */

int decode_time(attribute *patr, char *name, char *rescn, char *val, int perm); 

int encode_time(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm); 


