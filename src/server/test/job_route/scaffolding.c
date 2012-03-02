#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "pbs_job.h" /* job */
#include "list_link.h" /* list_link, tlist_head */
#include "queue.h" /* pbs_queue */
#include "batch_request.h" /* batach_request */
#include "resource.h"

char *msg_err_malloc = "malloc failed";
char *msg_routexceed = "Route queue lifetime exceeded";
int svr_resc_size = 0;
resource_def *svr_resc_def;

int svr_movejob(job *jobp, char *destination, struct batch_request *req)
  {
  fprintf(stderr, "The call to svr_movejob needs to be mocked!!\n");
  exit(1);
  }

job *next_job(struct all_jobs *aj, int *iter)
  {
  fprintf(stderr, "The call to next_job needs to be mocked!!\n");
  exit(1);
  }

 int job_abt(job **pjobp, char *text)
  {
  fprintf(stderr, "The call to job_abt needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
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

void append_link(tlist_head *head, list_link *new, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, const char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, const char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

resource *add_resource_entry(pbs_attribute *pattr, resource_def *prdef)
  {
  return(NULL);
  }

long count_proc(char *param_spec)

  {
  return(0);
  }

resource_def *find_resc_def(resource_def *rscdf, char *name, int limit) 
  {
  return(NULL);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  return(NULL);
  }

int enqueue_threadpool_request(void *(*func)(void *), void *arg)
  {
  fprintf(stderr, "The call to enqueue_threadpool_request needs to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_jobs_queue(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_queue needs to be mocked!!\n");
  exit(1);
  }


int unlock_queue(struct pbs_queue *the_queue, const char *method_name, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue needs to be mocked!!\n");
  exit(1);
  }

job *find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }
