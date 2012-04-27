#include "license_pbs.h" /* See here for the software license */
#ifndef _REQ_GETCRED_H
#define _REQ_GETCRED_H

#include "batch_request.h" /* batch_request */

void req_connect(struct batch_request *preq);

int get_encode_host(int s, char *munge_buf, struct batch_request *preq);

int get_UID(int s, char *munge_buf, struct batch_request *preq);
 
int write_munge_temp_file(struct batch_request *preq, char *mungeFileName);

int pipe_and_read_unmunge(char *mungeFileName, struct batch_request *preq, int sock);

int unmunge_request(int sock, struct batch_request *preq);

int req_authenuser(struct batch_request *preq);

int req_altauthenuser(struct batch_request *preq);

#endif /* _REQ_GETCRED_H */
