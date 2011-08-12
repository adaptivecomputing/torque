#ifndef _RESC_DEF_ALL_H
#define _RESC_DEF_ALL_H
#include "license_pbs.h" /* See here for the software license */

#include "resource.h" /* resource */
#include "attribute.h" /* attribute */

int init_resc_defs(void);

/* static int decode_nodes(attribute *patr, char *name, char *rescn, char *val, int perm); */

int ctnodes(char *spec);

long count_proc(char *spec);

int set_node_ct(resource *pnodesp, attribute *pattr, int actmode);

int set_proc_ct(resource *pprocsp, attribute *pattr, int actmode);

/* static int set_tokens(struct attribute *attr, struct attribute *new, enum batch_op op); */

/* static int set_mppnodect(resource *res, attribute *attr, int op); */

#endif /* _RESC_DEF_ALL_H */
