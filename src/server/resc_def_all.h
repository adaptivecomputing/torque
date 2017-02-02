#ifndef _RESC_DEF_ALL_H
#define _RESC_DEF_ALL_H
#include "license_pbs.h" /* See here for the software license */

#include "resource.h" /* resource */
#include "attribute.h" /* pbs_attribute */

int init_resc_defs(void);

int ctnodes(char *spec);

long count_proc(char *spec);

int set_node_ct(resource *pnodesp, pbs_attribute *pattr, int actmode);

int set_proc_ct(resource *pprocsp, pbs_attribute *pattr, int actmode);

#endif /* _RESC_DEF_ALL_H */
