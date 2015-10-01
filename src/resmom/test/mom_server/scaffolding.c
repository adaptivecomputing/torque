#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <netinet/in.h> /* sockaddr_in, sockaddr */
#include <errno.h>
#include <sys/statfs.h>

#include "resizable_array.h" /* resizable_array */
#include "mom_hierarchy.h" /* mom_hierarchy_t, node_comm_t */
#include "mom_server.h"
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
#include "pbs_config.h"

bool            ForceServerUpdate = false;
char log_buffer[LOG_BUF_SIZE];
char *apbasil_protocol = NULL;
char *apbasil_path = NULL;
int is_reporter_mom = 0;
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
char *auto_max_load = NULL;
unsigned int pbs_rm_port = 0;
char PBSNodeCheckPath[1024];
int internal_state = 0;
char PBSNodeMsgBuf[1024];
int alarm_time;
tlist_head svr_alljobs;
char mom_alias[PBS_MAXHOSTNAME + 1];
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
int rm_errno;
int needs_cluster_addrs;
time_t LastServerUpdateTime;
int received_cluster_addrs;
time_t       requested_cluster_addrs;
time_t       first_update_time = 0;

char  ret_string[MAXLINE];
char  path_spool[] = "/var/spool";

bool no_error = true;
bool no_event = true;

#ifdef NUMA_SUPPORT
int       num_node_boards;
nodeboard node_boards[MAX_NODE_BOARDS]; 
int       numa_index;
#endif

#ifdef PENABLE_LINUX26_CPUSETS
int              memory_pressure_threshold = 0; /* 0: off, >0: check and kill */
short            memory_pressure_duration  = 0; /* 0: off, >0: check and kill */
int              MOMConfigUseSMT           = 1; /* 0: off, 1: on */
//hwloc_topology_t topology;
#endif


int MUReadPipe(char *Command, char *Buffer, int BufSize)
  {
  if ((no_error == true) && (no_event == true))
    {
    memset(Buffer, 0, BufSize);
    return(0);
    }

  if (no_error == false)
    {
    strcpy(Buffer, "ERROR");
    return(0);
    }

  if (no_event == false)
    {
    strcpy(Buffer, "EVENT:");
    return(0);
    }

  return(0);
  }

node_comm_t *update_current_path(mom_hierarchy_t *nt)
  {
  fprintf(stderr, "The call to update_current_path needs to be mocked!!\n");
  exit(1);
  }

const char *dependent(const char *res, struct rm_attribute *attr)
  {
  fprintf(stderr, "The call to dependent needs to be mocked!!\n");
  exit(1);
  }

char * netaddr(struct sockaddr_in *ap)
  {
  fprintf(stderr, "The call to netaddr needs to be mocked!!\n");
  exit(1);
  }

int MUStrNCat(char **BPtr, int *BSpace, const char *Src)
  {
  fprintf(stderr, "The call to MUStrNCat needs to be mocked!!\n");
  exit(1);
  }

const char *reqgres(struct rm_attribute *attrib)
  {
  fprintf(stderr, "The call to reqgres needs to be mocked!!\n");
  exit(1);
  }

int read_tcp_reply(struct tcp_chan *chan, int protocol, int version, int command, int *exit_status)
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

int MUSNPrintF(char **BPtr, int *BSpace, const char *Format, ...)
  {
  fprintf(stderr, "The call to MUSNPrintF needs to be mocked!!\n");
  exit(1);
  }

int read_status_strings(tcp_chan *chan, int version)
  {
  fprintf(stderr, "The call to read_status_strings needs to be mocked!!\n");
  exit(1);
  }

int AVL_is_in_tree_no_port_compare(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_is_in_tree_no_port_compare needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(tcp_chan *chan)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

int diswcs(tcp_chan *chan, const char *value, size_t nchars)
  {
  fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
  exit(1);
  }

int add_conn(int sock, enum conn_type type, pbs_net_t addr, unsigned int port, unsigned int socktype, void *(*func)(void *))
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

int diswui(tcp_chan *chan, unsigned value)
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
  return 100;
  }

char *size_fs(char *param)
  {
  struct statfs fsbuf;
  

  /* We need to make up our own parameter */
  
  if (path_spool[0] != '/')
    {
    sprintf(log_buffer, "%s: not full path filesystem name: %s", __func__, path_spool);
    log_err(-1, __func__, log_buffer);

    rm_errno = RM_ERR_BADPARAM;

    return(NULL);
    }
      
    if (statfs(path_spool, &fsbuf) == -1)
      {
      log_err(errno, __func__, "statfs");
      rm_errno = RM_ERR_BADPARAM;
      return(NULL);
      }

#ifdef RPT_BAVAIL
#define RPT_STATFS_MEMBER f_bavail
#else
#define RPT_STATFS_MEMBER f_bfree
#endif

  sprintf(ret_string, "%lukb:%lukb",
                      (ulong)(((double)fsbuf.f_bsize * (double)fsbuf.RPT_STATFS_MEMBER) / 1024.0),
                      (ulong)(((double)fsbuf.f_bsize * (double)fsbuf.f_blocks) / 1024.0)); /* KB */

  return(ret_string);
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

struct config *rm_search(struct config *where, const char *what)
  {
  fprintf(stderr, "The call to rm_search needs to be mocked!!\n");
  exit(1);
  }

int diswsi(tcp_chan *chan, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }

int disrsi(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrsi needs to be mocked!!\n");
  exit(1);
  }

int pbs_disconnect_socket(int sock)
  {
  fprintf(stderr, "The call to pbs_disconnect_socket needs to be mocked!!\n");
  exit(1);
  }

int add_network_entry(mom_hierarchy_t *mh, char *name, struct addrinfo *ai, unsigned short rm_port, int path, int level)
  {
  fprintf(stderr, "The call to add_network_entry needs to be mocked!!\n");
  exit(1);
  }

int swap_things(resizable_array *ra, void *obj1, void *obj2)
  {
  fprintf(stderr, "The call to swap_things needs to be mocked!!\n");
  exit(1);
  }

char *disrst(tcp_chan *chan, int *ret)
  {
  fprintf(stderr, "The call to disrst needs to be mocked!!\n");
  exit(1);
  }

mom_hierarchy_t *initialize_mom_hierarchy()
  {
  fprintf(stderr, "The call to initialize_mom_hierarchy needs to be mocked!!\n");
  exit(1);
  }

int remove_last_thing(resizable_array *ra)
  {
  fprintf(stderr, "The call to remove_last_thing needs to be mocked!!\n");
  exit(1);
  }

void *next_thing_from_back(resizable_array *ra, int *iter)
  {
  fprintf(stderr, "The call to next_thing_from_back needs to be mocked!!\n");
  exit(1);
  }

int append_dynamic_string(dynamic_string *ds, const char *to_append)
  {
  fprintf(stderr, "The call to append_dynamic_string to be mocked!!\n");
  exit(1);
  }

dynamic_string *get_dynamic_string(int initial_size, const char *str)
  {
  fprintf(stderr, "The call to get_dynamic_string needs to be mocked!!\n");
  exit(1);
  }

void free_dynamic_string(dynamic_string *ds)
  {
  fprintf(stderr, "The call to attr_to_str needs to be mocked!!\n");
  exit(1);
  }

int delete_last_word_from_dynamic_string(dynamic_string *ds)
  {
  fprintf(stderr, "The call to delete_last_word_from_dynamic_string needs to be mocked!!\n");
  exit(1);
  }

void send_update_soon()
  {
  fprintf(stderr, "The call to send_update_soon needs to be mocked!!\n");
  exit(1);
  }

int AVL_list(AvlTree tree, char **Buf, long *current_len, long *max_len)
  {
  fprintf(stderr, "The call to AVL_list needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

int generate_alps_status(

  dynamic_string *status,
  const char     *apbasil_path,
  const char     *apbasil_protocol)

  {
  return(0);
  }

void free_mom_hierarchy(mom_hierarchy_t *mh) {}

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int type, const char *func_name, const char *msg, int o) {}

int pbs_getaddrinfo(

  const char       *pNode,
  struct addrinfo  *pHints,
  struct addrinfo **ppAddrInfoOut)

  {
  return(0);
  }

int concat_dynamic_strings(

    dynamic_string *source,
    dynamic_string *destination)
  {
  fprintf(stderr, "The call to concat_dynamic_strings needs to be mocked!!\n");
  exit(1);
  }

time_t get_stat_update_interval()

  {
  return ForceServerUpdate ? ServerStatUpdateInterval / 3 : ServerStatUpdateInterval;
  } /* END get_next_update_time() */
