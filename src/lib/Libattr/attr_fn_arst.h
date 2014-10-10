#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */

int decode_arst_direct( struct pbs_attribute *patr, const char *val);

int decode_arst( struct pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm);
int decode_acl_arst( struct pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm);

int encode_arst( pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm);

int set_arst( struct pbs_attribute *attr, struct pbs_attribute *newAttr, enum batch_op op); 
int set_acl_arst( struct pbs_attribute *attr, struct pbs_attribute *newAttr, enum batch_op op);

int comp_arst( struct pbs_attribute *attr, struct pbs_attribute *with);

void free_arst( struct pbs_attribute *attr);

char *arst_string( const char *str, pbs_attribute *pattr);
