#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* svrattrl, pbs_attribute, attribute_def */

int attr_atomic_set(struct svrattrl *plist, pbs_attribute *old, pbs_attribute *newAttr, attribute_def *pdef, int limit, int unkn, int privil, int *badattr);

void attr_atomic_kill(pbs_attribute *temp, attribute_def *pdef, int limit);



