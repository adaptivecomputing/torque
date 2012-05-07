#ifndef _DIS_READ_H
#define _DIS_READ_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "libpbs.h" /* batch_reply */
#include "tcp.h" /* tcp_chan */

int dis_request_read(struct tcp_chan *chan, struct batch_request *request);

int DIS_reply_read(struct tcp_chan *chan, struct batch_reply *preply);

#endif /* _DIS_READ_H */
