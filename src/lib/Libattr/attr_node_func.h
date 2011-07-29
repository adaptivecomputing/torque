#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute, attribute_def, svrattrl */
#include "list_link.h" /* tlist_head */

/* static struct node_state */

int PNodeStateToString(int SBM, char *Buf, int BufSize); 

int encode_state(attribute *pattr, tlist_head *ph, char *aname, char *rname, int mode, int perm); 

int encode_ntype(attribute *pattr, tlist_head *ph, char *aname, char *rname, int mode, int perm); 

int encode_jobs(attribute *pattr, tlist_head *ph, char *aname, char *rname, int mode, int perm); 

int decode_state(attribute *pattr, char *name, char *rescn, char *val, int perm); 

int decode_ntype(attribute *pattr, char *name, char *rescn, char *val, int perm); 

void free_prop_list(struct prop *prop);

int set_node_state(attribute *pattr, attribute *new, enum batch_op op);

int set_node_ntype(attribute *pattr, attribute *new, enum batch_op op);

/* static int set_nodeflag(char *str, short *pflag); */

int node_state(attribute *new, void *pnode, int actmode); 

int node_ntype(attribute *new, void *pnode, int actmode);

int node_prop_list(attribute *new, void *pnode, int actmode); 

int node_status_list(attribute *new, void *pnode, int actmode); 

int node_gpustatus_list(attribute *new, void *pnode, int actmode); 

int node_note(attribute *new, void *pnode, int actmode); 

int set_note_str(struct attribute *attr, struct attribute *new, enum batch_op op);

