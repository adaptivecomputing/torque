#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */

int decode_arst_direct( struct attribute *patr, char *val); 

int decode_arst( struct attribute *patr, char *name, char *rescn, char *val, int perm); 

int decode_arst_merge( struct attribute *patr, char *name, char *rescn, char *val); 

int encode_arst( attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm); 

int set_arst( struct attribute *attr, struct attribute *new, enum batch_op op); 

int comp_arst( struct attribute *attr, struct attribute *with);

void free_arst( struct attribute *attr);

char *arst_string( char *str, attribute *pattr);
