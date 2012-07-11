#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attribute_def */
#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request.h */
#include "work_task.h" /* work_task, all_tasks */

char *msg_jobrerun = "Job Rerun";
attribute_def job_attr_def[10];
char *msg_manager = "%s at request of %s@%s";


void account_record(int acctype, job *pjob, char *text)
  {
  fprintf(stderr, "The call to account_record to be mocked!!\n");
  exit(1);
  }

char *parse_servername(char *name, unsigned int *service)
  {
  fprintf(stderr, "The call to parse_servername to be mocked!!\n");
  exit(1);
  }

void svr_mailowner(job *pjob, int mailpoint, int force, char *text)
  {
  fprintf(stderr, "The call to svr_mailowner to be mocked!!\n");
  exit(1);
  }

void set_statechar(job *pjob)
  {
  fprintf(stderr, "The call to set_statechar to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

void rel_resc( job *pjob)  
  {
  fprintf(stderr, "The call to rel_resc to be mocked!!\n");
  exit(1);
  }

void delete_task(struct work_task *ptask)
  {
  fprintf(stderr, "The call to delete_task to be mocked!!\n");
  exit(1);
  }

int issue_signal(job *pjob, char *signame, void (*func)(struct work_task *), void *extra)
  {
  fprintf(stderr, "The call to issue_signal to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req to be mocked!!\n");
  exit(1);
  }

work_task *next_task(all_tasks *at, int *iter)
  {
  fprintf(stderr, "The call to next_task to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job *pjob, int *newstate, int *newsub, int forceeval)
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
