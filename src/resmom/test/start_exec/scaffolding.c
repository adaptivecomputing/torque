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
#ifdef PENABLE_LINUX26_CPUSETS
#include "pbs_cpuset.h"
#include "node_internals.hpp"
#endif

int  send_ms_called;
int  send_sisters_called;
int  num_contacted;
bool am_ms = false;
bool bad_pwd = false;
bool fail_init_groups = false;
bool fail_site_grp_check = false;
int logged_event;
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

#ifdef NUMA_SUPPORT
nodeboard node_boards[MAX_NODE_BOARDS];
int       num_node_boards = 10;
#endif

#ifdef PENABLE_LINUX26_CPUSETS
node_internals internal_layout;
#endif




int diswcs (struct tcp_chan *chan, const char *value,size_t nchars) 
  { return 0; }
int create_job_cpuset(job * pj) { return 0; }
int DIS_tcp_wflush (struct tcp_chan *chan) { return 0; }
int move_to_job_cpuset(pid_t, job *) { return 0; }
int diswsi(tcp_chan *chan, int i) { return 0; }
int encode_DIS_svrattrl(tcp_chan *chan, svrattrl *s) { return 0; }
int im_compose(tcp_chan *chan, char *arg2, char *a3, int a4, int a5, unsigned int a6) { return 0; }
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
  fprintf(stderr, "The call to find_resc_def needs to be mocked!!\n");
  exit(1);
  }

int mom_checkpoint_job_is_checkpointable(job *pjob)
  {
  fprintf(stderr, "The call to mom_checkpoint_job_is_checkpointable needs to be mocked!!\n");
  exit(1);
  }

struct passwd * getpwnam_ext(char *user_name)
  {
  static int ct = 1;
  static passwd pwd;

  if (ct == 1)
    {
    pwd.pw_dir = strdup("/home/dbeer");
    pwd.pw_gid = 6;
    pwd.pw_name = strdup("dbeer");
    }

  if ((ct++ % 2 == 0) &&
      (bad_pwd == false))
    {
    return(&pwd);
    }

  return(NULL);
  }

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
  fprintf(stderr, "The call to task_save needs to be mocked!!\n");
  exit(1);
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
  fprintf(stderr, "The call to find_resc_entry needs to be mocked!!\n");
  exit(1);
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

int get_hostaddr_hostent_af(int *local_errno, char *hostname, unsigned short *af_family, char **host_addr, int *host_addr_len)
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
  return(0);
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

int pbs_getaddrinfo(const char *hostname, struct addrinfo *bob, struct addrinfo **)
  {
  return -1;
  }

bool am_i_mother_superior(const job &pjob)
  {
  return(am_ms);
  }

int ctnodes(char *epec)
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

void translate_range_string_to_vector(const char *range, std::vector<int> &indices)
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

