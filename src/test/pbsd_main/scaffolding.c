#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "queue.h" /* all_queues */
#include "pbs_ifl.h" /* PBS_MAXQUEUENAME */
#include "net_connect.h" /* pbs_net_t */
#include "netinet/in.h" /* sockaddr_in */
#include "attribute.h" /* pbs_attribute */
#include "work_task.h" /* work_task, work_type, all_tasks */
#include "pbs_job.h" /* job, all_jobs */
#include "batch_request.h" /* batch_request */
#include "queue.h" /* pbs_queue, all_queues */
#include "server.h" /* server */
#include "pbs_nodes.h" /* pbsnode */
#include "sched_cmds.h" /* SCH_SCHEDULE_NULL */
#include "threadpool.h"
#include "mom_hierarchy_handler.h"
#include "completed_jobs_map.h"
#include "acl_special.hpp"

bool exit_called = false;
pthread_mutex_t *job_log_mutex;
pthread_mutex_t *log_mutex;
all_queues svr_queues;
const char *msg_daemonname = "unset";
const char *msg_startup2 = "Server Ready, pid = %d, loglevel=%d";
char pbs_current_user[PBS_MAXUSER];
const char *msg_svrdown = "Server shutdown completed";
const char *msg_info_server = "Torque Server Version = %s, loglevel = %d";
all_jobs alljobs;
long *log_event_mask = NULL;
const char *msg_startup3 = "%s %s: %s mode and %s exist, \ndo you wish to continue y/(n)?";
pbs_net_t pbs_server_addr;
int listener_command = SCH_SCHEDULE_NULL;
const char *msg_startup1 = "Server %s started, initialization type = %d";
int svr_chngNodesfile = 0; /* 1 signals want nodes file update */
int svr_totnodes = 0; /* total number nodes defined */
char pbs_server_name[1];
pthread_mutex_t *poll_job_task_mutex;
threadpool_t *request_pool;
threadpool_t *async_pool;
threadpool_t *task_pool;
int              max_poll_job_tasks;
char           **ArgV = NULL;
mom_hierarchy_handler hierarchy_handler;

void *start_process_pbs_server_port(void *new_sock)
  {
  return(NULL);
  }

int log_remove_old(char *DirPath, unsigned long ExpireTime)
  {
  fprintf(stderr, "The call to log_remove_old needs to be mocked!!\n");
  exit(1);
  }
 
long job_log_size(void)
  {
  fprintf(stderr, "The call to job_log_size needs to be mocked!!\n");
  exit(1);
  }

char *parse_servername(const char *name, unsigned int *service)
  {
  fprintf(stderr, "The call to parse_servername needs to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save needs to be mocked!!\n");
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

char * netaddr(struct sockaddr_in *ap)
  {
  fprintf(stderr, "The call to netaddr needs to be mocked!!\n");
  exit(1);
  }

int decode_b(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_b needs to be mocked!!\n");
  exit(1);
  }

pbs_net_t get_hostaddr(int *local_errno, const char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr needs to be mocked!!\n");
  exit(1);
  }

int IamRoot()
  {
  fprintf(stderr, "The call to IamRoot needs to be mocked!!\n");
  exit(1);
  }

void acct_cleanup(long days_to_keep)
  {
  fprintf(stderr, "The call to acct_cleanup needs to be mocked!!\n");
  exit(1);
  }

void svr_shutdown(int type)
  {
  fprintf(stderr, "The call to svr_shutdown needs to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type wt, long event, void (*func)(struct work_task *), void *param, int hmm)
  {
  fprintf(stderr, "The call to set_task needs to be mocked!!\n");
  exit(1);
  }

int svr_startjob(job *pjob, struct batch_request **preq, char *FailHost, char *EMsg)
  {
  fprintf(stderr, "The call to svr_startjosvr_startjob needs to be mocked!!\n");
  exit(1);
  }

void job_log_roll(int max_depth)
  {
  fprintf(stderr, "The call to job_log_roll needs to be mocked!!\n");
  exit(1);
  }

void job_log_close(int msg)
  {
  fprintf(stderr, "The call to job_log_close needs to be mocked!!\n");
  exit(1);
  }

int has_task(all_tasks *at)
  {
  fprintf(stderr, "The call to has_task needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

void notify_listeners(void)
  {
  fprintf(stderr, "The call to notify_listeners needs to be mocked!!\n");
  exit(1);
  }

void net_close(int but)
  {
  fprintf(stderr, "The call to net_close needs to be mocked!!\n");
  exit(1);
  }

void shutdown_ack(void)
  {
  fprintf(stderr, "The call to shutdown_ack needs to be mocked!!\n");
  exit(1);
  }

job *next_job(all_jobs *aj, all_jobs_iterator *iter)
  {
  fprintf(stderr, "The call to next_job needs to be mocked!!\n");
  exit(1);
  }

int svr_is_request(tcp_chan *chan, int version, long *args)
  {
  fprintf(stderr, "The call to svr_is_request needs to be mocked!!\n");
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

void check_nodes(struct work_task *ptask)
  {
  fprintf(stderr, "The call to check_nodes needs to be mocked!!\n");
  exit(1);
  }

int process_request(tcp_chan *chan)
  {
  fprintf(stderr, "The call to process_request needs to be mocked!!\n");
  exit(1);
  }

int log_open(char *filename, char *directory)
  {
  fprintf(stderr, "The call to log_open needs to be mocked!!\n");
  exit(1);
  }

int init_network(unsigned int socket, void *(*readfunc)(void *))
  {
  fprintf(stderr, "The call to init_network needs to be mocked!!\n");
  exit(1);
  }

void log_roll(int max_depth)
  {
  fprintf(stderr, "The call to log_roll needs to be mocked!!\n");
  exit(1);
  }
 
work_task *next_task(all_tasks *at, int *iter)
  {
  fprintf(stderr, "The call to next_task needs to be mocked!!\n");
  exit(1);
  }

pbs_queue *next_queue(all_queues *aq, all_queues_iterator *iter)
  {
  fprintf(stderr, "The call to next_queue needs to be mocked!!\n");
  exit(1);
  }

int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg)
  {
  fprintf(stderr, "The call to get_fullhostname needs to be mocked!!\n");
  exit(1);
  }

int dispatch_task(struct work_task *ptask)
  {
  fprintf(stderr, "The call to dispatch_task needs to be mocked!!\n");
  exit(1);
  }

int lock_init()
  {
  fprintf(stderr, "The call to lock_init needs to be mocked!!\n");
  exit(1);
  }

unsigned disrui_peek(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrui_peek needs to be mocked!!\n");
  exit(1);
  }

long log_size(void)
  {
  fprintf(stderr, "The call to log_size needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

void track_save(struct work_task *pwt)
  {
  fprintf(stderr, "The call to track_save needs to be mocked!!\n");
  exit(1);
  }

void *queue_route(void *vp)
  {
  fprintf(stderr, "The call to queue_route needs to be mocked!!\n");
  exit(1);
  }

void acct_close(bool acct_mutex_locked)
  {
  return;
  }

int svr_save(struct server *ps, int mode)
  {
  fprintf(stderr, "The call to svr_save needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_settimeout(long timeout)
  {
  fprintf(stderr, "The call to DIS_tcp_settimeout needs to be mocked!!\n");
  exit(1);
  }

int update_nodes_file(struct pbsnode *held)
  {
  fprintf(stderr, "The call to update_nodes_file needs to be mocked!!\n");
  exit(1);
  }

int schedule_jobs(void)
  {
  fprintf(stderr, "The call to schedule_jobs needs to be mocked!!\n");
  exit(1);
  }

int pbsd_init(int type)
  {
  fprintf(stderr, "The call to pbsd_init needs to be mocked!!\n");
  exit(1);
  }

int disrsi(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrsi needs to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue needs to be mocked!!\n");
  exit(1);
  }

void destroy_request_pool(threadpool_t *tp)
  {
  }


int unlock_node(struct pbsnode *pnode, const char *id, char *msg, int log_level)
  {
  fprintf(stderr, "The call to unlock_node needs to be mocked!!\n");
  exit(1);
  }

struct pbsnode *find_nodebyname(const char *name)
  {
  fprintf(stderr, "The call to find_nodebyname needs to be mocked!\n");
  exit(1);
  }


int send_hierarchy(char *nodename, unsigned short port)
  {
  fprintf(stderr, "The call to send_hierarchy needs to be mocked!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

void *send_hierarchy_threadtask(void *vp)
  {
  return(NULL);
  }

void log_init(const char *suffix, const char *hostname) {}

int enqueue_threadpool_request(void *(*func)(void *), void *arg, threadpool_t *tp)
  {
  return(0);
  }

int set_svr_attr(int index, void *val)
  {
  return(0);
  }

int recov_svr_attr(int type)
  {
  return(0);
  }

int get_svr_attr_str(int index, char **str)
  {
  return(0);
  }

int get_svr_attr_arst(int index, struct array_strings **arst)
  {
  return(0);
  }

int acl_check_my_array_string(struct array_strings *pas, char *name, int type)
  {
  return(0);
  }

void get_port_from_server_name_file(unsigned int *server_name_file_port)
  {
  fprintf(stderr, "The call to get_port_from_server_name_file needs to be mocked!\n");
  exit(1);
  }

unsigned int get_random_number()
  {
  fprintf(stderr, "The call to get_random_number needs to be mocked!\n");
  exit(1);
  }

int check_network_port(

  unsigned int  port)

  {
  return(0);
  }

void set_localhost_name(char *localhost_name, size_t len)
  {
  }

int start_listener_addrinfo(

  char   *host_name,
  int     server_port,
  void *(*process_meth)(void *))
  {
  return(0);
  }

void netcounter_incr(void) {}

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

void scheduler_close() {}

int initialize_ruserok_mutex()
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int run_change_logs;

void change_logs() {}

void *inspect_exiting_jobs(void *vp) 
  {
  return(NULL);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

int save_args(int argc, char **argv)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int eventtype, const char *func_name, const char *msg, int level) {}

int dispatch_timed_task(

  work_task *ptask)
  
  {
  return(0);
  }

work_task *pop_timed_task(

  time_t  time_now)

  {
  return(NULL);
  }

void *remove_extra_recycle_jobs(void *)
  {
  return(NULL);
  }
  
void mom_hierarchy_handler::checkAndSendHierarchy(bool b)
  {
  }

void clear_all_alps_reservations() {}

completed_jobs_map_class::completed_jobs_map_class() {}
completed_jobs_map_class::~completed_jobs_map_class() {}
void *remove_completed_jobs(void *vp) {return(NULL);}

acl_special::acl_special() {}
