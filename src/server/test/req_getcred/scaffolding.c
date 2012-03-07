#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "net_connect.h" /* connection */
#include "server_limits.h" /* PBS_NET_MAX_CONNECTIONS */
#include "batch_request.h" /* batch_request */

char *path_credentials;
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];


size_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socketneeds to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket( int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject needs to be mocked!!\n");
  exit(1);
  }
