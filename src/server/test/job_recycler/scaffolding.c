#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <errno.h>

#include "pbs_job.h" /* job_recycler, all_jobs, job */
#include "resizable_array.h"

job_recycler recycler;
int          LOGLEVEL;


#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))
#define mix(a,b,c) \
  { \
    a -= c;  a ^= rot(c, 4);  c += b; \
    b -= a;  b ^= rot(a, 6);  a += c; \
    c -= b;  c ^= rot(b, 8);  b += a; \
    a -= c;  a ^= rot(c,16);  c += b; \
    b -= a;  b ^= rot(a,19);  a += c; \
    c -= b;  c ^= rot(b, 4);  b += a; \
  }
#define final(a,b,c) \
  { \
    c ^= b; c -= rot(b,14); \
    a ^= c; a -= rot(c,11); \
    b ^= a; b -= rot(a,25); \
    c ^= b; c -= rot(b,16); \
    a ^= c; a -= rot(c,4);  \
    b ^= a; b -= rot(a,14); \
    c ^= b; c -= rot(b,24); \
  }


int enqueue_threadpool_request(void *(*func)(void *),void *arg)
  {
  fprintf(stderr, "The call to enqueue_threadpool_request to be mocked!!\n");
  exit(1);
  }


int insert_job(struct all_jobs *aj, job *pjob)
  {
  pthread_mutex_lock(aj->alljobs_mutex);

  int rc = insert_thing(aj->ra,pjob);
  if (rc == -1)
    {
    rc = -1;
    }
  else
    {
    add_hash(aj->ht, rc, pjob->ji_qs.ji_jobid);
    rc = PBSE_NONE;
    }

  pthread_mutex_unlock(aj->alljobs_mutex);

  return(rc);
  } 



job *next_job(struct all_jobs *aj, int *iter)
  {
  fprintf(stderr, "The call to next_job to be mocked!!\n");
  exit(1);
  }


void initialize_all_jobs_array(struct all_jobs *aj)
  {
  aj->ra = initialize_resizable_array(INITIAL_JOB_SIZE);
  aj->ht = create_hash(INITIAL_HASH_SIZE);

  aj->alljobs_mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(aj->alljobs_mutex, NULL);
  }

int remove_job(struct all_jobs *aj, job *pjob)
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

int lock_alljobs_mutex(struct all_jobs *aj, const char *id, char *msg, int logging)
  {
  return(0);
  }

int unlock_alljobs_mutex(struct all_jobs *aj, const char *id, char *msg, int logging)
  {
  return(0);
  }


void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_err(int objclass, const char *objname, const char *text) {}

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

  CLEAR_HEAD(pj->ji_rejectdest);
  pj->ji_is_array_template = FALSE;

  pj->ji_momhandle = -1;

  return(pj);
  }
