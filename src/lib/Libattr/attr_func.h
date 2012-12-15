#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute, attribute_def, svrattrl */
#include "list_link.h" /* tlist_head */

void clear_attr(pbs_attribute *pattr, attribute_def *pdef); 

int find_attr(struct attribute_def *attr_def, const char *name, int limit);

long attr_ifelse_long(pbs_attribute *attr1, pbs_attribute *attr2, long deflong);

void free_null(struct pbs_attribute *attr);

void free_noop(struct pbs_attribute *attr);

int comp_null(struct pbs_attribute *attr, struct pbs_attribute *with); 

svrattrl *attrlist_alloc(int szname, int szresc, int szval); 

svrattrl *attrlist_create(const char *aname, const char *rname, int vsize);

void free_attrlist(tlist_head *pattrlisthead);

char *parse_comma_string(char *start, char **ptr); 

int count_substrings(char *val, int *pcnt);

void attrl_fixlink(tlist_head *phead); 

