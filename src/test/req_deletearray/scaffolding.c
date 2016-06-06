#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "server.h" /* server */
#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */
#include "attribute.h" /* pbs_attribute */
#include "work_task.h" /* work_task, all_tasks */
#include "queue.h" /* pbs_queue */
#include "array.h" /* job_array */

const char *msg_permlog = "Unauthorized Request, request type: %d, Object: %s, Name: %s, request from: %s@%s";
struct server server;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */

int svr_authorize_req(struct batch_request *preq, char *owner, char *submit_host)
  {
  fprintf(stderr, "The call to svr_authorize_req needs to be mocked!!\n");
  exit(1);
  }

int has_job_delete_nanny(struct job *pjob)
  {
  fprintf(stderr, "The call to has_job_delete_nanny needs to be mocked!!\n");
  exit(1);
  }

long attr_ifelse_long(pbs_attribute *attr1, pbs_attribute *attr2, long deflong)
  {
  fprintf(stderr, "The call to attr_ifelse_long needs to be mocked!!\n");
  exit(1);
  }

void on_job_exit(struct work_task *ptask)
  {
  fprintf(stderr, "The call to on_job_exit needs to be mocked!!\n");
  exit(1);
  }

void remove_stagein(job **pjob_ptr)
  {
  fprintf(stderr, "The call to remove_stagein needs to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_jobs_queue(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_queue needs to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack needs to be mocked!!\n");
  exit(1);
  }

struct work_task *apply_job_delete_nanny(struct job *pjob, int delay)
  {
  fprintf(stderr, "The call to apply_job_delete_nanny needs to be mocked!!\n");
  exit(1);
  }

 int delete_array_range(job_array *pa, char *range_str)
  {
  fprintf(stderr, "The call to delete_array_range needs to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(struct work_task *), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task needs to be mocked!!\n");
  exit(1);
  }

job_array *get_array(const char *id)
  {
  return(NULL);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject needs to be mocked!!\n");
  exit(1);
  }

int job_abt(struct job **pjobp, const char *text, bool b=false)
  {
  fprintf(stderr, "The call to job_abt needs to be mocked!!\n");
  exit(1);
  }

int issue_signal(job **pjob_ptr, const char *signame, void (*func)(batch_request *), void *extra, char *extend)
  {
  fprintf(stderr, "The call to issue_signal needs to be mocked!!\n");
  exit(1);
  }

work_task *next_task(all_tasks *at, int *iter)
  {
  fprintf(stderr, "The call to next_task needs to be mocked!!\n");
  exit(1);
  }

void change_restart_comment_if_needed(struct job *pjob)
  {
  fprintf(stderr, "The call to change_restart_comment_if_needed needs to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate needs to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue needs to be mocked!!\n");
  exit(1);
  }

int insert_task(all_tasks *at, work_task *wt)
  {
  fprintf(stderr, "The call to insert_task needs to be mocked!!\n");
  exit(1);
  }

void get_jobowner(char *from, char *to)
  {
  fprintf(stderr, "The call to get_jobowner needs to be mocked!!\n");
  exit(1);
  }

int delete_whole_array(job_array *pa)
  {
  fprintf(stderr, "The call to delete_whole_array needs to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  return(NULL);
  }

int array_delete(
    
  job_array *pa)

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

int unlock_ai_mutex(job_array *pa, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int lock_ai_mutex(job_array *pa, const char *id, char *msg, int logging)
  {
  return(0);
  }

void free_br(struct batch_request *preq) {}

void on_job_exit_task(struct work_task *ptask) {} 

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

int delete_inactive_job(job **, const char *)
  {
  return(0);
  }

void setup_apply_job_delete_nanny(

  job    *pjob,           /* I */
  time_t  time_now)       /* I */

  {
  }
