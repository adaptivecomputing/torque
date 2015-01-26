#ifndef _REQ_SHUTDOWN_H
#define _REQ_SHUTDOWN_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */

void svr_shutdown(int type);

void shutdown_ack(void);

int req_shutdown(struct batch_request *preq);

#endif /* _REQ_SHUTDOWN_H */
