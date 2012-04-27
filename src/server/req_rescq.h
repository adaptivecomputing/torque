#ifndef _REQ_RESCQ_H
#define _REQ_RESCQ_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */

int req_rescq(struct batch_request *preq);

int req_rescreserve(struct batch_request *preq);

int req_rescfree(struct batch_request *preq);

#endif /* _REQ_RESCQ_H */
