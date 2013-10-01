#include "license_pbs.h" /* See here for the software license */
#include <string>
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "net_connect.h" /* connection */
#include "server_limits.h" /* PBS_NET_MAX_CONNECTIONS */
#include "batch_request.h" /* batch_request */

char *path_credentials;
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
int rejected = FALSE;
int acked = FALSE;
int LOGLEVEL = 3;


size_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socketneeds to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  acked = TRUE;
  }

ssize_t write_nonblocking_socket( int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  rejected = TRUE;
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

int pthread_mutex_lock (pthread_mutex_t *__mutex) throw()
  {
  return 0;
  }
int pthread_mutex_unlock (pthread_mutex_t *__mutex) throw()
  {
  return 0;
  }

void get_cached_fullhostname(unsigned long addr, std::string &fullhostname)
  {
  if (addr == 3577385)
    fullhostname = "napali.ac";
  }

unsigned long get_connectaddr(int sock, int mutex)
  {
  return(svr_conn[sock].cn_addr);
  }


void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
