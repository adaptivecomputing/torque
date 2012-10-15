#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <netdb.h>

#include "net_connect.h"
#include "log.h"

char *msg_daemonname = "unset";

void log_get_set_eventclass(int *objclass, SGetter action) {}

int debug_mode = 0;
int socket_get_tcp()
  {
  fprintf(stderr, "The call to socket_get_tcp needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, const char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

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
  void *arg)

  {
  return(0);
  }
