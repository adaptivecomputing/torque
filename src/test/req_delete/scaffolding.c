#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include <stdlib.h>
#include <stdio.h> /* sprintf */
#include <errno.h>

#include "pbs_job.h" /* all_jobs, job */
#include "server.h" /* server */
#include "array.h" /* job_array */
#include "batch_request.h" /* batch_request */
#include "attribute.h" /* pbs_attribute */
#include "work_task.h" /* work_task, all_tasks */
#include "queue.h" /* pbs_queue */
#include "node_func.h" /* node_info */
#include "threadpool.h"
#include "delete_all_tracker.hpp"

int lock_ji_mutex(svr_job *pjob, const char *id, const char *msg, int logging);
int unlock_ji_mutex(svr_job *pjob, const char *id, const char *msg, int logging);

#define MSG_LEN_LONG 160

threadpool_t *request_pool;
threadpool_t *async_pool;
const char *msg_deletejob = "Job deleted";
all_jobs alljobs;
const char *msg_delrunjobsig = "Job sent signal %s on delete";
extern struct server server;
extern time_t apply_job_delete_nanny_time;
const char *msg_manager = "%s at request of %s@%s";
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
long keep_seconds;
int  bad_queue;
int  bad_relay;
int  signal_issued;
int  nanny = 1;
bool  br_freed;
int  alloc_work = 1;
int  depend_term_called;
int  updated_array_values = 0;
bool find_job_fail = false;

batch_request *alloc_br(int type)
  {
  if (alloc_work)
    return(new batch_request());
  else
    return(NULL);
  }

job_array *get_jobs_array(svr_job **pjob)
  {
  if (pjob == NULL)
    return(NULL);

  return((job_array *)calloc(1, sizeof(job_array)));
  }

void account_record(int acctype, svr_job *pjob, const char *text)
  {
  }

int svr_job_save(svr_job *pjob)
  {
  return(0);
  }

int svr_job_purge(svr_job *pjob, int leaveSpoolFiles)
  {
  pjob->set_state(JOB_STATE_COMPLETE);
  pthread_mutex_unlock(pjob->ji_mutex);
  return(0);
  }

void chk_job_req_permissions(svr_job **pjob_ptr, struct batch_request *preq)
  {
  }

void svr_mailowner(svr_job *pjob, int mailpoint, int force, const char *text)
  {
  }

long attr_ifelse_long(pbs_attribute *attr1, pbs_attribute *attr2, long deflong)
  {
  return(keep_seconds);
  }

void on_job_exit(struct work_task *ptask)
  {
  fprintf(stderr, "The call to on_job_exit needs to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_jobs_queue(svr_job **pjob)
  {
  static pbs_queue pque;

  pque.qu_qs.qu_type = QTYPE_Execution;

  if (bad_queue)
    return(NULL);
  else
    return(&pque);
  }

void reply_ack(struct batch_request *preq) {}

void free_nodes(svr_job *pjob, const char *spec) 
  {
  pjob->set_str_attr(JOB_ATR_exec_host, NULL);
  }

void free_br(struct batch_request *preq)
  {
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(struct work_task *), void *parm, int get_lock)
  {
  apply_job_delete_nanny_time = event_id;
  return(NULL);
  }

void req_reject(int code, int aux, batch_request *preq, const char *HostName, const char *Msg) { }


void delete_task(struct work_task *ptask) { }

int svr_chk_owner(struct batch_request *preq, svr_job *pjob)
  {
  return(0);
  }

int job_abt(struct svr_job **pjobp, const char *text, bool b=false)
  {
  if (pjobp != NULL)
    *pjobp = NULL;

  return(0);
  }

int issue_signal(svr_job **pjob_ptr, const char *signame, void (*func)(struct batch_request *), void *extra, char *extend)
  {
  signal_issued = TRUE;
  return(PBSE_NONE);
  }

void set_resc_assigned(svr_job *pjob, enum batch_op op) { }

void release_req(struct work_task *pwt) { }

char *pbse_to_txt(int err)
  {
  return(strdup("bob"));
  }

 work_task *next_task(all_tasks *at, int *iter)
  {
  fprintf(stderr, "The call to next_task needs to be mocked!!\n");
  exit(1);
  }

batch_request *cpy_stage(batch_request *preq, svr_job *pjob, enum job_atr ati, int direction)
  {
  return(new batch_request());
  }

int svr_setjobstate(svr_job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  pjob->set_state(newstate);
  pjob->set_substate(newsubstate);
  return(0);
  }

svr_job *svr_find_job(const char *jobid, int get_subjob)
  {
  if ((strcmp(jobid, "1.napali") == 0) ||
      ((strstr(jobid, "roshar") != NULL) &&
       (find_job_fail == false)))
    {
    svr_job *pjob = new svr_job();
    pjob->set_jobid(jobid);
    pjob->set_state(JOB_STATE_RUNNING);
    return(pjob);
    }

  return(NULL);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void svr_evaljobstate(svr_job &pjob, int &newstate, int &newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate needs to be mocked!!\n");
  exit(1);
  }

int insert_task(all_tasks *at, work_task *wt)
  {
  fprintf(stderr, "The call to insert_task needs to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  if (nanny)
    *l = 1;
  else if (index == SRV_ATR_KeepCompleted || 
      index == SRV_ATR_JobNanny)
    {
    *l = server.sv_attr[index].at_val.at_long;
    }

  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  if (nanny)
    *b = true;
  else if (index == SRV_ATR_JobNanny)
    *b = server.sv_attr[index].at_val.at_bool;

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

int unlock_ai_mutex(job_array *pa, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void on_job_exit_task(struct work_task *ptask) {}

int enqueue_threadpool_request(void *(*func)(void *), void *arg, threadpool_t *tp)
  {
  return(0);
  }

void log_err(int l, const char *func_name, const char *msg) {}
void log_event(int type, int otype, const char *func_name, const char *msg) {}

int relay_to_mom(svr_job **pjob_ptr, batch_request   *request, void (*func)(struct work_task *))
  {
  return(bad_relay);
  }

void removeBeforeAnyDependencies(svr_job **pjob_ptr) {}


/*
 * insert a new job into the array
 *
 * @param pjob - the job to be inserted
 * @return PBSE_NONE on success 
 */
int insert_job(

  all_jobs *aj,
  svr_job             *pjob)

  {
  int rc = -1;

  if (aj == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc,__func__,"null svr_job array input");
    return(rc);
    }
  if (pjob == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc,__func__,"null job input");
    return(rc);
    }

  aj->lock();

  if(!aj->insert(pjob,pjob->get_jobid()))
    {
    rc = ENOMEM;
    log_err(rc, __func__, "No memory to resize the array...SYSTEM FAILURE\n");
    }
  else
    {
    rc = PBSE_NONE;
    }

  aj->unlock();

  return(rc);
  } /* END insert_job() */


svr_job *next_job(

  all_jobs *aj,
  all_jobs_iterator             *iter)

  {
  svr_job *pjob;

  if (aj == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "null input pointer to all_jobs struct");
    return(NULL);
    }
  if (iter == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "null input iterator");
    return(NULL);
    }

  aj->lock();

  pjob = iter->get_next_item();

  aj->unlock();

  if (pjob != NULL)
    {
    lock_ji_mutex(pjob, __func__, NULL, LOGLEVEL);

    if (pjob->ji_being_recycled == TRUE)
      {
      unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);

      pjob = next_job(aj,iter);
      }
    }

  return(pjob);
  } /* END next_job() */

int lock_ji_mutex(

  svr_job        *pjob,
  const char *id,
  const char *msg,
  int        logging)

  {
  int rc = PBSE_NONE;
  return rc;
  }


int unlock_ji_mutex(

  svr_job        *pjob,
  const char *id,
  const char *msg,
  int        logging)

  {
  int rc = PBSE_NONE;
  return rc;
  }

void log_record(

  int         eventtype,  /* I */
  int         objclass,   /* I */
  const char *objname,    /* I */
  const char *text)       /* I */

  {
  return;
  }

char *threadsafe_tokenizer(

  char       **str,    /* M */
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

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

delete_all_tracker::delete_all_tracker()
  {
  }

delete_all_tracker::~delete_all_tracker()
  {
  }

void delete_all_tracker::done_deleting_all(const char *user_name, int perm)
  {
  
  }

bool delete_all_tracker::start_deleting_all_if_possible(const char *username, int perm)
  {
  return(true);
  }

int get_fullhostname(

  char *shortname,  /* I */
  char *namebuf,    /* O */
  int   bufsize,    /* I */
  char *EMsg)       /* O (optional,minsize=MAXLINE - 1024) */

  {
  extern char      server_host[];
  struct addrinfo  hints;
  struct addrinfo *info;
  int              gai_result;

  char             hostname[1024];

  if (strncmp(shortname, server_host, strlen(shortname)))
    return(0);

  memset(&hostname, 0, sizeof(hostname));
  gethostname(hostname, sizeof(hostname));

  memset(&hints, 0, sizeof hints);
  hints.ai_flags = AI_CANONNAME;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0)
    {
    perror("couldn't getaddrinfo");
    perror(hostname);
    fprintf(stderr, "%s\n", gai_strerror(gai_result));
    return(-1);
    }

  snprintf(namebuf, bufsize, "%s", info->ai_canonname);

  freeaddrinfo(info);

  return(0);
  }

int depend_on_term(

  svr_job *pjob)

  {
  depend_term_called++;
  return(0);
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

pbs_attribute *job::get_attr(int index)
  {
  return(this->ji_wattr + index);
  }

job::job()
  {
  memset(&this->ji_qs, 0, sizeof(this->ji_qs));
  memset(this->ji_wattr, 0, sizeof(this->ji_wattr));
  }

job::~job() {}
svr_job::svr_job() : ji_has_delete_nanny(false)
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
  updated_array_values++;
  }

void job_array::mark_deleted() {}

batch_request::batch_request(const batch_request &other)
  {
  if (other.rq_extend == NULL)
    this->rq_extend = NULL;
  else
    this->rq_extend = strdup(other.rq_extend);
  }

batch_request::~batch_request()

  {
  br_freed = true;
  }

batch_request::batch_request() {}
batch_request::batch_request(int type) : rq_type(type) {}

