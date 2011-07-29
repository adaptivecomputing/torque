#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute */
#include "list_link.h" /* tlist_head */
#include "pbs_ifl.h" /* batch_op */
#include "resource.h" /* resource_def */

int decode_resc(struct attribute *patr, char *name, char *rescn, char *val, int perm); 

int encode_resc(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int ac_perm); 

int set_resc(struct attribute *old, struct attribute *new, enum batch_op op);

int comp_resc(struct attribute *attr, struct attribute *with); 

int comp_resc2(struct attribute *attr, struct attribute *with, int IsQueueCentric, char *EMsg, enum compare_types type); 

void free_resc(attribute *pattr);

resource_def *find_resc_def(resource_def *rscdf, char *name, int limit); 

resource *find_resc_entry(attribute *pattr, resource_def *rscdf); 

int action_resc(attribute *pattr, void *pobject, int actmode);

