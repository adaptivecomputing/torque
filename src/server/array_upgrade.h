#ifndef _ARRAY_UPGRADE_H
#define _ARRAY_UPGRADE_H
#include "license_pbs.h" /* See here for the software license */

#include "array.h" /* job_array */

int array_upgrade(job_array *pa, int fds, int version, int *old_version);

#endif /* _ARRAY_UPGRADE_H */
