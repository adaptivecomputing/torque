#ifndef _MOM_PROCESS_REQUEST_H
#define _MOM_PROCESS_REQUEST_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */

void *mom_process_request(void *sock_num);

void mom_dispatch_request(int  sfds, struct batch_request *request);

/* static void mom_close_client(int sfds); */

struct batch_request *alloc_br(int type);

/* static void close_quejob(int sfds); */

void free_br(struct batch_request *preq);

/* static void freebr_manage(struct rq_manage *pmgr); */

/* static void freebr_cpyfile(struct rq_cpyfile *pcf); */

#endif /* _MOM_PROCESS_REQUEST_H */
