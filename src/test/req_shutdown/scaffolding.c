#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attribute_def */
#include "pbs_job.h" /* all_jobs, job */
#include "server.h" /* server */
#include "batch_request.h" /* batch_request */
#include "work_task.h" /* work_task */
#include "queue.h"
#include "u_tree.h"

all_queues              svr_queues;
int lockfds=0;
const char *msg_daemonname = "unset";
const char *msg_shutdown_start = "Starting to shutdown the server, type is ";
const char *msg_init_queued = "Requeued in queue: ";
const char *msg_on_shutdown = " on Server shutdown";
const char *msg_leftrunning = "job running on at Server shutdown";
const char *msg_shutdown_op = "Shutdown request from %s@%s ";
const char *msg_job_abort = "Aborted by PBS Server ";
attribute_def job_attr_def[10];
all_jobs alljobs;
struct server server;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
bool exit_called = false;
AvlTree ipaddrs = NULL;

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

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

job *next_job(all_jobs *aj, all_jobs_iterator *iter)
  {
  fprintf(stderr, "The call to next_job to be mocked!!\n");
  exit(1);
  }

int job_abt(struct job **pjobp, const char *text, bool b=false)
  {
  fprintf(stderr, "The call to job_abt to be mocked!!\n");
  exit(1);
  }

int issue_signal(job **pjob_ptr, const char *signame, void (*func)(batch_request *), void *extra, char *extend)
  {
  fprintf(stderr, "The call to issue_signal to be mocked!!\n");
  exit(1);
  }

char * csv_find_string(const char *csv_str, const char *search_str)
  {
  fprintf(stderr, "The call to csv_find_string to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  fprintf(stderr, "The call to find_job to be mocked!!\n");
  exit(1);
  }

int svr_save(struct server *ps, int mode)
  {
  fprintf(stderr, "The call to svr_save to be mocked!!\n");
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

void free_br(struct batch_request *b) {}

int set_svr_attr (int index, void *v)
  {
  return(0);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *id, const char *msg, int logging)
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

int que_save(pbs_queue *pque)

  {
  return(0);
  }

pbs_queue *next_queue(all_queues *aq, all_queues_iterator        *iter)
  {
  return(NULL);
  }

void log_event(int type, int otype, const char *func_name, const char *msg) {}

int relay_to_mom(job **pjob_ptr, batch_request   *request, void (*func)(struct work_task *))
  {
  return(0);
  }

AvlTree AVL_clear_tree(AvlTree a)
  {
  fprintf(stderr, "The call to AVL_clear_tree needs to be mocked!!\n");
  exit(1);
  }
