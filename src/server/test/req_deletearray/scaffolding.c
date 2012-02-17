#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "server.h" /* server */
#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */
#include "attribute.h" /* attribute */
#include "work_task.h" /* work_task, all_tasks */
#include "queue.h" /* pbs_queue */
#include "array.h" /* job_array */

char *msg_permlog = "Unauthorized Request, request type: %d, Object: %s, Name: %s, request from: %s@%s";
struct server server;
int LOGLEVEL = 0;

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

long attr_ifelse_long(attribute *attr1, attribute *attr2, long deflong)
  {
  fprintf(stderr, "The call to attr_ifelse_long needs to be mocked!!\n");
  exit(1);
  }

void on_job_exit(struct work_task *ptask)
  {
  fprintf(stderr, "The call to on_job_exit needs to be mocked!!\n");
  exit(1);
  }

void remove_stagein(job *pjob)
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

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task needs to be mocked!!\n");
  exit(1);
  }

job_array *get_array(char *id)
  {
  fprintf(stderr, "The call to get_array needs to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject needs to be mocked!!\n");
  exit(1);
  }

int job_abt(job **pjobp, char *text)
  {
  fprintf(stderr, "The call to job_abt needs to be mocked!!\n");
  exit(1);
  }

int issue_signal(job *pjob, char *signame, void (*func)(struct work_task *), void *extra)
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

void log_event(int eventtype, int objclass, const char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate needs to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, char *msg, int logging)
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

job *find_job(char *jobid)
  {
  return(NULL);
  }

void log_record(int eventtype, int objclass, const char *objname, char *text)
  {
  }
