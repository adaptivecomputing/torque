#ifndef _DIS_READ_H
#define _DIS_READ_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "batch_reply.h" /* batch_reply */

int dis_request_read(int sfds, struct batch_request *request);

int DIS_reply_read(int sock, struct batch_reply *preply);

#endif /* _DIS_READ_H */
