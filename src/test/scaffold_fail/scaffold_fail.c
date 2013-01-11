#include<stdlib.h>
#include<stdio.h>
#include<pbs_config.h>

#include "license_pbs.h"
#include "list_link.h"
#include "portability.h"
#include "pbs_job.h"
#include "queue.h"
#include "batch_request.h"
#include "resource.h"
#include <time.h> /* timeval */
#include <pthread.h> /* pthread_mutex_t */
#include "attribute.h" /* attribute_def, svrattrl, pbs_attribute */
#include "net_connect.h" /* pbs_net_t, conn_type */
#include "server_limits.h" /* RECOV_WARM */
#include "work_task.h" /* work _task, work_type, all_tasks */
#include "server.h" /* server */
#include "sched_cmds.h" /* SCH_SCHEDULE_NULL */
#include "array.h" /* job_array */
#include "pbs_ifl.h" /* batch_op */
#include "array.h" /* ArrayEventsEnum */
#include "pbs_nodes.h" /* pbsnode */
#include "queue.h" /* pbs_queue */



int svr_movejob(job *jobp, char *destination, int *i, struct batch_request *req)
  {
  fprintf(stderr, "The call to svr_movejob needs to be mocked!!\n");
  exit(1);
  }

job *next_job(struct all_jobs *aj, int *iter)
  {
  fprintf(stderr, "The call to next_job needs to be mocked!!\n");
  exit(1);
  }

 int job_abt(struct job **pjobp, const char *text)
  {
  fprintf(stderr, "The call to job_abt needs to be mocked!!\n");
  exit(1);
  }

int site_alt_router(job *jobp, pbs_queue *qp, long retry_time)
  {
  fprintf(stderr, "The call to site_alt_router needs to be mocked!!\n");
  exit(1);
  }

 char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  exit(1);
  }

int enqueue_threadpool_request(void *(*func)(void *), void *arg)
  {
  fprintf(stderr, "The call to enqueue_threadpool_request needs to be mocked!!\n");
  exit(1);
  }

/*
pbs_queue *get_jobs_queue(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_queue needs to be mocked!!\n");
  exit(1);
  }
*/


int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue needs to be mocked!!\n");
  exit(1);
  }

/*
job *svr_find_job(char *jobid, int get_subjob)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }
*/

job_array *get_jobs_array(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_array to be mocked!!\n");
  exit(1);
  }

int modify_job_attr(job *pjob, svrattrl *plist, int perm, int *bad)
  {
  fprintf(stderr, "The call to modify_job_attr to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

void cleanup_restart_file( job *pjob)
  {
  fprintf(stderr, "The call to cleanup_restart_file to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req to be mocked!!\n");
  exit(1);
  }

void remove_job_delete_nanny( struct job *pjob)
  {
  fprintf(stderr, "The call to remove_job_delete_nanny to be mocked!!\n");
  exit(1);
  }

void account_jobend( job *pjob, char *used)
  {
  fprintf(stderr, "The call to account_jobend to be mocked!!\n");
  exit(1);
  }

void svr_disconnect(int handle)
  {
  fprintf(stderr, "The call to svr_disconnect to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

void update_array_values(job_array *pa, int old_state, enum ArrayEventsEnum event, char *job_id, long job_atr_hold, int job_exit_status)
  {
  fprintf(stderr, "The call to update_array_values to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job *pjob, int *newstate, int *newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate to be mocked!!\n");
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

void close_conn(int sd, int mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

int remove_alps_reservation(char *rsv_id)
   {
   fprintf(stderr,"Function %s needs to be mocked.\n",__func__);
   exit(1);
   }

int destroy_alps_reservation(char *reservation_id, char *apbasil_path, char *apbasil_protocol)
   {
   fprintf(stderr,"Function %s needs to be mocked.\n",__func__);
   exit(1);
   }
