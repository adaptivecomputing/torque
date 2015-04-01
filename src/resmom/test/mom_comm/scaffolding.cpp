#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h> /* sockaddr_in */
#include <errno.h>
#include <set>

#include "mom_server.h" /* mom_server */
#include "resmon.h" /* PBS_MAXSERVER */
#include "attribute.h" /* attribute_def, pbs_attribute */
#include "net_connect.h" /* connection, PBS_NET_MAX_CONNECTIONS */
#include "log.h" /* LOG_BUF_SIZE */
#include "list_link.h" /* tlist_head */
#include "resource.h" /* resource_def */
#include "u_tree.h" /* AvlTree */
#include "pbs_job.h" /* job */
#include "mom_func.h" /* radix_buf */
#include "dis.h"

int create_job_cpuset(job *pj) { return 0; }
int use_cpusets(job *pj) { return 0; }
const char *path_jobs = "some path to nowhere."; /* mom_main.c */
int multi_mom = 1; /* mom_main.c */
int svr_resc_size = 0; /* resc_def_all.c */
u_long localaddr = 0; /* mom_main.c */
time_t time_now = 0; /* mom_main.c */
mom_server mom_servers[PBS_MAXSERVER]; /* mom_server.c */
attribute_def job_attr_def[10]; /* src/server/job_attr_def.c */
int exiting_tasks = 0; /* mom_main.c */
const char *dis_emsg[10]; /* dis.h */
char *path_prologuserp; /* mom_main.c */
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS]; /* net_server.c */
tlist_head svr_newjobs; /* mom_main.c */
resource_def *svr_resc_def; /* resc_def_all.c */
int port_care = TRUE; /* mom_main.c */
unsigned int pbs_rm_port = 0; /* mom_main.c */
AvlTree okclients; /* mom_main.c */
tlist_head mom_polljobs; /* mom_main.c */
char *path_prologp; /* mom_main.c */
tlist_head svr_alljobs; /* mom_main.c */
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */ /* mom_main.c/pbsd_main.c */
int maxupdatesbeforesending = 0;
int    ServerStatUpdateInterval = DEFAULT_SERVER_STAT_UPDATES;
time_t          LastServerUpdateTime = 0;  /* NOTE: all servers updated together */
bool            ForceServerUpdate = false;
char log_buffer[LOG_BUF_SIZE];
int log_event_counter;
bool exit_called = false;
bool ms_val = true;

#undef disrus
unsigned short disrus(tcp_chan *c, int *retval)
  {
  *retval = DIS_SUCCESS;
  return(0);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "This mock job_save always returns 0!!\n");
  return(0);
  }

void mom_job_purge(job *pjob) {}

int decode_DIS_svrattrl(struct tcp_chan *chan, tlist_head *phead)
  {
  fprintf(stderr, "This mock decode_DIS_svrattrl always returns 0!!\n");
  return(0);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  return(0);
  }

int TMakeTmpDir(job *pjob, char *tmpdir)
  {
  fprintf(stderr, "The call to TMakeTmpDir needs to be mocked!!\n");
  return(0);
  }

unsigned long gettime(resource *pres)
  {
  fprintf(stderr, "The call to gettime needs to be mocked!!\n");
  return(0);
  }

void exec_bail(job *pjob, int code, std::set<int> *contacted_sisters) {}

int AVL_list(AvlTree tree, char **Buf, long *current_len, long *max_len)
  {
  fprintf(stderr, "The call to AVL_list needs to be mocked!!\n");
  return(0);
  }

int exec_job_on_ms(job *pjob)
  {
  fprintf(stderr, "The call to exec_job_on_ms needs to be mocked!!\n");
  return(0);
  }

struct passwd *check_pwd_return;
struct passwd *check_pwd(job *pjob)
  {
  if(check_pwd_return == NULL)
    {
    check_pwd_return = (struct passwd *)calloc(1,sizeof(*check_pwd_return));
    }
  return(check_pwd_return);
  }

int mom_do_poll(job *pjob)
  {
  return(0);
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  return;
  }

char *get_job_envvar(job *pjob, const char *variable)
  {
  fprintf(stderr, "The call to get_job_envvar needs to be mocked!!\n");
  return(0);
  }

int open_tcp_stream_to_sisters(job *pjob, int com, tm_event_t parent_event, int mom_radix, hnodent *hosts, struct radix_buf **sister_list, tlist_head *phead, int flag)
  {
  fprintf(stderr, "The call to open_tcp_stream_to_sisters needs to be mocked!!\n");
  return(0);
  }

int open_demux(u_long addr, int port)
  {
  fprintf(stderr, "The call to open_demux needs to be mocked!!\n");
  return(0);
  }

int mom_get_sample()
  {
  fprintf(stderr, "The call to mom_get_sample needs to be mocked!!\n");
  return(0);
  }

int run_pelog(int which, char *specpelog, job *pjog, int pe_io_type, int deletejob)
  {
  return(0);
  }

#undef disrul
unsigned long disrul(struct tcp_chan * chan, int *retval)
  {
  static int rc = 0;
  return(rc++);
  }

const char * getuname(void )
  {
  fprintf(stderr, "The call to getuname needs to be mocked!!\n");
  return(0);
  }

size_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  return(0);
  }

struct tcp_chan *DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "This mock DIS_tcp_setup always returns NULL!!\n");
  return(NULL);
  }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr needs to be mocked!!\n");
  return(0);
  }

#undef disrui
unsigned disrui(struct tcp_chan *chan, int *retval)
  {
  *retval = DIS_SUCCESS;
  return(0);
  }

int kill_task(struct task *task, int sig, int pg)
  {
  fprintf(stderr, "The call to kill_task needs to be mocked!!\n");
  return(0);
  }

int DIS_tcp_wflush(struct tcp_chan * chan)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  return(0);
  }

int diswcs(struct tcp_chan * chan, const char *value, size_t nchars)
  {
  fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
  return(0);
  }

unsigned long getsize(resource *pres)
  {
  fprintf(stderr, "The call to getsize needs to be mocked!!\n");
  return(0);
  }

void *get_next(list_link pl, char *file, int line)
  {
  if ((pl.ll_next == NULL) ||
      ((pl.ll_next == &pl) && (pl.ll_struct != NULL)))
    {
    return NULL;
    }
  return(pl.ll_next->ll_struct);
  }

int add_host_to_sister_list(char *hostname, unsigned short port, struct radix_buf *rb)
  {
  fprintf(stderr, "The call to add_host_to_sister_list needs to be mocked!!\n");
  return(0);
  }

char *disrcs(struct tcp_chan * chan, size_t *nchars, int *retval)
  {
  fprintf(stderr, "The call to disrcs needs to be mocked!!\n");
  return(0);
  }

int start_process(task *tsk, char **argv, char **envp)
  {
  fprintf(stderr, "The call to start_process needs to be mocked!!\n");
  return(0);
  }

#undef diswui
int diswui(struct tcp_chan * chan, unsigned value)
  {
  fprintf(stderr, "The call to diswui needs to be mocked!!\n");
  return(0);
  }

void free_sisterlist(struct radix_buf **list, int radix)
  {
  fprintf(stderr, "The call to free_sisterlist needs to be mocked!!\n");
  return;
  }

int write_tcp_reply(struct tcp_chan *chan, int protocol, int version, int command, int exit_code)
  {
  fprintf(stderr, "This mock write_tcp_reply always returns 0!!\n");
  return(0);
  }

int mom_set_use(job *pjob)
  {
  fprintf(stderr, "The call to mom_set_use needs to be mocked!!\n");
  return(0);
  }

void free_attrlist(tlist_head *pattrlisthead) {}

void attrl_fixlink(tlist_head *phead)
  {
  fprintf(stderr, "The call to attrl_fixlink needs to be mocked!!\n");
  return;
  }

struct resource_def *find_resc_def_return = NULL;
resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  if (find_resc_def_return == NULL)
    {
    find_resc_def_return = (struct resource_def *)calloc(1, sizeof(*find_resc_def_return));
    }
  if (find_resc_def_return->rs_name != NULL)
    {
    free((void*)find_resc_def_return->rs_name);
    }
  find_resc_def_return->rs_name = strdup(name);
  return(find_resc_def_return);
  }

struct radix_buf **allocate_sister_list(int radix)
  {
  fprintf(stderr, "The call to allocate_sister_list needs to be mocked!!\n");
  return(0);
  }

int tcp_connect_sockaddr(struct sockaddr *sa, size_t sa_size, bool use_log)
  {
  fprintf(stderr, "This mock tcp_connect_sockaddr always returns 0!!\n");
  return(10);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj) {}

void sister_job_nodes(job *pjob, char *radix_hosts, char *radix_ports )
  {
  fprintf(stderr, "The call to sister_job_nodes needs to be mocked!!\n");
  return;
  }

int TTmpDirName(job *pjob, char *tmpdir, int tmdir_size)
  {
  return(0);
  }

void job_nodes(job &pjob) {}

void close_conn(int sd, int has_mutex) {}

int diswul(struct tcp_chan * chan, unsigned long value)
  {
  fprintf(stderr, "The call to diswul needs to be mocked!!\n");
  return(0);
  }

int rpp_eom(int index)
  {
  fprintf(stderr, "The call to rpp_eom needs to be mocked!!\n");
  return(0);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "This mock find_resc_entry always returns NULL!!\n");
  return(NULL);
  }

job *mock_mom_find_job_return = NULL;
job *mom_find_job(const char *jobid)
  {
  if (strcmp("jobid", jobid) != 0)
    {
    return(NULL);
    }
  if (mock_mom_find_job_return == NULL)
    {
      mock_mom_find_job_return = (job *)calloc(1, sizeof(*mock_mom_find_job_return));
      mock_mom_find_job_return->ji_wattr[JOB_ATR_Cookie].at_flags |= ATR_VFLAG_SET;
      mock_mom_find_job_return->ji_wattr[JOB_ATR_Cookie].at_val.at_str = strdup("cookie");
    }
  return(mock_mom_find_job_return);
  }

#undef diswsi
int diswsi(tcp_chan *c, int value)
  {
  fprintf(stderr, "This mock diswsi always returns 0!!\n");
  return(0);
  }

job *mock_job_alloc_return = NULL;
job *job_alloc(void )
  {
  if (mock_job_alloc_return == NULL)
    {
    mock_job_alloc_return = (job *)calloc(1, sizeof(*mock_job_alloc_return));
    }
  return(mock_job_alloc_return);
  }

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
  {
  fprintf(stderr, "The call to timeval_subtract needs to be mocked!!\n");
  return(0);
  }

int allocate_demux_sockets(job *pjob, int flag)
  {
  fprintf(stderr, "The call to allocate_demux_sockets needs to be mocked!!\n");
  return(0);
  }

int kill_job(job *pjob, int sig, const char *killer_id_name, const char *why_killed_reason)
  {
  fprintf(stderr, "This mock kill_job always returns 0!!\n");
  return(0);
  }

int add_to_resend_things(resend_momcomm *mc)
  {
  fprintf(stderr, "The call to add_to_resend_things needs to be mocked!!\n");
  return(0);
  }

im_compose_info *create_compose_reply_info(char *jobid, char *cookie, hnodent *np, int command, tm_event_t event, tm_task_id taskid)
  {
  fprintf(stderr, "The call to create_compose_reply_info needs to be mocked!!\n");
  return(0);
  }

int get_hostaddr_hostent_af(int *local_errno, char *hostname, unsigned short *af_family, char **host_addr, int *host_addr_len)
  {
  fprintf(stderr, "The call to get_hostaddr_hostent_af needs to be mocked!!\n");
  return(0);
  }

pid_t fork_me(int conn) 
  {
  return(0);
  }

void DIS_tcp_close(struct tcp_chan *chan) {}

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

int getpeername(int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __len)  __THROW
  {
  memset(__addr, 0, sizeof(*__addr));
  return(0);
  }

char * netaddr_long(long ap, char *out)
  {
  u_long  ipadd;

  ipadd = ap;

  sprintf(out, "%ld.%ld.%ld.%ld",
           (ipadd & 0xff000000) >> 24,
           (ipadd & 0x00ff0000) >> 16,
           (ipadd & 0x0000ff00) >> 8,
           (ipadd & 0x000000ff));

  return out;
  }

char * netaddr(struct sockaddr_in *ap)
  {
  static char out[80];
  char tmp[80];

  if (ap == NULL)
    return (char *)"unknown";

  netaddr_long( ntohl(ap->sin_addr.s_addr), tmp);

  sprintf(out, "%s:%d", tmp, ntohs(ap->sin_port));

  return out;
  }

int AVL_is_in_tree_no_port_compare(u_long key, uint16_t port, AvlTree tree)
  {
  return(1);
  }

int disrst_return_index = 0;
#define disrst_array_size 10
char *disrst_array[disrst_array_size];
char *disrst(struct tcp_chan *chan, int *retval)
  {
  if (disrst_return_index >= disrst_array_size)
    {
    *retval = -1;
    return(NULL);
    }
    *retval = DIS_SUCCESS;
    return(disrst_array[disrst_return_index++]);
  }

int disrsi_return_index = 0;
const int disrsi_array_size = 10;
#define disrsi_array_size 10
int disrsi_array[disrsi_array_size];
#undef disrsi
int disrsi(struct tcp_chan *chan, int *retval)
  {
  if (disrsi_return_index >= disrsi_array_size)
    {
    *retval = -1;
    return(-1);
    }
  *retval = DIS_SUCCESS;
  return disrsi_array[disrsi_return_index++];
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) 
  {
  log_event_counter++;
  }
void log_ext(int type, const char *func_name, const char *msg, int o) {}

bool am_i_mother_superior(const job &pjob)
  {
  bool mother_superior = ((pjob.ji_nodeid == 0) && ((pjob.ji_qs.ji_svrflags & JOB_SVFLG_HERE) != 0));
    
  return(mother_superior);
  }

void create_cpuset_reservation_if_needed(job &pjob){}

