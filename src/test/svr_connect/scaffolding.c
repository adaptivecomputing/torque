#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* connect_handle */
#include "pbs_ifl.h" /* PBS_MAXUSER */
#include "net_connect.h" /* pbs_net_t, conn_type, connection */
#include "pbs_nodes.h" /* pbsnode */
#include "server_limits.h" /* PBS_NET_MAX_CONNECTIONS */
#include "tcp.h"

struct connect_handle connection[10];
char pbs_current_user[PBS_MAXUSER];
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
pbs_net_t pbs_server_addr;
unsigned int pbs_server_port_dis;
int LOGLEVEL = 10; /* force logging code to be exercised as tests run */


int find_node_called;
int node_unlocked;
bool free_node_on_unlock;

int addr_ok(pbs_net_t addr, struct pbsnode *pnode)
  {
  return(1);
  }

int get_connection_entry(int *conn_pos)
  {
  fprintf(stderr, "The call to get_connection_entry to be mocked!!\n");
  exit(1);
  }

char *netaddr_pbs_net_t(pbs_net_t ipadd)
  {
  fprintf(stderr, "The call to netaddr_pbs_net_t to be mocked!!\n");
  exit(1);
  }

ssize_t read_blocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_blocking_socket to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup to be mocked!!\n");
  exit(1);
  }

int get_num_connections()
  {
  fprintf(stderr, "The call to get_num_connections to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(tcp_chan *chan)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush to be mocked!!\n");
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
  return(0);
  }

int encode_DIS_ReqHdr(struct tcp_chan *chan, int reqt, char *user)
  {
  fprintf(stderr, "The call to encode_DIS_ReqHdr to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

int lock_node(
    
  struct pbsnode *the_node,
  const char     *id,
  const char     *msg,
  int             logging)

  {
  return(0);
  }

int unlock_node(
    
  struct pbsnode *the_node,
  const char     *id,
  const char     *msg,
  int             logging)

  {
  node_unlocked++;

  if (free_node_on_unlock)
    free(the_node);

  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int l, const char *func_name, const char *msg, int o) {}

int socket_to_handle(int sock, int *my_err)
  {
  return(0);
  }

struct pbsnode *find_nodebyname(const char *node_name)
  {
  find_node_called++;
  return(NULL);
  }
