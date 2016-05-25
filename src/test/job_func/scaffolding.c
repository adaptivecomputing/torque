#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string>
#include <semaphore.h>

#include "pbs_ifl.h" /* MAXPATHLEN, PBS_MAXSERVERNAME */
#include "server.h" /* server, NO_BUFFER_SPACE */
#include "pbs_job.h" /* all_jobs, job_array, job */
#include "attribute.h" /* pbs_attribute, attribute_def */
#include "net_connect.h" /* pbs_net_t */
#include "list_link.h" /* list_link */
#include "batch_request.h" /* batch_request */
#include "work_task.h" /* all_tasks */
#include "array.h" /* ArrayEventsEnum */
#include "id_map.hpp"
#include "completed_jobs_map.h"

/* This section is for manipulting function return values */
#include "test_job_func.h" /* *_SUITE */
#include "user_info.h"
int func_num = 0; /* Suite number being run */
int tc = 0; /* Used for test routining */
int iter_num = 0;
int called_remove_job;
int dequejob_rc;

bool exit_called = false;

int valbuf_size = 0;
/* end manip */

sem_t *job_clone_semaphore;
char *path_jobs;
char path_checkpoint[MAXPATHLEN + 1];
char *job_log_file = NULL;
char server_name[PBS_MAXSERVERNAME + 1];
const char *msg_abt_err = "Unable to abort Job %s which was in substate %d";
int queue_rank = 0;
char *path_spool;
const char *msg_err_purgejob = "Unlink of job file failed";
struct server server;
all_jobs array_summary;
all_jobs alljobs;
char *path_jobinfo_log;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
pthread_mutex_t job_log_mutex = PTHREAD_MUTEX_INITIALIZER;
completed_jobs_map_class completed_jobs_map;

user_info_holder users;
extern bool add_job_called;

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int errnum, const char *routine, const char *text, int severity) {}

int array_save(job_array *pa)
  {
  fprintf(stderr, "The call to array_save needs to be mocked!!\n");
  exit(1);
  }

int job_route(job *jobp)
  {
  fprintf(stderr, "The call to job_route to be mocked!!\n");
  exit(1);
  }

int relay_to_mom(job **pjob_ptr, batch_request   *request, void (*func)(struct work_task *))
  {
  return(0);
  }

void account_record(int acctype, job *pjob, const char *text)
  {
  fprintf(stderr, "The call to account_record needs to be mocked!!\n");
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

void svr_mailowner(job *pjob, int mailpoint, int force, const char *text)
  {
  fprintf(stderr, "The call to svr_mailowner needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  memset(pattr, 0, sizeof(pbs_attribute));

  pattr->at_type = pdef->at_type;

  if ((pattr->at_type == ATR_TYPE_RESC) ||
      (pattr->at_type == ATR_TYPE_LIST))
    {
    CLEAR_HEAD(pattr->at_val.at_list);
    }
  }

pbs_net_t get_hostaddr(int *local_errno, const char *hostname)
  {
  pbs_net_t loopback = ntohl(INADDR_LOOPBACK);
  return(loopback);
  }

int log_job_record(const char *buf)
  {
  int rc = 0;
  if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 2))
    rc = -1;
  if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 8))
    {
    if (iter_num == 1)
      rc = -1;
    iter_num++;
    }
  return rc;
  }

int job_log_open(char *filename, char *directory)
  {
  int rc = 0;
  if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 1))
    rc = -1;
  return rc;
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br needs to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(work_task *), void *parm, int get_lock)
  {
  add_job_called = true;
  return NULL;
  }

int svr_dequejob(job *pjob, int val)
  {
  return(dequejob_rc);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void initialize_all_tasks_array(all_tasks *at)
  {
  fprintf(stderr, "The call to initialize_all_tasks_array needs to be mocked!!\n");
  exit(1);
  }

job_array *get_array(const char *id)
  {
  return(NULL);
  }

job *get_recycled_job()
  {
  fprintf(stderr, "The call to get_recycled_job needs to be mocked!!\n");
  exit(1);
  }

void delete_task(struct work_task *ptask)
  {
  fprintf(stderr, "The call to delete_task needs to be mocked!!\n");
  exit(1);
  }

int depend_on_term(job *pjob)
  {
  return(0);
  }

int client_to_svr(pbs_net_t hostaddr, unsigned int port, int local_port, char *EMsg)
  {
  fprintf(stderr, "The call to client_to_svr mock always return 0!\n");
  return(0);
  }

void *get_next(list_link pl, char *file, int line)
  {
  return(NULL);
  }

int issue_signal(job **pjob, const char *signame, void (*func)(batch_request *), void *extra, char *extend)
  {
  fprintf(stderr, "The call to issue_signal needs to be mocked!!\n");
  exit(1);
  }

int svr_enquejob(job *pjob, int has_sv_qs_mutex, const char *prev_jobid, bool reservation, bool recov)
  {
  fprintf(stderr, "The call to svr_enquejob needs to be mocked!!\n");
  exit(1);
  }

void update_array_values(job_array *pa, int old_state, enum ArrayEventsEnum event, const char *job_id, long job_atr_hold, int job_exit_status)
  {
  fprintf(stderr, "The call to update_array_values needs to be mocked!!\n");
  exit(1);
  }

int array_delete(job_array *pa)
  {
  fprintf(stderr, "The call to array_delete needs to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req needs to be mocked!!\n");
  exit(1);
  }

work_task *next_task(all_tasks *at, int *iter)
  {
  fprintf(stderr, "The call to next_task needs to be mocked!!\n");
  exit(1);
  }

void issue_track(job *pjob)
  {
  fprintf(stderr, "The call to issue_track needs to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  return 0;
  }

struct batch_request *setup_cpyfiles(struct batch_request *preq, job *pjob, char *from, char *to, int direction, int tflag)
  {
  fprintf(stderr, "The call to setup_cpyfiles needs to be mocked!!\n");
  exit(1);
  }

int insert_into_recycler(job *pjob)
  {
  return 0;
  }

int attr_to_str(std::string& ds, attribute_def *attr_def,struct pbs_attribute attr, bool XML)
  {
  int rc = 0;
  if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 4))
    {
    if (iter_num == 0)
      {
      iter_num++;
      rc = NO_BUFFER_SPACE;
      }
    else if (iter_num == 1)
      {
      iter_num++;
      }
    }
  else if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 5))
    {
    rc = NO_BUFFER_SPACE;
    }
  else if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 6))
    {
    iter_num++;
    }
  return rc;
  }

void check_job_log(struct work_task *ptask)
  {
  fprintf(stderr, "The call to check_job_log needs to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job &pjob, int &newstate, int &newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate needs to be mocked!!\n");
  exit(1);
  }

int lock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  return(0);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int is_svr_attr_set(int index)
  {
  return(0);
  }

int get_svr_attr_str(int index, char **str)
  {
  return(0);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *id, const char *msg, int logging)
  {
  return(0);
  }

struct pbs_queue *lock_queue_with_job_held(

  struct pbs_queue  *pque,
  job       **pjob_ptr)

  {
  return(NULL);
  }

int decode_ll(

  pbs_attribute *patr,
  const char   *name,  /* pbs_attribute name */
  const char *rescn, /* resource name, unused here */
  const char    *val,   /* pbs_attribute value */
  int            perm)  /* only used for resources */

  {
  return(0);
  }

int set_ll(
   
  pbs_attribute *attr, 
  pbs_attribute *new_attr,
  enum batch_op  op)

  {
  return(0);
  }

int encode_ll(

  pbs_attribute  *attr,   /* ptr to pbs_attribute */
  tlist_head     *phead,   /* head of attrlist list */
  const char    *atname,  /* pbs_attribute name */
  const char    *rsname,  /* resource name or null */
  int             mode,   /* encode mode, unused here */
  int             perm)  /* only used for resources */

  {
  return(0);
  }

int comp_ll(
   
  pbs_attribute *attr,
  pbs_attribute *with)

  {
  return(0);
  }

int remove_alps_reservation(
    
  char *rsv_id)

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

int  decrement_queued_jobs(user_info_holder *uih, char *user_name)
  {
  return(0);
  }

int set_str(
    
  pbs_attribute *attr,
  pbs_attribute *new_attr,
  enum batch_op  op)

  {
  char *new_value;
  char *p;
  size_t nsize;

  nsize = strlen(new_attr->at_val.at_str) + 1; /* length of new string */

  if ((op == INCR) && (attr->at_val.at_str == NULL))
    op = SET; /* no current string, change INCR to SET */

  switch (op)
    {

    case SET: /* set is replace old string with new */

      if ((new_value = (char *)calloc(1, nsize)) == NULL)
        return (PBSE_SYSTEM);

      if (attr->at_val.at_str)
        (void)free(attr->at_val.at_str);
      attr->at_val.at_str = new_value;

      (void)strcpy(attr->at_val.at_str, new_attr->at_val.at_str);

      break;

    case INCR: /* INCR is concatenate new to old string */

      nsize += strlen(attr->at_val.at_str);
      new_value = (char *)calloc(1, nsize + 1);

      if (new_value == NULL)
        return (PBSE_SYSTEM);

      strcat(new_value, attr->at_val.at_str);
      strcat(new_value, new_attr->at_val.at_str);

      free(attr->at_val.at_str);
      attr->at_val.at_str = new_value;

      break;

    case DECR: /* DECR is remove substring if match, start at end */

      if (attr->at_val.at_str == NULL)
        break;

      if (--nsize == 0)
        break;

      p = attr->at_val.at_str + strlen(attr->at_val.at_str) - nsize;

      while (p >= attr->at_val.at_str)
        {
        if (strncmp(p, new_attr->at_val.at_str, (int)nsize) == 0)
          {
          do
            {
            *p = *(p + nsize);
            }
          while (*p++);
          }

        p--;
        }

      break;

    default:
      return (PBSE_INTERNAL);
    }

  if ((attr->at_val.at_str != (char *)0) && (*attr->at_val.at_str != '\0'))
    attr->at_flags |= ATR_VFLAG_SET | ATR_VFLAG_MODIFY;
  else
    attr->at_flags &= ~ATR_VFLAG_SET;

  return (0);
  }


int lock_ai_mutex(job_array *pa, const char *func_id, char *msg, int logging)
  {
  return(0);
  }

int unlock_ai_mutex(job_array *pa, const char *func_id, const char *msg, int logging)
  {
  return(0);
  }

job *svr_find_job(const char *jobid, int sub)
  {
  return(NULL);
  }

int remove_job(all_jobs *aj, job             *pjob, bool b)
  {
  called_remove_job++;
  return(0);
  }

int get_jobs_index(all_jobs *aj, job             *pjob)
  {
  return(0);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

struct batch_request *alloc_br(int type)
  {
  struct batch_request *request = NULL;
  request = (batch_request *)calloc(1, sizeof(struct batch_request));
  request->rq_type = type;
  return request;
  }

void free_str(struct pbs_attribute *attr) {}
void free_resc(pbs_attribute *pattr) {}
void free_null(struct pbs_attribute *attr) {}
void free_depend(struct pbs_attribute *attr) {}
void free_arst( struct pbs_attribute *attr) {}
void free_unkn(pbs_attribute *pattr) {}

/* copied from job_container.c */

job *find_job_by_array(

  all_jobs *aj,
  char            *job_id,
  int              get_subjob,
  bool             locked)

  {
  job *pj = NULL;

  if (aj == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "null all_jobs pointer fail");
    return(NULL);
    }
  if (job_id == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "null job_id pointer fail");
    return(NULL);
    }

  aj->lock();
  pj = aj->find(job_id);
  if (pj != NULL)
    lock_ji_mutex(pj, __func__, NULL, LOGLEVEL);

  aj->unlock();

  if (pj != NULL)
    {
    if (get_subjob == TRUE)
      {
      if (pj->ji_cray_clone != NULL)
        {
        pj = pj->ji_cray_clone;
        unlock_ji_mutex(pj->ji_parent_job, __func__, NULL, LOGLEVEL);
        lock_ji_mutex(pj, __func__, NULL, LOGLEVEL);
        }
      }

    if (pj->ji_being_recycled == TRUE)
      {
      unlock_ji_mutex(pj, __func__, "1", LOGLEVEL);
      pj = NULL;
      }
    }

  return(pj);
  } /* END find_job_by_array() */

id_map::id_map() {}

id_map::~id_map() {}

int id_map::get_new_id(const char *id) 
  {
  return(-1);
  }

const char *id_map::get_name(int internal_job_id)
  {
  static char buf[1024];

  if (internal_job_id < 5)
    {
    snprintf(buf, sizeof(buf), "%d.napali", internal_job_id);

    return(buf);
    }

  return(NULL);
  }

id_map job_mapper;

int encode_complete_req(
    
  pbs_attribute *attr,
  tlist_head    *phead,
  const char    *atname,
  const char    *rsname,
  int            mode,
  int            perm)

  {
  return(0);
  }

int  decode_complete_req(
    
  pbs_attribute *patr,
  const char    *name,
  const char    *rescn,
  const char    *val,
  int            perm)

  {
  return(0);
  }

int comp_complete_req(
   
  pbs_attribute *attr,
  pbs_attribute *with)

  {
  return(0);
  } // END comp_complete_req()

void free_complete_req(

  pbs_attribute *patr) {}

int set_complete_req(
    
  pbs_attribute *attr,
  pbs_attribute *new_attr,
  enum batch_op  op)
  
  {
  return(0);
  }

 
void handle_complete_second_time(struct work_task *ptask)
  {
  }

completed_jobs_map_class::completed_jobs_map_class() {}
completed_jobs_map_class::~completed_jobs_map_class() {}
bool completed_jobs_map_class::add_job(char const* s, time_t t)
  {
  add_job_called = true;

  return false;
  }

std::string get_path_jobdata(const char *a, const char *b) {return ""; }

void add_to_completed_jobs(work_task *ptask) {}
