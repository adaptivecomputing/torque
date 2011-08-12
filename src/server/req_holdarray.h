#include "license_pbs.h" /* See here for the software license */
#ifndef _REQ_HOLDARRAY_H
#define _REQ_HOLDARRAY_H

#include "attribute.h" /* attribute */

void hold_job(attribute *temphold, void *j);

void *req_holdarray(void *vp);

#endif /* _REQ_HOLDARRAY_H */
