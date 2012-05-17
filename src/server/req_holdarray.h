#include "license_pbs.h" /* See here for the software license */
#ifndef _REQ_HOLDARRAY_H
#define _REQ_HOLDARRAY_H

#include "attribute.h" /* pbs_attribute */
#include "batch_request.h"

void hold_job(pbs_attribute *temphold, void *j);

int req_holdarray(struct batch_request *preq);

#endif /* _REQ_HOLDARRAY_H */
