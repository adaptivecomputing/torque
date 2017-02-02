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



job_array *get_jobs_array(svr_job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_array to be mocked!!\n");
  exit(1);
  }

void account_record(int acctype, svr_job *pjob, const char *text)
  {
  fprintf(stderr, "The call to account_record to be mocked!!\n");
  exit(1);
  }

char *parse_servername(const char *name, unsigned int *service)
  {
  fprintf(stderr, "The call to parse_servername to be mocked!!\n");
  exit(1);
  }

int svr_job_save(svr_job *pjob)
  {
  fprintf(stderr, "The call to job_save to be mocked!!\n");
  exit(1);
  }

void svr_mailowner(svr_job *pjob, int mailpoint, int force, const char *text)
  {
  fprintf(stderr, "The call to svr_mailowner to be mocked!!\n");
  exit(1);
  }

char *find_ts_node(void)
  {
  fprintf(stderr, "The call to find_ts_node to be mocked!!\n");
  exit(1);
  }

void stat_mom_job(const char *job_id)
  {
  }

pbs_net_t get_hostaddr(int *local_errno, const char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr to be mocked!!\n");
  exit(1);
  }

struct batch_request *cpy_checkpoint(struct batch_request *preq, svr_job *pjob, enum job_atr ati, int direction)
  {
  fprintf(stderr, "The call to cpy_checkpoint to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

void free_nodes(svr_job *pjob, const char *spec)
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

int depend_on_exec(svr_job *pjob)
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

void set_resc_assigned(svr_job *pjob, enum batch_op op)
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

 int set_nodes(svr_job *pjob, const char *spec, int procs, std::string &node_list, std::string &portlist, char *FailHost, char *EMsg)
  {
  fprintf(stderr, "The call to set_nodes to be mocked!!\n");
  exit(1);
  }

struct batch_request *cpy_stage(struct batch_request *preq, svr_job *pjob, enum job_atr ati, int direction)
  {
  fprintf(stderr, "The call to cpy_stage to be mocked!!\n");
  exit(1);
  }

void account_jobstr(svr_job *pjob)
  {
  fprintf(stderr, "The call to account_jobstr to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(svr_job *pjob, int newstate, int newsubstate, int  has_queue_mutex)
  {
  pjob->set_state(newstate);
  pjob->set_substate(newsubstate);

  return(0);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry to be mocked!!\n");
  exit(1);
  }

svr_job *svr_find_job(const char *jobid, int get_subjob)
  {
  return (svr_job *) atol(jobid);
  }

void DIS_tcp_settimeout(long timeout)
  {
  }

int send_job_work(char *job_id, const char *node_name, int type, int *my_err, struct batch_request *preq)
  {
  //returning failure
  return -1;
  }

void svr_evaljobstate(svr_job &pjob, int &newstate, int &newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate to be mocked!!\n");
  exit(1);
  }

svr_job *chk_job_request(char *jobid, struct batch_request *preq)
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

pbs_queue *get_jobs_queue(svr_job **pjob)
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

  const char *br_id)

  {
  return(NULL);
  }

int get_batch_request_id(

  batch_request *preq)

  {
  return(0);
  }

int unlock_ji_mutex(svr_job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int split_job(svr_job *pjob)
  {
  return(0);
  }

int kill_job_on_mom(const char *job_id, struct pbsnode *pnode)
  {
  return(0);
  }

int lock_ji_mutex(svr_job *pjob, const char *id, const char *msg, int logging)
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

int relay_to_mom(svr_job **pjob_ptr, batch_request   *request, void (*func)(struct work_task *))
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

job::job() 
  {
  memset(&this->ji_qs, 0, sizeof(this->ji_qs));
  memset(&this->ji_wattr, 0, sizeof(this->ji_wattr));
  }
job::~job() {}

svr_job::svr_job() 
  {
  this->ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(this->ji_mutex, NULL);
  }
svr_job::~svr_job() {}

void job_array::update_array_values(

  int                   old_state, /* I */
  enum ArrayEventsEnum  event,     /* I */
  const char           *job_id,
  int                   job_exit_status)

  {
  }

batch_request::~batch_request()

  {
  }

batch_request::batch_request(const batch_request &other)

  {
  }

batch_request::batch_request()
  {
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

tlist_head job::get_list_attr(int index)
  {
  return(this->ji_wattr[index].at_val.at_list);
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
