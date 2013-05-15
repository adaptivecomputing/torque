#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h> /* pthread_mutex_t */

#include "net_connect.h" /* listener_connection, MAXLISTENERS, pbs_net_t */
#include "pbs_ifl.h" /* PBS_MAXSERVERNAME */
#include "server_limits.h" /* PBS_NET_MAX_CONNECTIONS */
#include "sched_cmds.h" /* SCH_SCHEDULE_NULL */
#include "pbs_nodes.h" /* pbsnode */
#include "tcp.h"

unsigned int pbs_scheduler_port;
listener_connection listener_conns[MAXLISTENERS];
const char *msg_sched_called = "Scheduler was sent the command %s";
const char *msg_listnr_nocall = "Could not contact Listener";
const char *msg_listnr_called = "Listener %d sent event %s";
const char *msg_sched_nocall = "Could not contact Scheduler";
char server_name[PBS_MAXSERVERNAME + 1];
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
pbs_net_t pbs_scheduler_addr;
int svr_do_schedule = SCH_SCHEDULE_NULL;
pthread_mutex_t *svr_do_schedule_mutex;
pthread_mutex_t *scheduler_sock_jobct_mutex;
pthread_mutex_t *listener_command_mutex;
int LOGLEVEL = 10;

int process_pbs_server_port(int sock, int is_scheduler_port, long *args)
  {
  fprintf(stderr, "The call to process_pbs_server_port to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup to be mocked!!\n");
  exit(1);
  }

void bad_node_warning(pbs_net_t addr, struct pbsnode *the_node)
  {
  fprintf(stderr, "The call to bad_node_warning to be mocked!!\n");
  exit(1);
  }

int add_conn(int sock, enum conn_type type, pbs_net_t addr, unsigned int port, unsigned int socktype, void *(*func)(void *))
  {
  fprintf(stderr, "The call to add_conn to be mocked!!\n");
  exit(1);
  }

int client_to_svr(pbs_net_t hostaddr, unsigned int port, int local_port, char *EMsg)
  {
  fprintf(stderr, "The call to client_to_svr to be mocked!!\n");
  exit(1);
  }

int process_request(tcp_chan *chan)
  {
  fprintf(stderr, "The call to process_request to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn to be mocked!!\n");
  exit(1);
  }

void net_add_close_func(int sd, void (*func)(int))
  {
  fprintf(stderr, "The call to net_add_close_func to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan) {}


int add_scheduler_conn(

  int            sock,    /* socket associated with connection */
  enum conn_type type,    /* type of connection */
  pbs_net_t      addr,    /* IP address of connected host */
  unsigned int   port,    /* port number (host order) on connected host */
  unsigned int   socktype, /* inet or unix */
  void *(*func)(void *))  /* function to invoke on data rdy to read */

  {
  return(0);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int l, const char *func_name, const char *msg, int o) {}
