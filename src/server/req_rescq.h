#ifndef _REQ_RESCQ_H
#define _REQ_RESCQ_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */

void *req_rescq(void *preq);

void *req_rescreserve(void *preq);

void *req_rescfree(void *preq);

#endif /* _REQ_RESCQ_H */
