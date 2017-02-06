#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <netinet/in.h> /* sockaddr_in, sockaddr */
#include <vector>
#include <errno.h>
#include <sys/statfs.h>

#include "mom_hierarchy.h" /* mom_hierarchy_t, node_comm_t */
#include "mom_server.h"
#include "resmon.h" /* config, rm_attribute */
#include "mom_main.h" /* DEFAULT_SERVER_STAT_UPDATES */
#include "log.h" /* MAXLINE, LOG_BUF_SIZE */
#include "pbs_ifl.h" /* PBS_MAXHOSTNAME */
#include "u_tree.h" /* AvlTree */
#include "net_connect.h" /* conn_type */
#include "server_limits.h" /* pbs_net_t. Also defined in net_connect.h */
#include "list_link.h" /* list_link, tlist_head */
#include "pbs_nodes.h" /* pbsnode */
#include "pbs_config.h"
#include "container.hpp"
#include "machine.hpp"
#include "dis.h"
#include "mom_func.h"
#include "authorized_hosts.hpp"

#define MAXLINE 1024

bool force_layout_update = false;
std::list<mom_job *> alljobs_list;
char log_buffer[LOG_BUF_SIZE];
char *apbasil_protocol = NULL;
char *apbasil_path = NULL;
int is_reporter_mom = FALSE;
mom_hierarchy_t *mh;
u_long              localaddr = 0;
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
char PBSNodeCheckPath[MAXLINE];
int internal_state = 0;
char PBSNodeMsgBuf[MAXLINE];
int alarm_time;
tlist_head svr_alljobs;
char mom_alias[PBS_MAXHOSTNAME + 1];
char mom_host[PBS_MAXHOSTNAME + 1];
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
int rm_errno;
int needs_cluster_addrs;
time_t LastServerUpdateTime;
int received_cluster_addrs;
time_t       requested_cluster_addrs;
time_t       first_update_time = 0;
container::item_container<received_node *> received_statuses;
bool exit_called = false;
bool ForceServerUpdate = false;


char  ret_string[MAXLINE];
char  path_spool[] = "/var/spool";

bool no_error = true;
bool no_event = true;

#ifdef NUMA_SUPPORT
int       num_node_boards;
nodeboard node_boards[MAX_NODE_BOARDS]; 
int       numa_index;
#endif

#ifdef PENABLE_LINUX_CGROUPS
int              memory_pressure_threshold = 0; /* 0: off, >0: check and kill */
short            memory_pressure_duration  = 0; /* 0: off, >0: check and kill */
int              MOMConfigUseSMT           = 1; /* 0: off, 1: on */
Machine this_node;
//hwloc_topology_t topology;
#endif

struct tcp_chan default_chan;


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
  return NULL;
  }

const char *dependent(const char *res, struct rm_attribute *attr)
  {
  return NULL;
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
  return NULL;
  }

int read_tcp_reply(struct tcp_chan *chan, int protocol, int version, int command, int *exit_status)
  {
  *exit_status = DIS_SUCCESS;
  return *exit_status; 
  }

char *conf_res(char *resline, struct rm_attribute *attr)
  {
  fprintf(stderr, "The call to conf_res needs to be mocked!!\n");
  exit(1);
  }

struct rm_attribute *momgetattr(char *str)
  {
  fprintf(stderr, "The call to rm_attribute needs to be mocked!!\n");
  exit(1);
  }

struct tcp_chan *DIS_tcp_setup(int fd)
  {
  default_chan.sock = 0;
  return &default_chan;
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
  return DIS_SUCCESS;
  }

int diswcs(tcp_chan *chan, const char *value, size_t nchars)
  {
  return DIS_SUCCESS;
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
  return DIS_SUCCESS;
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

int rpp_close(int index)
  {
  fprintf(stderr, "The call to rpp_close needs to be mocked!!\n");
  exit(1);
  }

int close(int fd)
  {
  return(0);
  }

int tcp_connect_sockaddr(struct sockaddr *sa, size_t sa_size, bool use_log)
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

AvlTree AVL_clear_tree(AvlTree a)
  {
  fprintf(stderr, "The call to AVL_clear_tree needs to be mocked!!\n");
  exit(1);
  }

struct config *rm_search(struct config *where, const char *what)
  {
  return NULL;
  }

int diswsi(tcp_chan *chan, int value)
  {
  return DIS_SUCCESS;
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

int add_network_entry(mom_hierarchy_t *mh, const char *name, struct addrinfo *ai, unsigned short rm_port, int path, int level)
  {
  fprintf(stderr, "The call to add_network_entry needs to be mocked!!\n");
  exit(1);
  }

char *disrst(tcp_chan *chan, int *ret)
  {
  fprintf(stderr, "The call to disrst needs to be mocked!!\n");
  exit(1);
  }

mom_hierarchy_t *initialize_mom_hierarchy()
  {
  mom_hierarchy_t *nt = (mom_hierarchy_t *)calloc(1, sizeof(mom_hierarchy_t));
  nt->current_path  = -1;
  nt->current_level = -1;
  nt->current_node  = -1;

  return(nt);
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

  std::vector<std::string> &status,
  const char               *apbasil_path,
  const char               *apbasil_protocol)

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

Machine::Machine()
  {
  }

Machine::~Machine()
  {
  }

void Machine::displayAsJson(std::stringstream &out, bool jobs) const {}

Socket::Socket()
  {
  }

Socket::~Socket()
  {
  }

Chip::Chip(){}
Chip::~Chip(){}

Core::Core(){}
Core::~Core(){}

PCI_Device::PCI_Device(){}
PCI_Device::~PCI_Device(){}


time_t get_stat_update_interval()

  {
  return ForceServerUpdate ? ServerStatUpdateInterval / 3 : ServerStatUpdateInterval;
  } /* END get_next_update_time() */


bool overwrite_cache(

  const char       *pNode,
  struct addrinfo **ppAddrInfoOut)

  {
  return(true);
  }

/*int gethostname(char *name, size_t len)
  {
  strcpy(name, "fattony3.ac");
  return(0);
  }*/

int add_gpu_status(std::vector<std::string> &mom_status)
  {
  return(0);
  }


#ifdef USE_RESOURCE_PLUGIN
void report_node_generic_resources(std::map<std::string, unsigned int> &gres) {}
void report_node_generic_metrics(std::map<std::string, double> &gmetrics) {}
void report_node_varattrs(std::map<std::string, std::string> &varattrs) {}
void report_node_features(std::set<std::string> &features) {}
#endif

bool authorized_hosts::is_authorized(unsigned long addr)
  {
  return(true);
  }

void authorized_hosts::list_authorized_hosts(std::string &output) {}

void authorized_hosts::add_authorized_address(unsigned long addr, unsigned short port, const std::string &hostname) {}

void authorized_hosts::clear() {}

authorized_hosts::authorized_hosts() {}
authorized_hosts auth_hosts;

void job::set_attr_flag(int index, int flag_val)

  {
  if ((index >= 0) &&
      (index < JOB_ATR_LAST))
    this->ji_wattr[index].at_flags = flag_val; 
  }

int job::get_mom_exitstat() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_exitstat);
  }

void job::set_mom_exitstat(int ev)
  {
  this->ji_qs.ji_un.ji_momt.ji_exitstat = ev;
  }

pbs_net_t job::get_svraddr() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_svraddr);
  }

void job::set_attr_flag_bm(int index, int bm)
  {
  if ((index >= 0) &&
      (index < JOB_ATR_LAST))
    this->ji_wattr[index].at_flags |= bm;
  }

struct timeval *job::get_tv_attr(int index)
  {
  return(&this->ji_wattr[index].at_val.at_timeval);
  }

int job::set_tv_attr(int index, struct timeval *tv)
  {
  memcpy(&(this->ji_wattr[index].at_val.at_timeval), tv, sizeof(struct timeval));
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

int job::set_resc_attr(int index, std::vector<resource> *resources)
  {
  this->ji_wattr[index].at_val.at_ptr = resources;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

void job::set_exec_exitstat(int ev)
  {
  this->ji_qs.ji_un.ji_exect.ji_exitstat = ev;
  }

unsigned short job::get_ji_mom_rmport() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_mom_rmport);
  }

int job::set_creq_attr(int index, complete_req *cr)
  {
  this->ji_wattr[index].at_val.at_ptr = cr;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

void job::set_qs_version(int version)
  {
  this->ji_qs.qs_version = version;
  }

void job::set_queue(const char *queue)
  {
  snprintf(this->ji_qs.ji_queue, sizeof(this->ji_qs.ji_queue), "%s", queue);
  }

int job::get_un_type() const
  {
  return(this->ji_qs.ji_un_type);
  }

void job::set_ji_momaddr(unsigned long momaddr)
  {
  this->ji_qs.ji_un.ji_exect.ji_momaddr = momaddr;
  }

void job::set_ji_mom_rmport(unsigned short mom_rmport)
  {
  this->ji_qs.ji_un.ji_exect.ji_mom_rmport = mom_rmport;
  }

void job::set_ji_momport(unsigned short momport)
  {
  this->ji_qs.ji_un.ji_exect.ji_momport = momport;
  }

const char *job::get_queue() const
  {
  return(this->ji_qs.ji_queue);
  }

void job::set_scriptsz(size_t scriptsz)
  {
  this->ji_qs.ji_un.ji_newt.ji_scriptsz = scriptsz;
  }

size_t job::get_scriptsz() const
  {
  return(this->ji_qs.ji_un.ji_newt.ji_scriptsz);
  }

pbs_net_t job::get_fromaddr() const
  {
  return(this->ji_qs.ji_un.ji_newt.ji_fromaddr);
  }

int job::get_fromsock() const
  {
  return(this->ji_qs.ji_un.ji_newt.ji_fromsock);
  }

void job::set_fromaddr(pbs_net_t fromaddr)
  {
  this->ji_qs.ji_un.ji_newt.ji_fromaddr = fromaddr;
  }

void job::set_fromsock(int sock)
  {
  this->ji_qs.ji_un.ji_newt.ji_fromsock = sock;
  }

int job::get_qs_version() const
  {
  return(this->ji_qs.qs_version);
  }

void job::set_un_type(int type)
  {
  this->ji_qs.ji_un_type = type;
  }

int job::get_exec_exitstat() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_exitstat);
  }

int job::get_svrflags() const
  {
  return(this->ji_qs.ji_svrflags);
  }

void job::set_modified(bool m)
  {
  this->ji_modified = m;
  }

void job::set_attr(int index)
  {
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  }

void job::set_fileprefix(const char *prefix)
  {
  strcpy(this->ji_qs.ji_fileprefix, prefix);
  }

int job::set_char_attr(int index, char c)
  {
  this->ji_wattr[index].at_val.at_char = c;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

void job::set_svrflags(int flags)
  {
  this->ji_qs.ji_svrflags = flags;
  }

const char *job::get_destination() const
  {
  return(this->ji_qs.ji_destin);
  }

void job::free_attr(int index)
  {
  }

void job::set_substate(int substate)
  {
  this->ji_qs.ji_substate = substate;
  }

void job::set_state(int state)
  {
  this->ji_qs.ji_state = state;
  }

void job::set_destination(const char *destination)
  {
  snprintf(this->ji_qs.ji_destin, sizeof(this->ji_qs.ji_destin), "%s", destination);
  }

pbs_net_t job::get_ji_momaddr() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_momaddr);
  }

bool job::has_been_modified() const
  {
  return(this->ji_modified);
  }

tlist_head *job::get_list_attr(int index)
  {
  return(&this->ji_wattr[index].at_val.at_list);
  }

complete_req *job::get_creq_attr(int index) const
  {
  complete_req *cr = NULL;
  if (this->ji_wattr[index].at_flags & ATR_VFLAG_SET)
    cr = (complete_req *)this->ji_wattr[index].at_val.at_ptr;

  return(cr);
  }

void job::set_exgid(unsigned int gid)
  {
  this->ji_qs.ji_un.ji_momt.ji_exgid = gid;
  }

void job::set_exuid(unsigned int uid)
  {
  this->ji_qs.ji_un.ji_momt.ji_exuid = uid;
  }

unsigned short job::get_ji_momport() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_momport);
  }

void job::set_jobid(const char *jobid)
  {
  strcpy(this->ji_qs.ji_jobid, jobid);
  }

int job::get_attr_flags(int index) const
  {
  return(this->ji_wattr[index].at_flags);
  }

struct jobfix &job::get_jobfix()
  {
  return(this->ji_qs);
  }

int job::set_bool_attr(int index, bool b)
  {
  this->ji_wattr[index].at_val.at_bool = b;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

bool job::get_bool_attr(int index) const
  {
  return(this->ji_wattr[index].at_val.at_bool);
  }

std::vector<resource> *job::get_resc_attr(int index)
  {
  return((std::vector<resource> *)this->ji_wattr[index].at_val.at_ptr);
  }

const char *job::get_str_attr(int index) const
  {
  return(this->ji_wattr[index].at_val.at_str);
  }

const char *job::get_jobid() const
  {
  return(this->ji_qs.ji_jobid);
  }

int job::get_substate() const
  {
  return(this->ji_qs.ji_substate);
  }

int job::get_state() const
  {
  return(this->ji_qs.ji_state);
  }

void job::unset_attr(int index)
  {
  this->ji_wattr[index].at_flags = 0;
  }

bool job::is_attr_set(int index) const
  {
  return((this->ji_wattr[index].at_flags & ATR_VFLAG_SET) != 0);
  }

const char *job::get_fileprefix() const
  {
  return(this->ji_qs.ji_fileprefix);
  }

int job::set_long_attr(int index, long l)
  {
  this->ji_wattr[index].at_val.at_long = l;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

int job::set_str_attr(int index, char *str)
  {
  this->ji_wattr[index].at_val.at_str = str;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

long job::get_long_attr(int index) const
  {
  return(this->ji_wattr[index].at_val.at_long);
  }

time_t job::get_start_time() const
  {
  return(this->ji_qs.ji_stime);
  }

void job::set_start_time(time_t t)
  {
  this->ji_qs.ji_stime = t;
  }

pbs_attribute *job::get_attr(int index)
  {
  return(this->ji_wattr + index);
  }

unsigned int job::get_exgid() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_exgid);
  }

unsigned int job::get_exuid() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_exuid);
  }

void job::set_svraddr(pbs_net_t addr)
  {
  this->ji_qs.ji_un.ji_momt.ji_svraddr = addr;
  }

char job::get_char_attr(int index) const
  {
  return(this->ji_wattr[index].at_val.at_char);
  }

