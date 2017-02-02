#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <time.h> /* timeval */
#include <pthread.h> /* pthread_mutex_t */

#include "attribute.h" /* attribute_def, svrattrl, pbs_attribute */
#include "net_connect.h" /* pbs_net_t, conn_type */
#include "server_limits.h" /* RECOV_WARM */
#include "work_task.h" /* work _task, work_type, all_tasks */
#include "server.h" /* server */
#include "sched_cmds.h" /* SCH_SCHEDULE_NULL */
#include "batch_request.h" /* batch_request */
#include "array.h" /* job_array */
#include "pbs_job.h" /* job */
#include "node_func.h" /* node_info */
#include "list_link.h" /* list_link */
#include "pbs_ifl.h" /* batch_op */
#include "array.h" /* ArrayEventsEnum */
#include "pbs_nodes.h" /* pbsnode */
#include "queue.h" /* pbs_queue */
#include "id_map.hpp"
#include "completed_jobs_map.h"
#include "resource.h"
#include "track_alps_reservations.hpp"


bool cray_enabled;
int  attr_count = 0;
int  next_count = 0;
const char *msg_momnoexec2 = "Job cannot be executed\nSee job standard error file";
const char *msg_job_end_sig = "Terminated on signal %d";
const char *msg_obitnojob  = "Job Obit notice received from %s has error %d";
attribute_def job_attr_def[10];
const char *msg_obitnocpy = "Post job file processing error; job %s on host %s";
int   server_init_type = RECOV_WARM;
char *path_spool;
const char *msg_init_abt = "Job aborted on PBS Server initialization";
pbs_net_t pbs_server_addr;
const char *PJobState[] = {"hi", "hello"};
struct server server;
int  svr_do_schedule = SCH_SCHEDULE_NULL;
int listener_command = SCH_SCHEDULE_NULL;
int LOGLEVEL = 10;
const char *msg_obitnodel = "Unable to delete files for job %s, on host %s";
const char *msg_momnoexec1 = "Job cannot be executed\nSee Administrator for help";
const char *msg_job_end_stat = "Exit_status=%d";
const char *msg_momjobovermemlimit = "Job exceeded a memory resource limit (vmem, pvmem, etc.). Job was aborted\nSee Administrator for help";
const char *msg_momjoboverwalltimelimit = "Job exceeded its walltime limit. Job was aborted\nSee Administrator for help";
const char *msg_momjobovercputlimit = "Job exceeded its cpu time limit. Job was aborted\nSee Administrator for help";
pthread_mutex_t *svr_do_schedule_mutex;
pthread_mutex_t *listener_command_mutex;
extern int alloc_br_null;
int bad_connect;
int bad_job;
int bad_queue;
int double_bad;
int reported;
int bad_drequest;
int usage;
int called_account_jobend;
bool purged = false;
bool completed = false;
bool exited = false;
long disable_requeue = 0;
completed_jobs_map_class completed_jobs_map;



char *parse_servername(const char *name, unsigned int *service)
  {
  return(strdup(name));
  }

int svr_job_save(svr_job *pjob)
  {
  return(0);
  }

int svr_job_purge(svr_job *pjob, int leaveSpoolFiles)
  {
  purged = true;
  return(0);
  }

void svr_mailowner_with_message(svr_job *pjob, int mailpoint, int force, const char *text,const char *msg) {}

void svr_mailowner(svr_job *pjob, int mailpoint, int force, const char *text) {}

pbs_net_t get_hostaddr(int *local_errno, const char *hostname)
  {
  return(0);
  }

long attr_ifelse_long(pbs_attribute *attr1, pbs_attribute *attr2, long deflong)
  {
  return(0);
  }

pbs_queue *get_jobs_queue(svr_job **pjob)
  {
  static pbs_queue bob;

  if (bad_queue)
    {
    return(NULL);
    }
  else if (double_bad)
    {
    *pjob = NULL;
    return(NULL);
    }

  return(&bob);
  }

job_array *get_jobs_array(svr_job **pjob)
  {
  static job_array pa;

  if (double_bad)
    {
    *pjob = NULL;
    return(NULL);
    }

  return(&pa);
  }


void free_nodes(svr_job *pjob, const char *spec) {}

void free_br(struct batch_request *preq) {}

struct work_task *set_task(enum work_type type, long event_id, void (*func)(work_task *), void *parm, int get_lock)
  {
  return(NULL);
  }

int depend_on_term(svr_job *pjob)
  {
  return(0);
  }

int issue_Drequest(int conn, batch_request *request, bool close_handle)
  {
  if (bad_drequest)
    return(-1);

  return(0);
  }

void set_resc_assigned(svr_job *pjob, enum batch_op op) {}

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  if (pobj != NULL)
    {
    new_link->ll_struct = pobj;
    }
  else
    {
    /* WARNING: This mixes list_link pointers and ll_struct
         pointers, and may break if the list_link we are operating
         on is not the first embeded list_link in the surrounding
         structure, e.g. work_task.wt_link_obj */

    new_link->ll_struct = (void *)new_link;
    }

  new_link->ll_prior = head->ll_prior;

  new_link->ll_next  = head;
  head->ll_prior = new_link;
  new_link->ll_prior->ll_next = new_link; /* now visible to forward iteration */
  }

int svr_connect(unsigned long, unsigned int, int*, pbsnode*, void* (*)(void*))
  {
  if (bad_connect)
    return(-1);

  return(1);
  }

void issue_track(svr_job *pjob) {}

int svr_setjobstate(svr_job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  pjob->set_state(newstate);
  pjob->set_substate(newsubstate);
  return(0);
  }

svr_job *svr_find_job(const char *jobid, int get_subjob)
  {
  svr_job *pjob = NULL;

  if (bad_job == 0)
    {
    pjob = new svr_job();

    pjob->set_jobid(jobid);
    pjob->set_bool_attr(JOB_ATR_reported, reported);

    if (exited == true)
      pjob->set_state(JOB_STATE_EXITING);

    if (completed == true)
      pjob->set_state(JOB_STATE_COMPLETE);
    }

  return(pjob);
  }

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
  {
  return(0);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  return(0);
  }

void get_jobowner(const char *from, char *to)
  {
  strcpy(to, "dbeer");
  }


int get_svr_attr_l(int index, long *l)
  {
  if (usage)
    *l = 0x0010;

  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  if (index == SRV_ATR_DisableAutoRequeue)
    *b = disable_requeue;
  else if (cray_enabled)
    *b = true;

  return(0);
  }

int safe_strncat(char *str, const char *to_append, size_t space_remaining)
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

int lock_ji_mutex(svr_job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

struct pbsnode *find_nodebyname(

  const char *nodename) /* I */

  {
  return(NULL);
  }

int kill_job_on_mom(

  const char     *job_id,
  struct pbsnode *pnode)

  {
  return(0);
  }

int unlock_node(
    
  struct pbsnode *the_node,
  const char     *id,
  const char           *msg,
  int             logging)

  {
  return(0);
  }

int unlock_ai_mutex(job_array *pa, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int record_job_as_exiting(svr_job *pjob)
  {
  return(0);
  }

int remove_job_from_exiting_list(svr_job **pjob)
  {
  return(0);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text)
  {
  }

void log_err(int error, const char *func_id, const char *msg) {}

void log_record(int eventtype, int objclass, const char *objname, const char *text) {}

void account_jobend(svr_job *pjob, std::string &data) 
  {
  called_account_jobend++;
  }

id_map::id_map() {}

id_map::~id_map() {}

const char *id_map::get_name(int id)
  {
  return(NULL);
  }

id_map job_mapper;

completed_jobs_map_class::completed_jobs_map_class() {}
completed_jobs_map_class::~completed_jobs_map_class() {}
int completed_jobs_map_class::cleanup_completed_jobs() {return 0;}
bool completed_jobs_map_class::add_job(char const *s, time_t t) {return true;}

int attr_to_str(

  std::string&      ds,     /* O */
  attribute_def    *at_def, /* I */
  pbs_attribute     attr,   /* I */
  bool              XML)    /* I */

  {
  char        buf[100];

  switch (attr_count)
    {
    case 0:

      sprintf(buf, "%d", 100);
      ds += buf;
      break;

    case 1:
      
      sprintf(buf, "%dmb", 4096);
      ds += buf;
      break;

    case 2:
      
      sprintf(buf, "%dmb", 8192);
      ds += buf;
      break;

    default:
      return(NO_ATTR_DATA);
    }

  attr_count++;

  return(PBSE_NONE);
  }


void *get_next(

  list_link  pl,   /* I */
  char     *file, /* I */
  int      line) /* I */

  {
  static struct resource_def   rs_def;
  static struct pbs_attribute  pattr;
  resource                    *r = NULL;

  switch (next_count)
    {
    case 0:

      r = (resource *)calloc(1, sizeof(resource));
      r->rs_defin = &rs_def;
      rs_def.rs_name = "cput";
      r->rs_value = pattr;
      break;

    case 1:
      
      r = (resource *)calloc(1, sizeof(resource));
      r->rs_defin = &rs_def;
      rs_def.rs_name = "mem";
      r->rs_value = pattr;
      break;

    case 2:
      
      r = (resource *)calloc(1, sizeof(resource));
      r->rs_defin = &rs_def;
      rs_def.rs_name = "vmem";
      r->rs_value = pattr;
      break;

    default:
      break;
    }

  next_count++;

  return(r);
  }

void add_to_completed_jobs(work_task *ptask) {}

pbsnode::pbsnode() {}
pbsnode::~pbsnode() {}

int pbsnode::get_version() const

  {
  return(0);
  }

int pbsnode::unlock_node(const char *id, const char *msg, int level)
  {
  return(0);
  }

job::job() 
  {
  memset(&this->ji_qs, 0, sizeof(this->ji_qs));
  memset(this->ji_wattr, 0, sizeof(this->ji_wattr));
  }

job::~job() {}
svr_job::svr_job() 
  {
  this->ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(this->ji_mutex, NULL);
  }
svr_job::~svr_job() {}

void svr_job::add_plugin_resource_usage(std::string &acct_data) const 
  {
  }

int get_time_string(char *buf, int bufsize, long timeval)
  {
  return(0);
  }

reservation_holder alps_reservations;

reservation_holder::reservation_holder() {}

int reservation_holder::remove_alps_reservation(

  const char *rsv_id)

  {
  return(PBSE_NONE);
  }

void set_reply_type(struct batch_reply *preply, int type)
  {
  preply->brp_choice = type;
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

job_array::job_array() {}
job_array::~job_array() {}

array_info::array_info() {}
array_info::~array_info() {}

void job_array::update_array_values(

  int                   old_state, /* I */
  enum ArrayEventsEnum  event,     /* I */
  const char           *job_id,
  int                   job_exit_status)

  {
  }

batch_request::batch_request(int type) : rq_type(type)
  {
  }

batch_request::batch_request()
  {
  }

batch_request::~batch_request()

  {
  }

void cleanup_restart_file(svr_job *)
  {
  }

void rel_resc(

  svr_job *pjob)  /* I (modified) */

  {
  }
