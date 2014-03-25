#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "pbs_job.h" /* job_recycler, all_jobs, job */
#include "threadpool.h"

threadpool_t *task_pool;
job_recycler recycler;
int          LOGLEVEL;


int enqueue_threadpool_request(void *(*func)(void *),void *arg, threadpool_t *tp)
  {
  fprintf(stderr, "The call to enqueue_threadpool_request to be mocked!!\n");
  exit(1);
  }

int insert_job(all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to insert_job to be mocked!!\n");
  exit(1);
  }

job *next_job(all_jobs *aj, all_jobs_iterator *iter)
  {
  fprintf(stderr, "The call to next_job to be mocked!!\n");
  exit(1);
  }

int remove_job(all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to remove_job to be mocked!!\n");
  exit(1);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int lock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int lock_alljobs_mutex(all_jobs *aj, const char *id, char *msg, int logging)
  {
  return(0);
  }

int unlock_alljobs_mutex(all_jobs *aj, const char *id, char *msg, int logging)
  {
  return(0);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

void free_job_allocation(job *pjob) {}
