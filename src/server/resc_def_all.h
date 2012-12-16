#ifndef _RESC_DEF_ALL_H
#define _RESC_DEF_ALL_H
#include "license_pbs.h" /* See here for the software license */

#include "resource.h" /* resource */
#include "attribute.h" /* pbs_attribute */

int init_resc_defs(void);

/* static int decode_nodes(pbs_attribute *patr, char *name, const char *rescn, const char *val, int perm); */

int ctnodes(char *spec);

long count_proc(char *spec);

int set_node_ct(resource *pnodesp, pbs_attribute *pattr, int actmode);

int set_proc_ct(resource *pprocsp, pbs_attribute *pattr, int actmode);

/* static int set_tokens(struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op); */

/* static int set_mppnodect(resource *res, pbs_attribute *attr, int op); */

#endif /* _RESC_DEF_ALL_H */
