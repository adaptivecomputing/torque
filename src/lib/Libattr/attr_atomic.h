#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* svrattrl, attribute, attribute_def */

int attr_atomic_set(struct svrattrl *plist, attribute *old, attribute *new, attribute_def *pdef, int limit, int unkn, int privil, int *badattr);

int attr_atomic_node_set(struct svrattrl *plist, attribute *old, attribute *new, attribute_def *pdef, int limit, int unkn, int privil, int *badattr);

void attr_atomic_kill(attribute *temp, attribute_def *pdef, int limit);



