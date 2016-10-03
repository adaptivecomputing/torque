#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <time.h> /* time_t */
#include <pwd.h> /* struct passwd, gid_t, uid_t */
#include <signal.h> /* sigset_t */
#include <md5.h> /* MD5_CTX */
#include <sys/socket.h> /* sockaddr_in, sockaddr */
#include <pwd.h> /* gid_t, uid_t */
#include <string> /* std::string */
#include <sys/types.h>
#include <grp.h>

#include "attribute.h" /* attribute_def, pbs_attribute, svrattrl */
#include "resource.h" /* resource_def */
#include "pbs_ifl.h" /* PBS_MAXHOSTNAME, MAXPATHLEN */
#include "list_link.h" /* tlist_head, list_link */
#include "resmon.h" /* TMAX_NSDCOUNT */
#include "log.h" /* LOG_BUF_SIZE */
#include "pbs_job.h" /* job, hnodent, pjobexec_t */
#include "tm_.h" /* tm_task_id, tm_event_t */
#include "mom_mach.h" /* startjob_rtn */
#include "mom_func.h" /* var_table */
#include "pbs_nodes.h"
#include "complete_req.hpp"
#ifdef PENABLE_LINUX26_CPUSETS
#include "pbs_cpuset.h"
#include "node_internals.hpp"
#endif

#include "complete_req.hpp"
#include "req.hpp"
#include "allocation.hpp"

std::string cg_memory_path;
std::string cg_cpuacct_path;
std::string cg_cpuset_path;
std::string cg_devices_path;

#define LDAP_RETRIES 5

// Sensing and control variables
unsigned linux_time = 0;
int  send_ms_called;
int  send_sisters_called;
int  num_contacted;
bool am_ms = false;
bool bad_pwd = false;
bool fail_init_groups = false;
bool fail_site_grp_check = false;
bool addr_fail = false;


bool force_file_overwrite;
int use_nvidia_gpu = TRUE;
int logged_event;
int MOMCudaVisibleDevices;
int exec_with_exec;
int is_login_node = 0;
char *apbasil_protocol = NULL;
char *apbasil_path = NULL;
int lockfds = -1;
char *path_jobs;
int multi_mom = 1;
int svr_resc_size = 0;
int jobstarter_set = 0;
int src_login_interactive = TRUE;
u_long localaddr = 0;
time_t time_now;
char *path_prolog;
char *nodefile_suffix = NULL;
attribute_def job_attr_def[10];
int src_login_batch = TRUE;
int exiting_tasks = 0;
char *path_prologuser;
char *path_aux;
gid_t pbsgroup;
time_t loopcnt;
char PRE_EXEC[1024];
resource_def *svr_resc_def;
sigset_t allsigs;
char mom_host[PBS_MAXHOSTNAME + 1];
uid_t pbsuser;
int spoolasfinalname = 0;
char *path_spool;
char tmpdir_basename[MAXPATHLEN];
unsigned int pbs_rm_port = 0;
char DEFAULT_UMASK[1024];
tlist_head mom_polljobs;
long TJobStartBlockTime = 5;
char *TNoSpoolDirList[TMAX_NSDCOUNT];
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
char *submithost_suffix = NULL;
int num_var_env;
char jobstarter_exe_name[MAXPATHLEN + 1];
int    attempttomakedir = 0;
int EXTPWDRETRY = 3;
char log_buffer[LOG_BUF_SIZE];
char mom_alias[1024];
int ac_read_amount;
int ac_errno;
int job_saved;
int task_saved;

#ifdef NUMA_SUPPORT
nodeboard node_boards[MAX_NODE_BOARDS];
int       num_node_boards = 10;
#endif

#ifdef PENABLE_LINUX26_CPUSETS
node_internals internal_layout;
#endif

void free_pwnam(struct passwd *pwdp, char *buf)
  {}

void free_grname(struct group *grp, char *buf)
  {}

int diswcs (struct tcp_chan *chan, const char *value,size_t nchars) 
  { return 0; }
int create_job_cpuset(job * pj) { return 0; }
int DIS_tcp_wflush (struct tcp_chan *chan) { return 0; }
int move_to_job_cpuset(pid_t, job *) { return 0; }
int diswsi(tcp_chan *chan, int i) { return 0; }
int encode_DIS_svrattrl(tcp_chan *chan, svrattrl *s) { return 0; }
int im_compose(tcp_chan *chan, char *arg2, const char *a3, int a4, int a5, unsigned int a6) { return 0; }
int create_alps_reservation(char *a1, char *a2, char *a3, char *a4, char *a5, long long a6, int a7, int a8, int a9, char **a10,const char *a11, std::string& cray_frequency) { return 0; }
int mom_close_poll(void)
  {
  fprintf(stderr, "The call to mom_close_poll needs to be mocked!!\n");
  exit(1);
  }

void MD5Init(MD5_CTX *mdContext)
  {
  fprintf(stderr, "The call to MD5_CTX needs to be mocked!!\n");
  exit(1);
  }

int rcvwinsize(int sock)
  {
  fprintf(stderr, "The call to rcvwinsize needs to be mocked!!\n");
  exit(1);
  }

int mom_checkpoint_start_restart(job *pjob)
  {
  fprintf(stderr, "The call to mom_checkpoint_start_restart needs to be mocked!!\n");
  exit(1);
  }

char *arst_string(const char *str, pbs_attribute *pattr)
  {
  fprintf(stderr, "The call to arst_string needs to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  job_saved++;
  return(0);
  }

int setwinsize(int pty)
  {
  fprintf(stderr, "The call to setwinsize needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

char * netaddr(struct sockaddr_in *ap)
  {
  fprintf(stderr, "The call to netaddr needs to be mocked!!\n");
  exit(1);
  }

task *pbs_task_create(job *pjob, tm_task_id taskid)
  {
  fprintf(stderr, "The call to pbs_task_create needs to be mocked!!\n");
  exit(1);
  }

char *get_local_script_path(job *pjob, char *base)
  {
  fprintf(stderr, "The call to get_local_script_path needs to be mocked!!\n");
  exit(1);
  }

int read_tcp_reply(struct tcp_chan *chan, int protocol, int version, int command, int *exit_status)
  {
  fprintf(stderr, "The call to read_tcp_reply needs to be mocked!!\n");
  exit(1);
  }

int open_master(char **rtn_name)
  {
  fprintf(stderr, "The call to open_master needs to be mocked!!\n");
  exit(1);
  }

int mom_writer(int s, int ptc)
  {
  fprintf(stderr, "The call to mom_writer needs to be mocked!!\n");
  exit(1);
  }

int mom_do_poll(job *pjob)
  {
  fprintf(stderr, "The call to mom_do_poll needs to be mocked!!\n");
  exit(1);
  }

void set_globid(job *pjob, struct startjob_rtn *sjr)
  {
  fprintf(stderr, "The call to set_globid needs to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  }

char *get_job_envvar(job *pjob, const char *variable)
  {
  fprintf(stderr, "The call to get_job_envvar needs to be mocked!!\n");
  exit(1);
  }

int x11_create_display(int x11_use_localhost, char *display, char *phost, int pport, char *homedir, char *x11authstr)
  {
  fprintf(stderr, "The call to x11_create_display needs to be mocked!!\n");
  exit(1);
  }

eventent *event_alloc(int command, hnodent *pnode, tm_event_t event, tm_task_id taskid)
  {
  fprintf(stderr, "The call to event_alloc needs to be mocked!!\n");
  exit(1);
  }

int mom_checkpoint_job_has_checkpoint(job *pjob)
  {
  fprintf(stderr, "The call to mom_checkpoint_job_has_checkpoint needs to be mocked!!\n");
  exit(1);
  }

int mom_get_sample(void)
  {
  fprintf(stderr, "The call to mom_get_sample needs to be mocked!!\n");
  exit(1);
  }

void MD5Update(MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen)
  {
  fprintf(stderr, "The call to MD5Update needs to be mocked!!\n");
  exit(1);
  }

int run_pelog(int which, char *specpelog, job *pjog, int pe_io_type, int deletejob)
  {
  fprintf(stderr, "The call to run_pelog needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int conn_qsub(char *hostname, long port, char *EMsg)
  {
  fprintf(stderr, "The call to conn_qsub needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr needs to be mocked!!\n");
  exit(1);
  }

void MD5Final(MD5_CTX *mdContext)
  {
  fprintf(stderr, "The call to MD5Final needs to be mocked!!\n");
  exit(1);
  }

int set_job(job *pjob, struct startjob_rtn *sjr)
  {
  fprintf(stderr, "The call to set_job needs to be mocked!!\n");
  exit(1);
  }

int mom_reader(int s, int ptc)
  {
  fprintf(stderr, "The call to mom_reader needs to be mocked!!\n");
  exit(1);
  }

void net_close(int but)
  {
  fprintf(stderr, "The call to net_close needs to be mocked!!\n");
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

void arrayfree(char **array)
  {
  fprintf(stderr, "The call to arrayfree needs to be mocked!!\n");
  exit(1);
  }

int mom_checkpoint_execute_job(job *pjob, char *shell, char *arg[], struct var_table *vtable)
  {
  fprintf(stderr, "The call to mom_checkpoint_execute_job needs to be mocked!!\n");
  exit(1);
  }

void log_ext(int errnum, const char *routine, const char *text, int severity)
  {
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text) 
  {
  logged_event++;
  }

void log_record(int eventtype, int objclass, const char *objname, const char *text) {}

void log_err(int errnum, const char *routine, const char *text) {}

void log_close(int msg)
  {
  fprintf(stderr, "The call to log_close needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  if ((pl.ll_next == NULL) ||
      ((pl.ll_next == &pl) && (pl.ll_struct != NULL)))
    {
    fprintf(stderr, "Assertion failed, bad pointer in link: file \"%s\", line %d\n",
            file,
            line);

    return NULL;
    }

  return(pl.ll_next->ll_struct);
  }  /* END get_next() */

int send_sisters(job *pjob, int com, int using_radix, std::set<int> *sisters_to_contact)
  {
  send_sisters_called++;
  return(num_contacted);
  }

int send_ms(job *pjob,int  com)
  {
  send_ms_called++;
  return(0);
  }



int mom_set_use(job *pjob)
  {
  fprintf(stderr, "The call to mom_set_use needs to be mocked!!\n");
  exit(1);
  }

int mom_set_limits(job *pjob, int set_mode)
  {
  fprintf(stderr, "The call to mom_set_limits needs to be mocked!!\n");
  exit(1);
  }

void free_attrlist(tlist_head *pattrlisthead)
  {
  fprintf(stderr, "The call to free_attrlist needs to be mocked!!\n");
  exit(1);
  }

void attrl_fixlink(tlist_head *phead)
  {
  fprintf(stderr, "The call to attrl_fixlink needs to be mocked!!\n");
  exit(1);
  }

int site_mom_chkuser(job *pjob)
  {
  if (fail_site_grp_check == true)
    return(-1);

  return(0);
  }

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  return(NULL);
  }

int mom_checkpoint_job_is_checkpointable(job *pjob)
  {
  fprintf(stderr, "The call to mom_checkpoint_job_is_checkpointable needs to be mocked!!\n");
  exit(1);
  }

struct passwd *getpwnam_wrapper(

  char       **user_buffer,
  const char *user_name)

  {
  struct passwd *pwent;
  char  *buf;
  long   bufsize;
  struct passwd *result;
  int rc;

  *user_buffer = NULL;
  bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize == -1)
    bufsize = 8196;

  buf = (char *)malloc(bufsize);
  if (buf == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "failed to allocate memory");
    return(NULL);
    }

  pwent = (struct passwd *)calloc(1, sizeof(struct passwd));
  if (pwent == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "could not allocate passwd structure");
    return(NULL);
    }

  rc = getpwnam_r(user_name, pwent, buf, bufsize, &result);
  if (rc)
    {
    sprintf(buf, "getpwnam_r failed: %d", rc);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);
    return (NULL);
    }
  
  *user_buffer = buf;
  return(pwent);
  }


struct group *getgrnam_ext( 

  char **grp_buf,
  char *grp_name) /* I */

  {
  struct group *grp;
  char  *buf;
  long   bufsize;
  struct group *result;
  int rc;

  if (grp_name == NULL)
    return(NULL);

  bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
  if (bufsize == -1)
    bufsize = 8196;

  buf = (char *)malloc(bufsize);
  if (buf == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "failed to allocate memory");
    return(NULL);
    }

  grp = (struct group *)calloc(1, sizeof(struct group));
  if (grp == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "could not allocate passwd structure");
    return(NULL);
    }

  rc = getgrnam_r(grp_name, grp, buf, bufsize, &result);
  if (rc)
    {
    /* See if a number was passed in instead of a name */
    if (isdigit(grp_name[0]))
      {
      rc = getgrgid_r(atoi(grp_name), grp, buf, bufsize, &result);
      if (rc == 0)
        return(grp);
      }
 
    sprintf(buf, "getgrnam_r failed: %d", rc);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);
    return (NULL);
    }

  return(grp);
  } /* END getgrnam_ext() */



struct passwd *getpwnam_ext( 

  char **user_buf,
  char *user_name) /* I */

  {
  static int ct = 1;
  passwd *pwd = (passwd *)calloc(1, sizeof(*pwd));

  pwd->pw_dir = strdup("/home/dbeer");
  pwd->pw_gid = 6;
  pwd->pw_name = strdup("dbeer");

  if ((ct++ % 2 == 0) &&
      (bad_pwd == false))
    {
    return(pwd);
    }

  return(NULL);
  } /* END getpwnam_ext() */


int tcp_connect_sockaddr(struct sockaddr *sa, size_t sa_size, bool use_log)
  {
  fprintf(stderr, "The call to tcp_connect_sockaddr needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *newLink, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_svrattrl(int sock, svrattrl *psattl)
  {
  fprintf(stderr, "The call to encode_DIS_svrattrl needs to be mocked!!\n");
  exit(1);
  }

int site_job_setup(job *pjob)
  {
  fprintf(stderr, "The call to site_job_setup needs to be mocked!!\n");
  exit(1);
  }

int set_mach_vars(job *pjob, struct var_table *vtab)
  {
  fprintf(stderr, "The call to set_mach_vars needs to be mocked!!\n");
  exit(1);
  }

int TMOMJobGetStartInfo(job *pjob, pjobexec_t **TJEP)
  {
  fprintf(stderr, "The call to TMOMJobGetStartInfo needs to be mocked!!\n");
  exit(1);
  }

void set_termcc(int fd)
  {
  fprintf(stderr, "The call to set_termcc needs to be mocked!!\n");
  exit(1);
  }

char *rcvttype(int sock)
  {
  fprintf(stderr, "The call to rcvttype needs to be mocked!!\n");
  exit(1);
  }

int task_save(task *ptask)
  {
  task_saved++;
  return(0);
  }

char * set_shell(job *pjob, struct passwd *pwdp)
  {
  fprintf(stderr, "The call to set_shell needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  static resource mem;
  
  memset(&mem, 0, sizeof(mem));
  mem.rs_value.at_val.at_size.atsv_num = 4;
  mem.rs_value.at_val.at_size.atsv_shift = 30;
  
  return(&mem);
  }

int im_compose(int stream, char *jobid, char *cookie, int command, tm_event_t event, tm_task_id taskid)
  {
  fprintf(stderr, "The call to im_compose needs to be mocked!!\n");
  exit(1);
  }

int diswsi(int stream, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }

void get_chkpt_dir_to_use(job *pjob, char *chkpt_dir, int size)
  {
  fprintf(stderr, "The call to get_chkpt_dir_to_use needs to be mocked!!\n");
  exit(1);
  }

void mom_checkpoint_init_job_periodic_timer(job *pjob)
  {
  fprintf(stderr, "The call to mom_checkpoint_init_job_periodic_timer needs to be mocked!!\n");
  exit(1);
  }

int socket_avail_bytes_on_descriptor(int socket)
  {
  fprintf(stderr, "The call to socket_avail_bytes_on_descriptor needs to be mocked!!\n");
  exit(1);
  }

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
  {
  fprintf(stderr, "The call to timeval_subtract needs to be mocked!!\n");
  exit(1);
  }

int get_hostaddr_hostent_af(int *local_errno, const char *hostname, unsigned short *af_family, char **host_addr, int *host_addr_len)
  {
  fprintf(stderr, "The call to get_hostaddr_hostent_af needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  errno = ac_errno;
  return(ac_read_amount);
  }

proc_stat_t *get_proc_stat(int pid)
  {
  return(NULL);
  }

int setuid_ext(uid_t uid, int set_euid)
  {
  return(0);
  }

int destroy_alps_reservation(char *reservation_id, char *apbasil_path, char *apbasil_protocol, int retries)
  {
  return(0);
  }

int pbs_getaddrinfo(const char *hostname, struct addrinfo *bob, struct addrinfo **ppAddrInfoOut)
  {
  if (addr_fail == true)
    return(-1);
  else
    {
    char buf[MAXLINE];
    gethostname(buf, sizeof(buf));
    getaddrinfo(buf, NULL, NULL, ppAddrInfoOut);

    return(0);
    }
  }

bool am_i_mother_superior(const job &pjob)
  {
  return(am_ms);
  }

int ctnodes(const char *epec)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

std::string get_frequency_request(struct cpu_frequency_value *pfreq)
  {
  std::string ret = "";
  return ret;
  }



#ifdef PENABLE_LINUX26_CPUSETS

node_internals::node_internals(void){}

void node_internals::reserve(int, unsigned long, char const*){}

long long get_memory_requested_in_kb(job&)
  {
  return 0;
  }

int get_cpu_count_requested_on_this_node(job&)
  {
  return 0;
  }

void node_internals::recover_reservation(int cpus, unsigned long memory, char const* jobid) {}

void numa_node::recover_reservation(

  int            num_cpus,
  unsigned long  memory,
  const char    *jobid,
  allocation    &alloc)

  {
  return;
  }

#endif

int translate_range_string_to_vector(const char *range, std::vector<int> &indices)
  {
  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(2);
  indices.push_back(3);
  indices.push_back(4);
  indices.push_back(5);
  indices.push_back(6);
  indices.push_back(7);
  indices.push_back(8);
  indices.push_back(9);

  return(PBSE_NONE);
  }

int initgroups_ext(const char *username, gid_t gr_id)
  {
  if (fail_init_groups == true)
    return(-1);

  return(0);
  }

job *mom_find_job(const char *jobid)
  {
  return(NULL);
  }

void capture_until_close_character(

  char        **start,
  std::string  &storage,
  char          end)

  {
  if ((start == NULL) ||
      (*start == NULL))
    return;

  char *val = *start;
  char *ptr = strchr(val, end);

  // Make sure we found a close quote and this wasn't an empty string
  if ((ptr != NULL) &&
       (ptr != val))
    {
    storage = val;
    storage.erase(ptr - val);
    *start = ptr + 1; // add 1 to move past the character
    }
  } // capture_until_close_character()

void translate_vector_to_range_string(

  std::string            &range_string,
  const std::vector<int> &indices)

  {
  } // END translate_vector_to_range_string()



char * csv_find_string(const char *csv_str, const char *search_str)
  {
  fprintf(stderr, "The call to csv_find_string to be mocked!!\n");
  exit(1);
  }

char *csv_nth(const char *csv_str, int n)
  {
  fprintf(stderr, "The call to csv_nth to be mocked!!\n");
  exit(1);
  }

int csv_length(const char *csv_str)
  {
  fprintf(stderr, "The call to csv_length to be mocked!!\n");
  exit(1);
  }

#ifdef PENABLE_LINUX_CGROUPS
Machine this_node;

int trq_cg_add_process_to_cgroup(std::string &path, const char *suffix, int gpu)
  {
  return(PBSE_NONE);
  }

int init_torque_cgroups()
  {
  return(PBSE_NONE);
  }

int trq_cg_add_devices_to_cgroup(job *pjob)
  {
  return(PBSE_NONE);
  }

int trq_cg_add_process_to_cgroup_accts(pid_t job_pid ) 
  {
  return(PBSE_NONE);
  }

int trq_cg_add_process_to_cgroup(std::string& cgroup_path, pid_t job_pid, pid_t new_pid) 
  {
  return(PBSE_NONE);
  }

int trq_cg_set_resident_memory_limit(pid_t pid, unsigned long memory_limit)
  {
  return(PBSE_NONE);
  }

int trq_cg_set_swap_memory_limit(pid_t pid, unsigned long memory_limit)
  {
  return(PBSE_NONE);
  }

int trq_cg_create_cpuset_cgroup(job*, int)
  {
  return(PBSE_NONE);
  }

int trq_cg_add_process_to_all_cgroups(const char *job_id, int pid)
  {
  return(PBSE_NONE);
  }

int trq_cg_add_process_to_cgroup(const char *job_id, int pid)
  {
  return(PBSE_NONE);
  }

int trq_cg_reserve_cgroup(job *pjob)
  {
  return(PBSE_NONE);
  }

int trq_cg_create_all_cgroups(job *pjob)
  {
  return(PBSE_NONE);
  }

int trq_cg_add_process_to_cgroup_accts(const char *job_id, int pid)
  {
  return(PBSE_NONE);
  }

int trq_cg_set_resident_memory_limit(const char *job_id, unsigned long memory_limit)
  {
  return(PBSE_NONE);
  }

int trq_cg_set_swap_memory_limit(const char *job_id, unsigned long limit)
  {
  return(PBSE_NONE);
  }

int trq_cg_add_process_to_task_cgroup(
  string     &cgroup_path,
  const char *job_id,
  const unsigned int req_index,
  const unsigned int task_index,
  pid_t       new_pid)
  {
  return(PBSE_NONE);
  }

int trq_cg_set_task_swap_memory_limit(
  const char    *job_id,
  unsigned int   req_index,
  unsigned int   task_index,
  unsigned long long  memory_limit)
  {
  return(PBSE_NONE);
  }

int trq_cg_set_swap_memory_limit(
  const char    *job_id,
  unsigned long long  memory_limit)
  {
  return(PBSE_NONE);
  }

int trq_cg_set_task_resident_memory_limit(
  const char    *job_id,
  unsigned int   req_index,
  unsigned int   task_index,
  unsigned long long memory_limit)
  {
  return(PBSE_NONE);
  }

int trq_cg_set_resident_memory_limit(
  const char    *job_id,
  unsigned long long  memory_limit)
  {
  return(PBSE_NONE);
  }

Machine::Machine() {}
Machine::~Machine() {}

hwloc_uint64_t Machine::getTotalMemory() const
  {
  return(this->totalMemory);
  }

PCI_Device::~PCI_Device() {}
Socket::~Socket() {}
Chip::~Chip() {}
Core::~Core() {}

#endif


int is_whitespace(

  char c)

  {
  if ((c == ' ')  ||
      (c == '\n') ||
      (c == '\t') ||
      (c == '\r') ||
      (c == '\f'))
    return(TRUE);
  else
    return(FALSE);
  } /* END is_whitespace */



void move_past_whitespace(

  char **str)

  {
  if ((str == NULL) ||
      (*str == NULL))
    return;

  char *current = *str;

  while (is_whitespace(*current) == TRUE)
    current++;

  *str = current;
  } // END move_past_whitespace()

bool task_hosts_match(const char *one, const char *two)
  {
  return(true);
  }
    
int complete_req::req_count() const
  {
  return(0);
  }

unsigned long long complete_req::get_swap_memory_for_this_host( const std::string &hostname) const
  {
  return(0);
  }

unsigned long long complete_req::get_swap_per_task( unsigned int req_index)
  {
  return(0);
  }

unsigned long long complete_req::get_memory_per_task(unsigned int req_index)
  {
  return(0);
  }

int complete_req::get_req_and_task_index(
  const int rank, 
  unsigned int &req_index, 
  unsigned int &task_index)

  {
  return(0);
  }

unsigned long long complete_req::get_memory_for_this_host(
  const std::string &hostname) const
  {
  return(0);
  }

struct passwd *get_password_entry_by_uid(

  char **user_buf,
  uid_t uid)

  {
  return(NULL);
  }

bool have_incompatible_dash_l_resource(
    
    pbs_attribute *pattr)

  {
  return(false);
  }


unsigned int complete_req::get_num_reqs()
  {
  return(1);
  }

req &complete_req::get_req(int i)
  {
  static req r;

  return(r);
  }

#include "../../src/lib/Libattr/req.cpp"
#include "../../src/lib/Libutils/allocation.cpp"

#ifdef ENABLE_PMIX
void register_jobs_nspace(job *pjob, pjobexec_t *TJE) {}
#endif

int setup_gpus_for_job(job *pjob)
  {return(0);}

