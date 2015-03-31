#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "pbs_job.h" /* job_recycler, all_jobs, job */
#include "threadpool.h"

#define LOCAL_LOG_BUF_SIZE 5096

threadpool_t *task_pool;
job_recycler recycler;
int          LOGLEVEL = 10;
pthread_t     open_threads[10];
int           open_thread_count;


int enqueue_threadpool_request(void *(*func)(void *),void *arg, threadpool_t *tp)
  {
  pthread_t t;
  pthread_create(&t, NULL, func, NULL);
  open_threads[open_thread_count++] = t;

  return(0);
  }

int insert_job(all_jobs *aj, job *pjob)
  {
  int rc = PBSE_NONE;

  aj->lock();

  if(!aj->insert(pjob,pjob->ji_qs.ji_jobid))
    {
    rc = -1;
    }
    else
    {
    rc = PBSE_NONE;
    }
  aj->unlock();

  return(rc);
  }

job *next_job(all_jobs *aj, all_jobs_iterator *iter)
  {
  fprintf(stderr, "The call to next_job to be mocked!!\n");
  exit(1);
  }

int remove_job(all_jobs *aj, job *pjob, bool b)
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
void log_err(int objclass, const char *objname, const char *text) {}
void free_all_of_job(job *pjob) {}

job *job_alloc(void)
  {
  job *pj = (job *)calloc(1, sizeof(job));

  if (pj == NULL)
    {
    return(NULL);
    }

  pj->ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(pj->ji_mutex,NULL);
  lock_ji_mutex(pj, __func__, NULL, LOGLEVEL);

  pj->ji_qs.qs_version = PBS_QS_VERSION;

  pj->ji_is_array_template = FALSE;

  pj->ji_momhandle = -1;

  return(pj);
  }
