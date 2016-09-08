#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h>


#include "attribute.h" /* attribute_def, batch_op */
#include "resource.h" /* resource_def */
#include "server.h" /* server */
#include "pbs_ifl.h" /* PBS_MAXHOSTNAME */
#include "array.h" /* job_array */
#include "pbs_job.h" /* job */
#include "net_connect.h" /* pbs_net_t */
#include "batch_request.h" /* batch_request */
#include "node_func.h" /* node_info */
#include "pbs_nodes.h" /* pbsnode */
#include "work_task.h" /* work_task, work_type */
#include "list_link.h" /* list_link  */
#include "queue.h"
#include "threadpool.h"
#include "complete_req.hpp"

pthread_mutex_t *scheduler_sock_jobct_mutex;
const char *PJobSubState[10];
const char *PJobState[] =
  {
  "TRANSIT",
  "QUEUED",
  "HELD",
  "WAITING",
  "RUNNING",
  "EXITING",
  "COMPLETE",
  NULL
  };
int svr_tsnodes = 0;
int svr_resc_size = 0;
attribute_def job_attr_def[10];
const char *msg_jobrun = "Job Run";
resource_def *svr_resc_def;
char mom_host[PBS_MAXHOSTNAME + 1];
int scheduler_jobct = 0;
struct server server;
int scheduler_sock = -1;
pbs_net_t pbs_mom_addr;
const char *msg_manager = "%s at request of %s@%s";
int LOGLEVEL = 10; /* force logging code to be exercised as tests run */
int svr_totnodes = 0;
threadpool_t *request_pool;
char scaff_buffer[1024];
threadpool_t *async_pool;
bool cray_enabled;



job_array *get_jobs_array(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_array to be mocked!!\n");
  exit(1);
  }

void account_record(int acctype, job *pjob, const char *text)
  {
  fprintf(stderr, "The call to account_record to be mocked!!\n");
  exit(1);
  }

char *parse_servername(const char *name, unsigned int *service)
  {
  fprintf(stderr, "The call to parse_servername to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save to be mocked!!\n");
  exit(1);
  }

void svr_mailowner(job *pjob, int mailpoint, int force, const char *text)
  {
  fprintf(stderr, "The call to svr_mailowner to be mocked!!\n");
  exit(1);
  }

char *find_ts_node(void)
  {
  fprintf(stderr, "The call to find_ts_node to be mocked!!\n");
  exit(1);
  }

void stat_mom_job(char *job_id)
  {
  fprintf(stderr, "The call to stat_mom_job to be mocked!!\n");
  exit(1);
  }

pbs_net_t get_hostaddr(int *local_errno, const char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr to be mocked!!\n");
  exit(1);
  }

struct batch_request *cpy_checkpoint(struct batch_request *preq, job *pjob, enum job_atr ati, int direction)
  {
  fprintf(stderr, "The call to cpy_checkpoint to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

void free_nodes(job *pjob, const char *spec)
  {
  fprintf(stderr, "The call to free_nodes to be mocked!!\n");
  exit(1);
  }

char *PAddrToString(pbs_net_t *Addr)
  {
  fprintf(stderr, "The call to PAddrToString to be mocked!!\n");
  exit(1);
  }

struct pbsnode *find_nodebyname(const char *nodename)
  {
  static struct pbsnode pnode;

  if (!strcmp("bob", nodename))
    return(&pnode);
  else
    return(NULL);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(work_task *), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task to be mocked!!\n");
  exit(1);
  }

int depend_on_exec(job *pjob)
  {
  fprintf(stderr, "The call to depend_on_exec to be mocked!!\n");
  exit(1);
  }

int unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  }

int is_ts_node(char *nodestr)
  {
  fprintf(stderr, "The call to is_ts_node to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

void stream_eof(int stream, u_long addr, uint16_t port, int ret)
  {
  fprintf(stderr, "The call to stream_eof to be mocked!!\n");
  exit(1);
  }

void poll_job_task(struct work_task *ptask)
  {
  fprintf(stderr, "The call to poll_job_task to be mocked!!\n");
  exit(1);
  }

void set_resc_assigned(job *pjob, enum batch_op op)
  {
  fprintf(stderr, "The call to set_resc_assigned to be mocked!!\n");
  exit(1);
  }

int job_set_wait(pbs_attribute *pattr, void *pjob, int mode)
  {
  fprintf(stderr, "The call to job_set_wait to be mocked!!\n");
  exit(1);
  }

 resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req to be mocked!!\n");
  exit(1);
  }

 int set_nodes(job *pjob, char *spec, int procs, char **rtnlist, char **rtnportlist, char *FailHost, char *EMsg)
  {
  fprintf(stderr, "The call to set_nodes to be mocked!!\n");
  exit(1);
  }

struct batch_request *cpy_stage(struct batch_request *preq, job *pjob, enum job_atr ati, int direction)
  {
  fprintf(stderr, "The call to cpy_stage to be mocked!!\n");
  exit(1);
  }

void account_jobstr(job *pjob)
  {
  fprintf(stderr, "The call to account_jobstr to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mutex)
  {
  pjob->ji_qs.ji_state    = newstate;
  pjob->ji_qs.ji_substate = newsubstate;

  return(0);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  return (job *) atol(jobid);
  }

void DIS_tcp_settimeout(long timeout)
  {
  }

int send_job_work(char *job_id, char *node_name, int type, int *my_err, struct batch_request *preq)
  {
  //returning failure
  return -1;
  }

void svr_evaljobstate(job &pjob, int &newstate, int &newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate to be mocked!!\n");
  exit(1);
  }

job *chk_job_request(char *jobid, struct batch_request *preq)
  {
  fprintf(stderr, "The call to chk_job_request to be mocked!!\n");
  exit(1);
  }

int insert_task(all_tasks *at, work_task *wt)
  {
  fprintf(stderr, "The call to insert_task to be mocked!!\n");
  exit(1);
  }

char *threadsafe_tokenizer(char **str, const char *delims)
  {
  fprintf(stderr, "The call to threadsafe_tokenizer needs to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  return(0);
  }

pbs_queue *get_jobs_queue(job **pjob)
  {
  return((*pjob)->ji_qhdr);
  }

void free_br(struct batch_request *b) {}

int get_svr_attr_str(int index, char **str)
  {
  return(0);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *id, const char *msg, int logging)
  {
  return(0);
  }

batch_request *get_remove_batch_request(

  char *br_id)

  {
  return(NULL);
  }

int get_batch_request_id(

  batch_request *preq)

  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int split_job(job *pjob)
  {
  return(0);
  }

int kill_job_on_mom(const char *job_id, struct pbsnode *pnode)
  {
  return(0);
  }

int lock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

batch_request *duplicate_request(struct batch_request *preq)
  {
  return(NULL);
  }

int unlock_ai_mutex(job_array *pa, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int enqueue_threadpool_request(void *(*func)(void *), void *arg, threadpool_t *tp)

  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}

void log_event(int eventtype, int objclass, const char *objname, const char *text)
  {
  snprintf(scaff_buffer, sizeof(scaff_buffer), "%s", text);
  }

int relay_to_mom(job **pjob_ptr, batch_request   *request, void (*func)(struct work_task *))
  {
  return(0);
  }

batch_request *duplicate_request(batch_request *preq, int job_index)
  {
  return NULL;
  }

int pbs_getaddrinfo(const char *hostname, struct addrinfo *in, struct addrinfo **out)
  {
  return(0);
  }

bool log_available(int eventtype)
  {
  return true;
  }

pbsnode::pbsnode() {}
pbsnode::~pbsnode() {}

int pbsnode::unlock_node(const char *msg, const char *id, int level)
  {
  return(0);
  }

void complete_req::set_hostlists(const char *jobid, const char *list) {}

job::job() {}
job::~job() {}

void job_array::update_array_values(

  int                   old_state, /* I */
  enum ArrayEventsEnum  event,     /* I */
  const char           *job_id,
  int                   job_exit_status)

  {
  }

