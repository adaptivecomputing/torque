#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <ctype.h>

#include "attribute.h" /* attribute_def, pbs_attribute, svrattrl */
#include "pbs_job.h" /* all_jobs, job */
#include "resource.h" /* resource_def */
#include "pbs_ifl.h" /* PBS_MAXSERVERNAME */
#include "server.h" /* server */
#include "log.h" /* LOG_BUF_SIZE */
#include "queue.h" /* pbs_queue */
#include "batch_request.h" /* batch_request */
#include "work_task.h" /* work_task, work_type */
#include "user_info.h"
#include "mutex_mgr.hpp"
#include "threadpool.h"
#include "id_map.hpp"
#include "pbs_nodes.h"

bool cray_enabled;
bool exit_called = false;
const char *PJobSubState[10];
char *path_jobs;
const char *msg_script_write = "Unable to write script file";
int svr_resc_size = 0;
attribute_def job_attr_def[10];
const char *msg_script_open = "Unable to open script file";
const char *msg_jobnew = "Job Queued at request of %s@%s, owner = %s, job name = %s, queue = %s";
all_jobs newjobs;
char server_name[PBS_MAXSERVERNAME + 1];
const char *pbs_o_host = "PBS_O_HOST";
resource_def *svr_resc_def;
int queue_rank = 0;
char *path_spool;
struct server server;
const char *msg_daemonname = "unset";
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
user_info_holder users;
threadpool_t *task_pool;
char str_to_set[1024];
long long_to_set = -1;
bool default_queue = false;
bool mem_fail = false;
bool set_ji_substate = false;

time_t time_now;

int setup_array_struct(svr_job *pjob)
  {
  fprintf(stderr, "The call to setup_array_struct to be mocked!!\n");
  exit(1);
  }

pbs_queue *find_queuebyname(const char *quename)
  {
  if (!strcmp(quename, "batch"))
    return((pbs_queue *)calloc(1, sizeof(pbs_queue)));

  return(NULL);
  }

int svr_job_save(svr_job *pjob)
  {
  return(1);
  }

int svr_job_purge(svr_job *pjob, int leaveSpoolFiles)
  {
  return(0);
  }

void set_chkpt_deflt(svr_job *pjob, pbs_queue *pque)
  {
  fprintf(stderr, "The call to set_chkpt_deflt to be mocked!!\n");
  exit(1);
  }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_jobs_queue(svr_job **pjob)
  {
  pbs_queue *pque = (pbs_queue *)calloc(sizeof(pbs_queue), 1);

  // create space for mutex
  pque->qu_mutex = (pthread_mutex_t *)calloc(sizeof(pthread_mutex_t), 1);
  pthread_mutex_init(pque->qu_mutex, NULL);

  strcpy(pque->qu_qs.qu_name, "foo");

  return(pque);
  }

void reply_ack(struct batch_request *preq)
  {
  }

int svr_authorize_jobreq(struct batch_request *preq, svr_job *pjob)
  {
  return 0;
  }

char *get_variable(svr_job *pjob, const char *variable)
  {
  fprintf(stderr, "The call to get_variable to be mocked!!\n");
  exit(1);
  }

void job_clone_wt(void *cloned_id)
  {
  fprintf(stderr, "The call to job_clone_wt to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task to be mocked!!\n");
  exit(1);
  }

char * csv_find_value(const char *csv_str, const char *search_str)
  {
  fprintf(stderr, "The call to csv_find_value to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket to be mocked!!\n");
  exit(1);
  }

int insert_job(all_jobs *aj, svr_job *pjob)
  {
  fprintf(stderr, "The call to insert_job to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr to be mocked!!\n");
  exit(1);
  }

int decode_resc(struct pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_resc to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  }

svr_job *next_job(all_jobs *aj, all_jobs_iterator *iter)
  {
  return(iter->get_next_item());
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

int job_route(svr_job *jobp)
  {
  fprintf(stderr, "The call to job_route to be mocked!!\n");
  exit(1);
  }

int svr_enquejob(svr_job *pjob, int has_sv_qs_mutex, const char *prev_id, bool reservation, bool recov)
  {
  return(0);
  }

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

int svr_chkque(svr_job *pjob, pbs_queue *pque, char *hostname, int mtype, char *EMsg)
  {
  fprintf(stderr, "The call to svr_chkque to be mocked!!\n");
  exit(1);
  }

int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg)
  {
  strcpy(namebuf, shortname);
  return(PBSE_NONE);
  }

int remove_job(all_jobs *aj, svr_job *pjob, bool b)
  {
  return(0);
  }

int reply_jobid(struct batch_request *preq, const char *jobid, int which)
  {
  fprintf(stderr, "The call to reply_jobid to be mocked!!\n");
  exit(1);
  }

void issue_track(svr_job *pjob)
  {
  fprintf(stderr, "The call to issue_track to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(svr_job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  return(0);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry to be mocked!!\n");
  exit(1);
  }

svr_job *svr_find_job(const char *jobid, int get_subjob)
  {
  svr_job *pjob;

  pjob = (svr_job *)malloc(sizeof(svr_job));
  if (pjob == NULL)
    return(NULL);

  if (isalpha(jobid[0]))
    return(NULL);

  if (jobid[0] == '0')
    return(NULL);

  pjob->set_jobid(jobid);
  pjob->set_substate(JOB_SUBSTATE_QUEUED);
  pjob->set_fileprefix("1.napali");
  return(pjob);
  }

int svr_save(struct server *ps, int mode)
  {
  return(0);
  }

void replace_attr_string(struct pbs_attribute *attr, char *newval)
  {
  fprintf(stderr, "The call to replace_attr_string to be mocked!!\n");
  exit(1);
  }

svr_job *job_alloc(void)
  {
  if (mem_fail == true)
    return(NULL);
  else
    return((svr_job *)calloc(1, sizeof(svr_job)));
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  return(0);
  }

void svr_evaljobstate(svr_job &pjob, int &newstate, int &newsub, int forceeval)
  {
  return;
  }

pbs_queue *get_dfltque(void)
  {
  static pbs_queue pque;

  if (default_queue == true)
    return(&pque);

  return(NULL);
  }

void reply_badattr(int code, int aux, svrattrl *pal, struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_badattr to be mocked!!\n");
  exit(1);
  }

char *pbs_default(void)
  {
  return(server_name);
  }

pbs_net_t get_connectaddr(int sock, int mutex)
  {
  fprintf(stderr, "The call to get_connectaddr to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  if (long_to_set != -1)
    *l = long_to_set;

  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  if (long_to_set != -1)
    *b = long_to_set;

  return(0);
  }

int get_svr_attr_str(int index, char **str)
  {
  if (str_to_set[0] != '\0')
    *str = str_to_set;

  return(0);
  }

int enqueue_threadpool_request(

  void *(*func)(void *),
  void *arg,
  threadpool_t *tp)

  {
  return(0);
  }

int unlock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

int lock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

struct pbsnode *find_nodebyname(

  const char *nodename) /* I */

  {
  return(NULL);
  }

void unlock_node(struct pbsnode *pnode, const char *func_id, const char *msg, int logging) {} 

int lock_queue(

  struct pbs_queue *the_queue,
  const char       *id,
  const char       *msg,
  int               logging)

  {
  return(0);
  }

int lock_ji_mutex(svr_job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int unlock_ji_mutex(svr_job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int  can_queue_new_job(const char *user_name, svr_job *pjob)
  {
  return(0);
  }

int  increment_queued_jobs(user_info_holder *uih, const char *user_name, svr_job *pjob)
  {
  return(0);
  }

int  decrement_queued_jobs(user_info_holder *uih, const char *user_name, svr_job *pjob)
  {
  return(0);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

mutex_mgr::mutex_mgr(pthread_mutex_t *mutex, bool locked) {}

int mutex_mgr::unlock() 
  {
  return(0);
  }

mutex_mgr::~mutex_mgr() {}

int safe_strncat(char *str, const char *to_append, size_t space_remaining)
  {
  return(0);
  }

const char *prefix_std_file(
  svr_job            *pjob,
  std::string&    ds,
  int             key)
  {
  return "prefix";
  }

const char *add_std_filename(
  svr_job            *pjob,
  const char     *path,
  int             key,
  std::string&    ds)
  {
  return "stdfilename";
  }

svr_job *find_job_by_array(all_jobs *aj, const char *jobid, int get_subjob, bool locked)
{
  if (!strcmp(jobid, "1.napali"))
    {
    svr_job *pj = (svr_job *)calloc(1, sizeof(svr_job));
    pj->set_fileprefix("1.napali");
    if (set_ji_substate)
      pj->set_substate(JOB_SUBSTATE_TRANSICM);
    return(pj);
    }

  return(NULL);
  }

id_map::id_map() {}

id_map::~id_map() {}

int id_map::get_new_id(const char *job_name)
  {
  static int i = 0;
  return(i++);
  }

id_map job_mapper;

void log_ext(int i, char const* s, char const* s2, int i2)
  {
  }

std::string get_path_jobdata(const char *a, const char *b) {return "";}

const char *pbsnode::get_name() const
  {
  return(this->nd_name.c_str());
  }

int pbsnode::unlock_node(const char *id, const char *msg, int level)
  {
  return(0);
  }

job::job() {}
job::~job() {}
svr_job::svr_job() 
  {
  this->ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(this->ji_mutex, NULL);
  }
svr_job::~svr_job() {}

int node_avail_complex(

  char *spec,   /* I - node spec */
  int  *navail, /* O - number available */
  int  *nalloc, /* O - number allocated */
  int  *nresvd, /* O - number reserved  */
  int  *ndown)  /* O - number down      */

  {
  return(0);
  }

struct batch_request *alloc_br(

  int type)

  {

  struct batch_request *req = NULL;

  req = (struct batch_request *)calloc(1, sizeof(struct batch_request));

  if (req == NULL)
    {
    return(NULL);
    }

  req->rq_type = type;

  req->rq_conn = -1;  /* indicate not connected */
  req->rq_orgconn = -1;  /* indicate not connected */
  req->rq_time = time_now;
  req->rq_reply.brp_choice = BATCH_REPLY_CHOICE_NULL;
  req->rq_noreply = FALSE;  /* indicate reply is needed */


  return(req);
  }


int req_runjob(batch_request *preq)
  {
  return(0);
  }

batch_request::batch_request() {}
batch_request::batch_request(int type) : rq_type(type) {}

batch_request::~batch_request()

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



