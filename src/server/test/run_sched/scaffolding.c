#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h> /* pthread_mutex_t */

#include "net_connect.h" /* listener_connection, MAXLISTENERS, pbs_net_t */
#include "pbs_ifl.h" /* PBS_MAXSERVERNAME */
#include "server_limits.h" /* PBS_NET_MAX_CONNECTIONS */
#include "sched_cmds.h" /* SCH_SCHEDULE_NULL */
#include "pbs_nodes.h" /* pbsnode */

unsigned int pbs_scheduler_port;
listener_connection listener_conns[MAXLISTENERS];
char *msg_sched_called = "Scheduler was sent the command %s";
char *msg_listnr_nocall = "Could not contact Listener";
char *msg_listnr_called = "Listener %d sent event %s";
char *msg_sched_nocall = "Could not contact Scheduler";
char server_name[PBS_MAXSERVERNAME + 1];
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
pbs_net_t pbs_scheduler_addr;
int svr_do_schedule = SCH_SCHEDULE_NULL;
pthread_mutex_t *svr_do_schedule_mutex;
pthread_mutex_t *scheduler_sock_jobct_mutex;
pthread_mutex_t *listener_command_mutex;


void *process_pbs_server_port(void *new_sock)
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

void *process_request(void *new_sock)
  {
  fprintf(stderr, "The call to process_request to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn to be mocked!!\n");
  exit(1);
  }

void net_add_close_func(int sd, void (*func)(int), int has_mutex)
  {
  fprintf(stderr, "The call to net_add_close_func to be mocked!!\n");
  exit(1);
  }



