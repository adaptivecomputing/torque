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


struct batch_request *alloc_br(int type)
  {
  batch_request *preq;
  if (alloc_br_null)
    return(NULL);

  preq = (batch_request *)calloc(1, sizeof(batch_request));
  preq->rq_type = type;

  return(preq);
  }

char *parse_servername(const char *name, unsigned int *service)
  {
  return(strdup(name));
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  return(0);
  }

int svr_job_purge(job *pjob, int leaveSpoolFiles)
  {
  purged = true;
  return(0);
  }

void svr_mailowner_with_message(job *pjob, int mailpoint, int force, const char *text,const char *msg) {}

void svr_mailowner(job *pjob, int mailpoint, int force, const char *text) {}

pbs_net_t get_hostaddr(int *local_errno, const char *hostname)
  {
  return(0);
  }

long attr_ifelse_long(pbs_attribute *attr1, pbs_attribute *attr2, long deflong)
  {
  return(0);
  }

pbs_queue *get_jobs_queue(job **pjob)
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

job_array *get_jobs_array(job **pjob)
  {
  static job_array pa;

  if (double_bad)
    {
    *pjob = NULL;
    return(NULL);
    }

  return(&pa);
  }


void free_nodes(job *pjob, const char *spec) {}

void free_br(struct batch_request *preq) {}

struct work_task *set_task(enum work_type type, long event_id, void (*func)(work_task *), void *parm, int get_lock)
  {
  return(NULL);
  }

int depend_on_term(job *pjob)
  {
  return(0);
  }

int issue_Drequest(int conn, batch_request *request, bool close_handle)
  {
  if (bad_drequest)
    return(-1);

  return(0);
  }

void set_resc_assigned(job *pjob, enum batch_op op) {}

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

void issue_track(job *pjob) {}

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  pjob->ji_qs.ji_state = newstate;
  pjob->ji_qs.ji_substate = newsubstate;
  return(0);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  job *pjob = NULL;

  if (bad_job == 0)
    {
    pjob = (job *)calloc(1, sizeof(job));
    strcpy(pjob->ji_qs.ji_jobid, jobid);
    pjob->ji_wattr[JOB_ATR_reported].at_flags = ATR_VFLAG_SET;
  
    if (reported)
      pjob->ji_wattr[JOB_ATR_reported].at_val.at_long = 1;

    if (exited == true)
      pjob->ji_qs.ji_state = JOB_STATE_EXITING;

    if (completed == true)
      pjob->ji_qs.ji_state = JOB_STATE_COMPLETE;
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

void get_jobowner(char *from, char *to)
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

int lock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
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

int record_job_as_exiting(job *pjob)
  {
  return(0);
  }

int remove_job_from_exiting_list(job **pjob)
  {
  return(0);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text)
  {
  }

void log_err(int error, const char *func_id, const char *msg) {}

void log_record(int eventtype, int objclass, const char *objname, const char *text) {}

void account_jobend(job *pjob, std::string &data) 
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
  memset(this->ji_wattr, 0, sizeof(this->ji_wattr));
  this->ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(this->ji_mutex, NULL);
  }

job::~job() {}

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

