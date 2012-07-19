#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */
#include "server.h" /* server */
#include "attribute.h" /* svrattrl */
#include "list_link.h" /* tlist_head, list_link */
#include "array.h" /* job_array */
#include "work_task.h" /* work_task */
#include "queue.h"

char *msg_illregister = "Illegal op in register request received for job %s";
char *msg_registerdel = "Job deleted as result of dependency on job %s";
char server_name[PBS_MAXSERVERNAME + 1];
char *msg_err_malloc = "malloc failed";
const char *PJobState[] = {"hi", "hello"};
struct server server;
int LOGLEVEL = 0;
char *msg_regrej = "Dependency request for job rejected by ";
char *msg_registerrel = "Dependency on job %s released.";



struct batch_request *alloc_br(int type)
  {
  fprintf(stderr, "The call to alloc_br to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save to be mocked!!\n");
  exit(1);
  }

void svr_mailowner(job *pjob, int mailpoint, int force, char *text)
  {
  fprintf(stderr, "The call to svr_mailowner to be mocked!!\n");
  exit(1);
  }

svrattrl *attrlist_create(char *aname, char *rname, int vsize)
  {
  fprintf(stderr, "The call to attrlist_create to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link to be mocked!!\n");
  exit(1);
  }

long calc_job_cost(job *pjob)
  {
  fprintf(stderr, "The call to calc_job_cost to be mocked!!\n");
  exit(1);
  }

job_array *get_array(char *id)
  {
  fprintf(stderr, "The call to get_array to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

int svr_chk_owner(struct batch_request *preq, job *pjob)
  {
  fprintf(stderr, "The call to svr_chk_owner to be mocked!!\n");
  exit(1);
  }

int job_abt(job **pjobp, char *text)
  {
  fprintf(stderr, "The call to job_abt to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new, void *pobj)
  {
  fprintf(stderr, "The call to append_link to be mocked!!\n");
  exit(1);
  }

int issue_to_svr(char *servern, struct batch_request *preq, void (*replyfunc)(struct work_task *))
  {
  fprintf(stderr, "The call to issue_to_svr to be mocked!!\n");
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

char *parse_comma_string(char *start, char **ptr) 
  {
  fprintf(stderr, "The call to parse_comma_string to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

pbs_queue *get_jobs_queue(job **pjob)
  {
  return((*pjob)->ji_qhdr);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *id, char *msg, int logging)
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

void free_br(batch_request *preq) {} 

int unlock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
  {
  return(0);
  }
