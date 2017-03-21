#include "license_pbs.h"
#include <pbs_config.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h> /* hostent */
#include <sstream>
#include <list>

#include "mom_func.h" /* sig_tbl */
#include "pbs_job.h" /* job, pjobexec_t, task, pjobexec_t */
#include "u_tree.h" /* AvlTree */
#include "libpbs.h" /* job_file */
#include "mom_hierarchy.h" /* mom_hierarchy_t */
#include "mom_server.h" 
#include "server_limits.h" /* pbs_net_t. Also defined in net_connect.h */
#include "threadpool.h" /* threadpool_t */
#include "list_link.h" /* list_link, tlist_head */
#include "pbs_nodes.h" /* pbsnode */
#include "attribute.h" /* pbs_attribute */
#include "resource.h" /* resource_def */
#include "log.h" /* LOG_BUF_SIZE */
#include "tcp.h"
#include "mom_config.h"
#include "machine.hpp"
#include <string>
#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>

extern mom_hierarchy_t *mh;

#ifdef PENABLE_LINUX_CGROUPS
#include "pbs_cpuset.h"
#include "node_internals.hpp"
#include "machine.hpp"

#endif

bool   thread_unlink_calls;
extern mom_hierarchy_t *mh;
std::list<job *> alljobs_list;
int              job_exit_wait_time = DEFAULT_JOB_EXIT_WAIT_TIME;
mom_server     mom_servers[PBS_MAXSERVER];
int mom_server_count = 0;
int MOMCudaVisibleDevices;
const char *msg_daemonname = "unset";
struct sig_tbl sig_tbl[2];
char pbs_current_user[PBS_MAXUSER];
extern char *server_alias;
const char *dis_emsg[10];
long *log_event_mask = NULL;
int rpp_dbprt = 0;
long pe_alarm_time;
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
struct config standard_config[2];
struct config dependent_config[2];
long MaxConnectTimeout = 5000000;
tlist_head svr_requests;
const char *msg_info_mom = "Torque Mom Version = %s, loglevel = %d";
threadpool_t *request_pool;
AvlTree okclients;
time_t wait_time = 10;
boost::ptr_vector<std::string> mom_status;
pthread_mutex_t log_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
char log_buffer[LOG_BUF_SIZE];
char      config_file[_POSIX_PATH_MAX];
char      xauth_path[1024];
int       MOMConfigRReconfig;
long      TJobStartBlockTime; /* seconds to wait for job to launch before backgrounding */
int       ServerStatUpdateInterval;
int       ignmem;
int       igncput;
int       PBSNodeCheckInterval;
int       hostname_specified;
char      rcp_path[MAXPATHLEN];
extern char      tmpdir_basename[];  /* for $TMPDIR */
float     max_load_val;
int       MOMConfigDownOnError;
int       mask_num;
extern char      PBSNodeMsgBuf[];
int       MOMConfigRestart;
attribute_def    job_attr_def[1];
int       LOGKEEPDAYS;
long      log_file_roll_depth;
char             extra_parm[] = "extra parameter(s)";
struct config   *config_array = NULL;
char           **maskclient = NULL; /* wildcard connections */
char             PBSNodeCheckPath[MAXLINE];
int       CheckPollTime;
char             rcp_args[MAXPATHLEN];
long      log_file_max_size;
char             mom_host[PBS_MAXHOSTNAME + 1];
int       rm_errno;
int       config_file_specified;
char             MOMConfigVersion[64];
struct config common_config[1];
char           **ArgV;
char            *OriginalPath;
int       resend_join_job_wait_time;
int       max_join_job_wait_time;
bool   parsing_hierarchy = false;
bool received_cluster_addrs;

int setbool(

  const char *value) /* I */

  {
  int enable = -1;

  if (value != NULL)
    {

    switch (value[0])
      {

      case 't':
      case 'T':
      case 'y':
      case 'Y':
      case '1':
        enable = 1;
        break;

      case 'f':
      case 'F':
      case 'n':
      case 'N':
      case '0':
        enable = 0;
        break;

      case 'o':
      case 'O':
        if ((strcasecmp(value,"on") == 0))
          enable = 1;
        else if ((strcasecmp(value,"off") == 0))
          enable = 0;
        break;

      }

    }

  return(enable);
  }

int wsi_ret = 0;
int wcs_ret = 0;
int flush_ret = 0;

void save_args(int argc, char **argv) {}

int log_remove_old(char *DirPath, unsigned long ExpireTime)
  {
  fprintf(stderr, "The call to long needs to be mocked!!\n");
  exit(1);
  }

void free_attrlist(list_link *l) {}

void attrl_fixlink(list_link *l) {}

int send_join_job_to_a_sister(job *pjob, int stream, eventent *ep, tlist_head phead, int node_id)
  {
  return(0);
  }

unsigned long jostartblocktime(const char *value)
  {
  return(1);
  }

int mom_close_poll(void)
  {
  fprintf(stderr, "The call to mom_close_poll needs to be mocked!!\n");
  exit(1);
  }

void init_abort_jobs(int recover)
  {
  fprintf(stderr, "The call to init_abort_jobs needs to be mocked!!\n");
  exit(1);
  }

int mom_server_add(const char *value)
  {
  fprintf(stderr, "The call to mom_server_add needs to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save needs to be mocked!!\n");
  exit(1);
  }

void mom_job_purge(job *pjob)
  {
  fprintf(stderr, "The call to job_purge needs to be mocked!!\n");
  exit(1);
  }

void scan_non_child_tasks(void)
  {
  fprintf(stderr, "The call to scan_non_child_tasks needs to be mocked!!\n");
  exit(1);
  }

extern "C"
{
unsigned int get_svrport(char *service_name, char *ptype, unsigned int pdefault)
  {
  fprintf(stderr, "The call to get_svrport needs to be mocked!!\n");
  exit(1);
  }
}

int get_la(double *rv)
  {
  fprintf(stderr, "The call to get_la needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int get_parent_and_child(char *start, char **parent, char **child, char **end)
  {
  fprintf(stderr, "The call to get_parent_and_child needs to be mocked!!\n");
  exit(1);
  }

int MUStrNCat(char **BPtr, int *BSpace, const char *Src)
  {
  fprintf(stderr, "The call to MUStrNCat needs to be mocked!!\n");
  exit(1);
  }

void scan_for_exiting(void)
  {
  fprintf(stderr, "The call to scan_for_exiting needs to be mocked!!\n");
  exit(1);
  }

void mom_checkpoint_set_directory_path(const char *str)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_directory_path needs to be mocked!!\n");
  exit(1);
  }

void exec_bail(job *pjob, int code, std::set<int> *sisters_contacted)
  {
  fprintf(stderr, "The call to exec_bail needs to be mocked!!\n");
  exit(1);
  }

int AVL_list(AvlTree tree, char **Buf, long *current_len, long *max_len)
  {
  fprintf(stderr, "The call to AVL_list needs to be mocked!!\n");
  exit(1);
  }

unsigned long mom_checkpoint_set_checkpoint_interval(const char *value)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_checkpoint_interval needs to be mocked!!\n");
  exit(1);
  }

int TMomCheckJobChild(pjobexec_t *TJE, int Timeout, int *Count, int *RC)
  {
  fprintf(stderr, "The call to TMomCheckJobChild needs to be mocked!!\n");
  exit(1);
  }

int message_job(job *pjob, enum job_file jft, char *text)
  {
  fprintf(stderr, "The call to message_job needs to be mocked!!\n");
  exit(1);
  }

int IamRoot()
  {
  fprintf(stderr, "The call to IamRoot needs to be mocked!!\n");
  exit(1);
  }

int add_network_entry(mom_hierarchy_t *nt, const char *name, struct addrinfo *ai, unsigned short rm_port, int path, int level)
  {
  fprintf(stderr, "The call to add_network_entry needs to be mocked!!\n");
  exit(1);
  }

int log_init(const char *suffix, const char *hostname)
  {
  fprintf(stderr, "The call to log_init needs to be mocked!!\n");
  exit(1);
  }

int post_epilogue(job *pjob, int ev)
  {
  fprintf(stderr, "The call to post_epilogue needs to be mocked!!\n");
  exit(1);
  }

char *get_job_envvar(job *pjob, const char *variable)
  {
  fprintf(stderr, "The call to get_job_envvar needs to be mocked!!\n");
  exit(1);
  }

char *netaddr_pbs_net_t(pbs_net_t ipadd)
  {
  fprintf(stderr, "The call to netaddr_pbs_net_t needs to be mocked!!\n");
  exit(1);
  }

unsigned long mom_checkpoint_set_checkpoint_script(const char *value)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_checkpoint_script needs to be mocked!!\n");
  exit(1);
  }

int mom_get_sample(void)
  {
  fprintf(stderr, "The call to mom_get_sample needs to be mocked!!\n");
  exit(1);
  }

void set_rpp_throttle_sleep_time(long sleep_time)
  {
  fprintf(stderr, "The call to set_rpp_throttle_sleep_time needs to be mocked!!\n");
  exit(1);
  }

int run_pelog(int which, char *specpelog, job *pjog, int pe_io_type, int deletejob)
  {
  fprintf(stderr, "The call to run_pelog needs to be mocked!!\n");
  exit(1);
  }

char *pbs_get_server_list(void)
  {
  fprintf(stderr, "The call to pbs_get_server_list needs to be mocked!!\n");
  exit(1);
  }

int tm_request(struct tcp_chan *chan, int version)
  {
  fprintf(stderr, "The call to tm_request needs to be mocked!!\n");
  exit(1);
  }

int csv_length(const char *csv_str)
  {
  fprintf(stderr, "The call to csv_length needs to be mocked!!\n");
  exit(1);
  }

char *csv_nth(const char *csv_str, int n)
  {
  fprintf(stderr, "The call to csv_nth needs to be mocked!!\n");
  exit(1);
  }

void dep_cleanup(void)
  {
  fprintf(stderr, "The call to dep_cleanup needs to be mocked!!\n");
  exit(1);
  }

size_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
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

int mom_over_limit(job *pjob)
  {
  fprintf(stderr, "The call to mom_over_limit needs to be mocked!!\n");
  exit(1);
  }

int AVL_is_in_tree_no_port_compare(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_is_in_tree_no_port_compare needs to be mocked!!\n");
  exit(1);
  }

void net_close(int but)
  {
  fprintf(stderr, "The call to net_close needs to be mocked!!\n");
  exit(1);
  }

int kill_task(job *pjob, task *ptask, int sig, int pg)
  {
  fprintf(stderr, "The call to kill_task needs to be mocked!!\n");
  exit(1);
  }

void mom_server_all_send_state(void)
  {
  fprintf(stderr, "The call to mom_server_all_send_state needs to be mocked!!\n");
  exit(1);
  }

void mom_server_all_init(void)
  {
  fprintf(stderr, "The call to mom_server_all_init needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(tcp_chan *chan)
  {
  return(flush_ret);
  }

int diswcs(tcp_chan *chan, const char *value, size_t nchars)
  {
  return(wcs_ret);
  }

void mom_checkpoint_check_periodic_timer(job *pjob)
  {
  fprintf(stderr, "The call to mom_checkpoint_check_periodic_timer needs to be mocked!!\n");
  exit(1);
  }

int chk_file_sec(const char *path, int isdir, int sticky, int disallow, int fullpath, char *SEMsg)
  {
  fprintf(stderr, "The call to chk_file_sec needs to be mocked!!\n");
  exit(1);
  }

int TMomFinalizeJob3(pjobexec_t *TJE, int ReadSize, int ReadErrno, int *SC)
  {
  fprintf(stderr, "The call to TMomFinalizeJob3 needs to be mocked!!\n");
  exit(1);
  }

unsigned long mom_checkpoint_set_restart_script(const char *value)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_restart_script needs to be mocked!!\n");
  exit(1);
  }

void mom_server_all_update_stat(void)
  {
  fprintf(stderr, "The call to mom_server_all_update_stat needs to be mocked!!\n");
  exit(1);
  }

void check_busy(double mla)
  {
  fprintf(stderr, "The call to check_busy needs to be mocked!!\n");
  exit(1);
  }

void mom_is_request(struct tcp_chan *chan, int version, int *cmdp,struct sockaddr_in *pSockAddr)
  {
  fprintf(stderr, "The call to mom_is_request needs to be mocked!!\n");
  exit(1);
  }

int mom_checkpoint_init(void)
  {
  fprintf(stderr, "The call to mom_checkpoint_init needs to be mocked!!\n");
  exit(1);
  }

int wait_request(time_t waittime, long *SState)
  {
  fprintf(stderr, "The call to wait_request needs to be mocked!!\n");
  exit(1);
  }

void log_close(int msg)
  {
  fprintf(stderr, "The call to log_close needs to be mocked!!\n");
  exit(1);
  }

void *get_next(

  list_link  pl,   /* I */
  char     *file, /* I */
  int      line) /* I */

  {
  if ((pl.ll_next == NULL) ||
      ((pl.ll_next == &pl) && (pl.ll_struct != NULL)))
    {
    return NULL;
    }

  return(pl.ll_next->ll_struct);
  }  /* END get_next() */

int log_open(char *filename, char *directory)
  {
  fprintf(stderr, "The call to log_open needs to be mocked!!\n");
  exit(1);
  }

int send_sisters(job *pjob, int com, int using_radix, std::set<int> *sisters_to_contact)
  {
  fprintf(stderr, "The call to send_sisters needs to be mocked!!\n");
  exit(1);
  }

int mom_set_use(job *pjob)
  {
  fprintf(stderr, "The call to mom_set_use needs to be mocked!!\n");
  exit(1);
  }

int initialize_threadpool(threadpool_t **pool, int min_threads, int max_threads, int max_idle_time)
  {
  fprintf(stderr, "The call to initialize_threadpool needs to be mocked!!\n");
  exit(1);
  }

void term_job(job *pjob)
  {
  fprintf(stderr, "The call to term_job needs to be mocked!!\n");
  exit(1);
  }

struct passwd * getpwnam_ext(char **user_buf, char * user_name)
  {
  fprintf(stderr, "The call to getpwnam_ext needs to be mocked!!\n");
  exit(1);
  }

char *disrst(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrst needs to be mocked!!\n");
  exit(1);
  }

int mom_open_poll(void)
  {
  fprintf(stderr, "The call to mom_open_poll needs to be mocked!!\n");
  exit(1);
  }

void dep_main_loop_cycle(void)
  {
  fprintf(stderr, "The call to dep_main_loop_cycle needs to be mocked!!\n");
  exit(1);
  }

int init_network(unsigned int socket, void *(*readfunc)(void *))
  {
  fprintf(stderr, "The call to init_network needs to be mocked!!\n");
  exit(1);
  }

void dep_initialize(void)
  {
  fprintf(stderr, "The call to dep_initialize needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

void log_roll(int max_depth)
  {
  fprintf(stderr, "The call to log_roll needs to be mocked!!\n");
  exit(1);
  }

int init_resc_defs(void)
  {
  fprintf(stderr, "The call to init_resc_defs needs to be mocked!!\n");
  exit(1);
  }

int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg)
  {
  fprintf(stderr, "The call to get_fullhostname needs to be mocked!!\n");
  exit(1);
  }

void scan_for_terminated(void)
  {
  fprintf(stderr, "The call to scan_for_terminated needs to be mocked!!\n");
  exit(1);
  }

long log_size(void)
  {
  fprintf(stderr, "The call to log_size needs to be mocked!!\n");
  exit(1);
  }

int task_save(task *ptask)
  {
  fprintf(stderr, "The call to task_save needs to be mocked!!\n");
  exit(1);
  }

void *mom_process_request(void *sock_num)
  {
  fprintf(stderr, "The call to mom_process_request needs to be mocked!!\n");
  exit(1);
  }

int setup_env(const char *filen)
  {
  fprintf(stderr, "The call to setup_env needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

void catch_child(int sig)
  {
  fprintf(stderr, "The call to catch_child needs to be mocked!!\n");
  exit(1);
  }

AvlTree AVL_insert(u_long key, uint16_t port, struct pbsnode *node, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_insert needs to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry needs to be mocked!!\n");
  exit(1);
  }

job *mom_find_job(const char *jobid)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_settimeout(long timeout)
  {
  fprintf(stderr, "The call to DIS_tcp_settimeout needs to be mocked!!\n");
  exit(1);
  }

mom_hierarchy_t *initialize_mom_hierarchy(void)
  {
  mom_hierarchy_t *nt = (mom_hierarchy_t *)calloc(1, sizeof(mom_hierarchy_t));
  return(nt);
  }

int diswsi(tcp_chan *chan, int value)
  {
  return(wsi_ret);
  }

void mom_server_all_diag(std::stringstream &output)
  {
  fprintf(stderr, "The call to mom_server_all_diag needs to be mocked!!\n");
  exit(1);
  }

int disrsi(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrsi needs to be mocked!!\n");
  exit(1);
  }

unsigned long mom_checkpoint_set_checkpoint_run_exe_name(const char *value)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_checkpoint_run_exe_name needs to be mocked!!\n");
  exit(1);
  }

void im_request(struct tcp_chan *chan, int version,struct sockaddr_in *pSockAddr)
  {
  fprintf(stderr, "The call to im_request needs to be mocked!!\n");
  exit(1);
  }

int lock_init()
  {
  fprintf(stderr, "The call to lock_init needs to be mocked!!\n");
  exit(1);
  }

char *netaddr(struct sockaddr_in *sai)
  {
  fprintf(stderr, "The call to netaddr needs to be mocked!!\n");
  exit(1);
  }

int tcp_connect_sockaddr(struct sockaddr *sa, size_t sa_size, bool use_log)
  {
  fprintf(stderr, "The call to tcp_connect_sockaddr needs to be mocked!!\n");
  exit(1);
  }

void clear_servers()
  {
  fprintf(stderr, "The call to clear_servers needs to be mocked!!\n");
  exit(1);
  }

int diswul(tcp_chan *chan, unsigned long value)
  {
  fprintf(stderr, "The call to diswul needs to be mocked!!\n");
  exit(1);
  }

int im_compose(tcp_chan *chan, char *jobid, const char *cookie, int command, tm_event_t event, tm_task_id taskid)
  {
  fprintf(stderr, "The call to im_compose needs to be mocked!!\n");
  exit(1);
  }

int socket_avail_bytes_on_descriptor(int socket)
  {
  return(0);
  }

int tcp_chan_has_data(struct tcp_chan *chan)
  {
  return(1);
  }

unsigned int get_random_number()
  {
  return(0);
  }

void shutdown_to_server(int ServerIndex) {}

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

void initialize_network_info() {}

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int type, const char *func_name, const char *msg, int o) {}

void parse_mom_hierarchy(int fds)
  {
  mom_levels lv;
  mh->paths.push_back(lv);
  }

int put_env_var(const char *name, const char *value)
  {
  return(0);
  }

void sort_paths() {}

int pbs_getaddrinfo(
    
  const char       *pNode,
  struct addrinfo  *pHints,
  struct addrinfo **ppAddrInfoOut)

  {
  return(0);
  }

void free_mom_hierarchy(mom_hierarchy_t *nh) {}

bool am_i_mother_superior(const job &pjob)
  {
  return(false);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  exit(1);
  }


#ifdef PENABLE_LINUX_CGROUPS 

int hwloc_topology_init(hwloc_topology_t *)
  {
  return 0;
  }

int hwloc_topology_set_flags(hwloc_topology_t,unsigned long)
  {
  return 0;
  }

int hwloc_topology_load(hwloc_topology_t )
  {
  return 0;
  }

int hwloc_get_type_depth (hwloc_topology_t topology, hwloc_obj_type_t type)
  {
  return 0;
  }

unsigned hwloc_get_nbobjs_by_depth (hwloc_topology_t topology, unsigned depth)
  {
  return 0;
  }

void cleanup_torque_cpuset(void){}

void create_cpuset_reservation_if_needed(job &pjob){}

int init_torque_cpuset(void)
  {
  return 0;
  }

int trq_cg_initialize_hierarchy()
  {
  return(0);
  }

int trq_cg_cleanup_torque_cgroups()
  {
  return(0);
  }


node_internals::node_internals(void){}

numa_node::numa_node(numa_node const&){}

allocation::allocation(allocation const&){}

int Machine::initializeMachine(hwloc_topology_t topology)
  {
  return(0);
  }

Machine::Machine(){}
Machine::~Machine(){}
Socket::Socket(){}
Socket::~Socket(){}
Chip::Chip(){}
Chip::~Chip(){}
Core::Core(){}
Core::~Core(){}
PCI_Device::PCI_Device(){}
PCI_Device::~PCI_Device(){}



void recover_cpuset_reservation(job &pjob)
  {
  return;
  }


#endif

void encode_used(job *pjob, int perm, std::stringstream *list, tlist_head *phead) {}

void encode_flagged_attrs(job *pjob, int perm, std::stringstream *list, tlist_head *phead) {}

char *threadsafe_tokenizer(

  char **str,    /* M */
  const char  *delims) /* I */

  {
  char *current_char;
  char *start;

  if ((str == NULL) ||
      (*str == NULL))
    return(NULL);

  /* save start position */
  start = *str;

  /* return NULL at the end of the string */
  if (*start == '\0')
    return(NULL);

  /* begin at the start */
  current_char = start;

  /* advance to the end of the string or until you find a delimiter */
  while ((*current_char != '\0') &&
         (!strchr(delims, *current_char)))
    current_char++;

  /* advance str */
  if (*current_char != '\0')
    {
    /* not at the end of the string */
    *str = current_char + 1;
    *current_char = '\0';
    }
  else
    {
    /* at the end of the string */
    *str = current_char;
    }

  return(start);
  } /* END threadsafe_tokenizer() */

time_t get_stat_update_interval()
  {
  return ServerStatUpdateInterval;
  }

void empty_received_nodes() {}

void start_request_pool(

    threadpool_t *tp)

  {
  return;
  }

void Machine::displayAsString(std::stringstream &out) const {}

unsigned long setenablemomrestart(

  const char *value)  /* I */

  {
  return(0);
  }

u_long setrcpcmd(

  const char *Value)  /* I */

  {
  return(0);
  }

unsigned long jobstartblocktime(

  const char *value)  /* I */
  
  {
  return(0);
  }

char *tokcpy(

  char *str,
  char *tok)

  {
  return(NULL);
  }

unsigned long setloglevel(

  const char *value)  /* I */

  {
  return(0);
  }

unsigned long setstatusupdatetime(

  const char *value)  /* I */

  {
  return(0);
  }

u_long setjobdirectorysticky(

  const char *value)  /* I */

  {
  int enable;

  if ((enable = setbool(value)) != -1)
    MOMJobDirStickySet = enable;

  return(1);
  }

void reset_config_vars() {}

u_long addclient(

  const char *name)  /* I */

  {
  return(0);
  }

u_long setcudavisibledevices(

  const char *value)  /* I */

  {
  return(0);
  }

unsigned long setdownonerror(

  const char *value)  /* I */

  {
  return(0);
  }

unsigned long setcheckpolltime(

  const char *value)  /* I */

  {
  return(0);
  }

int read_config(

  char *file)  /* I */

  {
  return(0);
  }

void free_pwnam(struct passwd *pwdp, char *buf)
  {}

#ifdef ENABLE_PMIX
#include <pmix_server.h>

pmix_server_module_t psm;
#endif

int check_nvidia_setup()
  {
  return(1);
  }

int init_nvidia_nvml(unsigned int &count)
 {
 return(1);
 }

int shut_nvidia_nvml()
  {
  return(1);
  }

const char *reqvarattr(struct rm_attribute *attrib)
  {
  return(NULL);
  }

int mkdir_wrapper(const char *path, mode_t mode)
  {
  return(0);
  }

int read_all_devices()
  {
  return(PBSE_NONE);
  }

