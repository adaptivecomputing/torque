#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */

int decode_arst_direct( struct pbs_attribute *patr, char *val); 

int decode_arst( struct pbs_attribute *patr, char *name, char *rescn, char *val, int perm); 

int decode_arst_merge( struct pbs_attribute *patr, char *name, char *rescn, char *val); 

int encode_arst( pbs_attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm); 

int set_arst( struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op); 

int comp_arst( struct pbs_attribute *attr, struct pbs_attribute *with);

void free_arst( struct pbs_attribute *attr);

char *arst_string( char *str, pbs_attribute *pattr);
