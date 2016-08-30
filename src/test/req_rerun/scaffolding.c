#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attribute_def */
#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request.h */
#include "work_task.h" /* work_task, all_tasks */
#include "server.h"

const char    *msg_jobrerun = "Job Rerun";
attribute_def  job_attr_def[10];
const char    *msg_manager = "%s at request of %s@%s";
int            LOGLEVEL     = 7; /* force logging code to be exercised as tests run */
bool           exit_called = false;
all_jobs       alljobs;


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

void svr_mailowner(job *pjob, int mailpoint, int force, const char *text)
  {
  }

void set_statechar(job *pjob)
  {
  }

void reply_ack(struct batch_request *preq)
  {
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  }

void rel_resc( job *pjob)  
  {
  }

void delete_task(struct work_task *ptask)
  {
  }

int issue_signal(job **pjob_ptr, const char *signame, void (*func)(batch_request *), void *extra, char *extend)
  {
  fprintf(stderr, "The call to issue_signal to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  }

work_task *next_task(all_tasks *at, int *iter)
  {
  fprintf(stderr, "The call to next_task to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  pjob->ji_qs.ji_state = newstate;
  pjob->ji_qs.ji_substate = newsubstate;
  return(0);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  fprintf(stderr, "The call to find_job to be mocked!!\n");
  exit(1);
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

void free_br(batch_request *preq) {} 

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

void free_nodes(job *pjob, const char *spec) {}

int svr_job_purge(job *pjob, int leaveSpoolFiles)
  {
  return(0);
  }

void set_resc_assigned(job *pjob, enum batch_op op) {}

struct work_task *set_task(

  enum work_type   type,
  long             event_id,  /* I - based on type can be time of event */
  void           (*func)(struct work_task *),
  void            *parm,
  int              get_lock)

  {
  return(NULL);
  }


struct server server;

job *next_job(

  all_jobs          *aj,
  all_jobs_iterator *iter)

  {
  static int i = 0;

  if (++i % 3 == 0)
    return(NULL);

  job *pjob = (job *)calloc(1, sizeof(job));
  pjob->ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  return(pjob);
  }

batch_request *duplicate_request(

  batch_request *preq,      /* I */
  int            job_index) /* I - optional */

  {
  return(NULL);
  }
