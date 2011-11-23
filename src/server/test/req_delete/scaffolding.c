#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* sprintf */

#include "pbs_job.h" /* all_jobs, job */
#include "server.h" /* server */
#include "array.h" /* job_array */
#include "batch_request.h" /* batch_request */
#include "attribute.h" /* attribute */
#include "work_task.h" /* work_task, all_tasks */
#include "queue.h" /* pbs_queue */
#include "node_func.h" /* node_info */


char *msg_deletejob = "Job deleted";
struct all_jobs alljobs;
char *msg_delrunjobsig = "Job sent signal %s on delete";
struct server server;
int LOGLEVEL = 0;
char *msg_manager = "%s at request of %s@%s";


struct batch_request *alloc_br(int type)
  {
  fprintf(stderr, "The call to alloc_br needs to be mocked!!\n");
  exit(1);
  }

job_array *get_jobs_array(job *pjob)
  {
  fprintf(stderr, "The call to get_jobs_array needs to be mocked!!\n");
  exit(1);
  }

void account_record(int acctype, job *pjob, char *text)
  {
  fprintf(stderr, "The call to acctype needs to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save needs to be mocked!!\n");
  exit(1);
  }

void job_purge(job *pjob)
  {
  fprintf(stderr, "The call to job_purge needs to be mocked!!\n");
  exit(1);
  }

void chk_job_req_permissions(job **pjob_ptr, struct batch_request *preq)
  {
  fprintf(stderr, "The call to chk_job_req_permissions needs to be mocked!!\n");
  exit(1);
  }

void svr_mailowner(job *pjob, int mailpoint, int force, char *text)
  {
  fprintf(stderr, "The call to svr_mailowner needs to be mocked!!\n");
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

pbs_queue *get_jobs_queue(job *pjob)
  {
  fprintf(stderr, "The call to get_jobs_queue needs to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack needs to be mocked!!\n");
  exit(1);
  }

void free_nodes(node_info **ninfo_arr)
  {
  fprintf(stderr, "The call to free_nodes needs to be mocked!!\n");
  exit(1);
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br needs to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task needs to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject needs to be mocked!!\n");
  exit(1);
  }

job *next_job(struct all_jobs *aj, int *iter)
  {
  fprintf(stderr, "The call to next_job needs to be mocked!!\n");
  exit(1);
  }

void delete_task(struct work_task *ptask)
  {
  fprintf(stderr, "The call to delete_task needs to be mocked!!\n");
  exit(1);
  }

int svr_chk_owner(struct batch_request *preq, job *pjob)
  {
  fprintf(stderr, "The call to svr_chk_owner needs to be mocked!!\n");
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

void set_resc_assigned(job *pjob, enum batch_op op)
  {
  fprintf(stderr, "The call to set_resc_assigned needs to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req needs to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  exit(1);
  }

int relay_to_mom(job *pjob, struct batch_request *request, void (*func)(struct work_task *))
  {
  fprintf(stderr, "The call to relay_to_mom needs to be mocked!!\n");
  exit(1);
  }

 work_task *next_task(all_tasks *at, int *iter, int *mutex_inuse)
  {
  fprintf(stderr, "The call to next_task needs to be mocked!!\n");
  exit(1);
  }

struct batch_request *cpy_stage(struct batch_request *preq, job *pjob, enum job_atr ati, int direction)
  {
  fprintf(stderr, "The call to cpy_stage needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate needs to be mocked!!\n");
  exit(1);
  }

job *find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, char *id, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue needs to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job *pjob, int *newstate, int *newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate needs to be mocked!!\n");
  exit(1);
  }

int insert_task(all_tasks *at, work_task *wt)
  {
  fprintf(stderr, "The call to insert_task needs to be mocked!!\n");
  exit(1);
  }


char *threadsafe_tokenizer(char **str, char *delims)
  {
  fprintf(stderr, "The call to threadsafe_tokenizer needs to be mocked!!\n");
  exit(1);
  }
