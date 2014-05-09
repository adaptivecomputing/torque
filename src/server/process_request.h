#ifndef _PROCESS_REQUEST_H
#define _PROCESS_REQUEST_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "tcp.h" /* tcp_chan */

#ifdef ENABLE_UNIX_SOCKETS
int get_creds(int   sd, char *username, char *hostname);
#endif

int dispatch_request(int sfds, struct batch_request *request);

int close_quejob_by_jobid(char *job_id);

#endif /* _PROCESS_REQUEST_H */
