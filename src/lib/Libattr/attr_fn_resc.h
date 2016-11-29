#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */
#include "resource.h" /* resource_def */

int decode_resc(struct pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm);

int encode_resc(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int ac_perm);

int set_resc(struct pbs_attribute *old, struct pbs_attribute *newAttr, enum batch_op op);

int comp_resc(struct pbs_attribute *attr, struct pbs_attribute *with); 

void free_resc(pbs_attribute *pattr);

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit); 

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf); 

int action_resc(pbs_attribute *pattr, void *pobject, int actmode);

