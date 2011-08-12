#ifndef _ATTR_RECOV_H
#define _ATTR_RECOV_H
#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute_def, attribute */

void save_setup(int fds);

int save_struct(char *pobj, unsigned int objsize);

int save_flush(void);

int save_attr(struct attribute_def *padef, struct attribute *pattr, int numattr);

#ifndef PBS_MOM
int save_attr_xml(struct attribute_def *padef, struct attribute *pattr, int numattr, int fds);
#endif /* ndef PBS_MOM */

int recov_attr(int fd, void *parent, struct attribute_def *padef, struct attribute *pattr, int limit, int unknown, int do_actions);

#endif /* _ATTR_RECOV_H */
