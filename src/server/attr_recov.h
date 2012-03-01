#ifndef _ATTR_RECOV_H
#define _ATTR_RECOV_H
#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute_def, pbs_attribute */

void save_setup(int fds);

int  save_struct(char *, unsigned int, int, char *, size_t *, size_t);

int save_attr(struct attribute_def *padef, struct pbs_attribute *pattr, int numattr, int fds, char *, size_t *, size_t);

#ifndef PBS_MOM
int save_attr_xml(struct attribute_def *padef, struct pbs_attribute *pattr, int numattr, int fds);
#endif /* ndef PBS_MOM */

int recov_attr(int fd, void *parent, struct attribute_def *padef, struct pbs_attribute *pattr, int limit, int unknown, int do_actions);

#endif /* _ATTR_RECOV_H */
