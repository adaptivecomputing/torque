#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <netdb.h>

#include "threadpool.h"
#include "net_connect.h"
#include "log.h"

const char *msg_daemonname = "unset";
bool trqauthd_up = true;
threadpool_t *request_pool;

void log_get_set_eventclass(int *objclass, SGetter action) {}

int debug_mode = 0;
int socket_get_tcp()
  {
  fprintf(stderr, "The call to socket_get_tcp needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

void log_record(int a, int b, const char *c, const char *d) {}

int get_listen_socket(struct addrinfo *addr_info)
  {
  return(0);
  }

int add_conn(

  int            sock,    /* socket associated with connection */
  enum conn_type type,    /* type of connection */
  pbs_net_t      addr,    /* IP address of connected host */
  unsigned int   port,    /* port number (host order) on connected host */
  unsigned int   socktype, /* inet or unix */
  void *(*func)(void *))  /* function to invoke on data rdy to read */

  {
  return(0);
  }

int enqueue_threadpool_request(

  void *(*func)(void *),
  void *arg,
  threadpool *tp)

  {
  return(0);
  }

void log_get_host_port(char *output, unsigned long size) {}

int pbs_getaddrinfo(const char *pNode,struct addrinfo *pHints,struct addrinfo **ppAddrInfoOut)
  {
  return(0);
  }

void log_close(

  int msg)  /* BOOLEAN - write close message */

  {
  return;
  }
