#ifndef _PROCESS_REQUEST_H
#define _PROCESS_REQUEST_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "tcp.h" /* tcp_chan */

#ifdef ENABLE_UNIX_SOCKETS
int get_creds(int   sd, char *username, char *hostname);
#endif

int process_request(struct tcp_chan *chan);

int dispatch_request(int sfds, struct batch_request *request);

/* static void svr_close_client(int sfds); */

struct batch_request *alloc_br(int type);

int close_quejob_by_jobid(char *job_id);

/* static void close_quejob(int sfds); */

void free_br(struct batch_request *preq);

/* static void freebr_manage(struct rq_manage *pmgr); */

/* static void freebr_cpyfile(struct rq_cpyfile *pcf); */

/* static void free_rescrq(struct rq_rescq *pq); */

#endif /* _PROCESS_REQUEST_H */
