#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute, attribute_def, svrattrl */
#include "list_link.h" /* tlist_head */
#include "pbs_nodes.h" /* struct prop */

/* static struct node_state */

int PNodeStateToString(int SBM, char *Buf, int BufSize); 

int encode_state(pbs_attribute *pattr, tlist_head *ph, char *aname, char *rname, int mode, int perm); 

int encode_ntype(pbs_attribute *pattr, tlist_head *ph, char *aname, char *rname, int mode, int perm); 

int encode_jobs(pbs_attribute *pattr, tlist_head *ph, char *aname, char *rname, int mode, int perm); 

int decode_state(pbs_attribute *pattr, char *name, char *rescn, char *val, int perm); 

int decode_ntype(pbs_attribute *pattr, char *name, char *rescn, char *val, int perm); 

void free_prop_list(struct prop *prop);

int set_node_state(pbs_attribute *pattr, pbs_attribute *new, enum batch_op op);

int set_node_ntype(pbs_attribute *pattr, pbs_attribute *new, enum batch_op op);

/* static int set_nodeflag(char *str, short *pflag); */

int node_state(pbs_attribute *new, void *pnode, int actmode); 

int node_ntype(pbs_attribute *new, void *pnode, int actmode);

int node_prop_list(pbs_attribute *new, void *pnode, int actmode); 

int node_status_list(pbs_attribute *new, void *pnode, int actmode); 

int node_gpustatus_list(pbs_attribute *new, void *pnode, int actmode); 

int node_note(pbs_attribute *new, void *pnode, int actmode); 

int set_note_str(struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op);

