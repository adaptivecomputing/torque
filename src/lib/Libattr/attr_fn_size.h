#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute, size_value */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */

int decode_size(attribute *patr, char *name, char *rescn, char *val, int perm); 

int encode_size(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm); 

int set_size(struct attribute *attr, struct attribute *new, enum batch_op op);

int comp_size(struct attribute *attr, struct attribute *with);

int normalize_size(struct size_value *a, struct size_value *b, struct size_value *ta, struct size_value *tb);

int to_size(char *val, struct size_value *psize); 

void from_size(struct size_value *psize, char *cvnbuf); 

