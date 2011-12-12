#ifndef _REPLY_SEND_H
#define _REPLY_SEND_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "libpbs.h" /* batch_reply */
#include "attribute.h" /* svrattrl */

/* static void set_err_msg(int code, char *msgbuf); */

/* static int dis_reply_write(int sfds, struct batch_reply *preply); */

int reply_send(struct batch_request *request);
int reply_send_svr(struct batch_request *request);
int reply_send_mom(struct batch_request *request);

void reply_ack(struct batch_request *preq);

void reply_free(struct batch_reply *prep);

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg);

void reply_badattr(int code, int aux, svrattrl *pal, struct batch_request *preq);

void reply_text(struct batch_request *preq, int code, char *text);

int  reply_jobid(struct batch_request *preq, char *jobid, int which);

#endif /* _REPLY_SEND_H */
