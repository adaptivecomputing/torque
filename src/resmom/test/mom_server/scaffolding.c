#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <netinet/in.h> /* sockaddr_in, sockaddr */

#include "resizable_array.h" /* resizable_array */
#include "mom_hierarchy.h" /* mom_hierarchy_t, node_comm_t */
#include "resmon.h" /* config, rm_attribute */
#include "mom_main.h" /* DEFAULT_SERVER_STAT_UPDATES */
#include "log.h" /* MAXLINE, LOG_BUF_SIZE */
#include "pbs_ifl.h" /* PBS_MAXHOSTNAME */
#include "dynamic_string.h" /* dynamic_string */
#include "u_tree.h" /* AvlTree */
#include "net_connect.h" /* conn_type */
#include "server_limits.h" /* pbs_net_t. Also defined in net_connect.h */
#include "list_link.h" /* list_link, tlist_head */
#include "pbs_nodes.h" /* pbsnode */


resizable_array *received_statuses;
mom_hierarchy_t *mh;
struct config *config_array = NULL;
long system_ncpus = 0;
time_t time_now;
unsigned int pbs_mom_port = 0;
unsigned int default_server_port = 0;
int ServerStatUpdateInterval = DEFAULT_SERVER_STAT_UPDATES;
float ideal_load_val = -1.0;
int updates_waiting_to_send = 0;
const char *PBSServerCmds[] = { "NULL", "HELLO", "CLUSTER_ADDRS", "UPDATE", "STATUS", "GPU_STATUS", NULL };
const char *dis_emsg[10];
float max_load_val = -1.0;
char TMOMRejectConn[MAXLINE];
int PBSNodeCheckInterval;
int UpdateFailCount = 0;
char *auto_ideal_load = NULL;
char *path_spool;
char *auto_max_load = NULL;
unsigned int pbs_rm_port = 0;
char PBSNodeCheckPath[1024];
int internal_state = 0;
char PBSNodeMsgBuf[1024];
int alarm_time;
tlist_head svr_alljobs;
char mom_alias[PBS_MAXHOSTNAME + 1];
int LOGLEVEL = 0;
char log_buffer[LOG_BUF_SIZE];
int rm_errno;
time_t LastServerUpdateTime;



int MUReadPipe(char *Command, char *Buffer, int BufSize)
  {
  fprintf(stderr, "The call to MUReadPipe needs to be mocked!!\n");
  exit(1);
  }

node_comm_t *update_current_path(mom_hierarchy_t *nt)
  {
  fprintf(stderr, "The call to update_current_path needs to be mocked!!\n");
  exit(1);
  }

char *dependent(char *res, struct rm_attribute *attr)
  {
  fprintf(stderr, "The call to dependent needs to be mocked!!\n");
  exit(1);
  }

char * netaddr(struct sockaddr_in *ap)
  {
  fprintf(stderr, "The call to netaddr needs to be mocked!!\n");
  exit(1);
  }

int MUStrNCat(char **BPtr, int *BSpace, char *Src)
  {
  fprintf(stderr, "The call to MUStrNCat needs to be mocked!!\n");
  exit(1);
  }

char *reqgres(struct rm_attribute *attrib)
  {
  fprintf(stderr, "The call to reqgres needs to be mocked!!\n");
  exit(1);
  }

int read_tcp_reply(int sock, int protocol, int version, int command, int *exit_status)
  {
  fprintf(stderr, "The call to read_tcp_reply needs to be mocked!!\n");
  exit(1);
  }

char *conf_res(char *resline, struct rm_attribute *attr)
  {
  fprintf(stderr, "The call to conf_res needs to be mocked!!\n");
  exit(1);
  }

void clear_dynamic_string(dynamic_string *ds)
  {
  fprintf(stderr, "The call to clear_dynamic_string needs to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_record needs to be mocked!!\n");
  exit(1);
  }

struct rm_attribute *momgetattr(char *str)
  {
  fprintf(stderr, "The call to rm_attribute needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int MUSNPrintF(char **BPtr, int *BSpace, char *Format, ...)
  {
  fprintf(stderr, "The call to MUSNPrintF needs to be mocked!!\n");
  exit(1);
  }

int read_status_strings(int fds, int version)
  {
  fprintf(stderr, "The call to read_status_strings needs to be mocked!!\n");
  exit(1);
  }

int AVL_is_in_tree_no_port_compare(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_is_in_tree_no_port_compare needs to be mocked!!\n");
  exit(1);
  }

void log_ext(int errnum, char *routine, char *text, int severity)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

int diswcs(int stream, const char *value, size_t nchars)
  {
  fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
  exit(1);
  }

void add_conn(int sock, enum conn_type type, pbs_net_t addr, unsigned int port, unsigned int socktype, void (*func)(int))
  {
  fprintf(stderr, "The call to add_conn needs to be mocked!!\n");
  exit(1);
  }
 
node_comm_t *force_path_update(mom_hierarchy_t *nt)
  {
  fprintf(stderr, "The call to force_path_update needs to be mocked!!\n");
  exit(1);
  }

int client_to_svr(pbs_net_t hostaddr, unsigned int port, int local_port, char *EMsg)
  {
  fprintf(stderr, "The call to client_to_svr needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

int diswui(int stream, unsigned value)
  {
  fprintf(stderr, "The call to diswui needs to be mocked!!\n");
  exit(1);
  }

void send_update_within_ten()
  {
  fprintf(stderr, "The call to send_update_within_ten needs to be mocked!!\n");
  exit(1);
  }

uint16_t AVL_get_port_by_ipaddr(u_long key, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_get_port_by_ipaddr needs to be mocked!!\n");
  exit(1);
  }

void *next_thing(resizable_array *ra, int *iter)
  {
  fprintf(stderr, "The call to next_thing needs to be mocked!!\n");
  exit(1);
  }

int rpp_close(int index)
  {
  fprintf(stderr, "The call to rpp_close needs to be mocked!!\n");
  exit(1);
  }

int tcp_connect_sockaddr(struct sockaddr *sa, size_t sa_size)
  {
  fprintf(stderr, "The call to tcp_connect_sockaddr needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

char *size_fs(char *param)
  {
  fprintf(stderr, "The call to size_fs needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

AvlTree AVL_insert(u_long key, uint16_t port, struct pbsnode *node, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_insert needs to be mocked!!\n");
  exit(1);
  }

struct config *rm_search(struct config *where, char *what)
  {
  fprintf(stderr, "The call to rm_search needs to be mocked!!\n");
  exit(1);
  }

int diswsi(int stream, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }

int disrsi(int stream, int *retval)
  {
  fprintf(stderr, "The call to disrsi needs to be mocked!!\n");
  exit(1);
  }

