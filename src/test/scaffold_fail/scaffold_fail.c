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

pbs_queue *get_jobs_queue(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_queue needs to be mocked!!\n");
  exit(1);
  }


int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue needs to be mocked!!\n");
  exit(1);
  }


job *svr_find_job(char *jobid, int get_subjob)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

