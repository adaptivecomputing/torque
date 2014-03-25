#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <errno.h>

#include "work_task.h" /* all_tasks, work_task */
#include "threadpool.h"
#include <signal.h>
#include <string.h>

#define FALSE 0
all_tasks             task_list_event;
task_recycler         tr;
pthread_mutex_t       task_list_timed_mutex;

threadpool_t         *task_pool;
threadpool_t         *request_pool;

sigset_t      fillset;

int create_work_thread()
  {
  return(0);
  }

int enqueue_threadpool_request(void *(*func)(void *),void *arg, threadpool_t *tp)
  {
  return 5;
  }

void check_nodes(struct work_task *ptask)
  {
  fprintf(stderr, "The call to check_nodes to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, const char *func_id, const char *msg) {}


int initialize_threadpool(

  threadpool_t **pool,           /* I/O */
  int            min_threads,    /* I */
  int            max_threads,    /* I */
  int            max_idle_time)  /* I */

  {
  int           i;
  int           rc = 0;

  sigfillset(&fillset);

  if ((min_threads > max_threads) ||
      (max_threads < 1))
    {
    /* bad parameters */
    return(EINVAL);
    }

  if ((*pool = (threadpool_t *)calloc(1, sizeof(threadpool_t))) == NULL)
    {
    return(ENOMEM);
    }

  memset(*pool,0,sizeof(threadpool_t));
  pthread_mutex_init(&(*pool)->tp_mutex,NULL);
  pthread_cond_init(&(*pool)->tp_waiting_work,NULL);
  pthread_cond_init(&(*pool)->tp_can_destroy,NULL);
  (*pool)->tp_min_threads = min_threads;
  (*pool)->tp_max_threads = max_threads;
  (*pool)->tp_max_idle_secs = max_idle_time;
  (*pool)->tp_started = FALSE;

  /* initialize attributes */
  if ((rc = pthread_attr_init(&(*pool)->tp_attr)) != 0)
    {
    perror("pthread_attr_init failed. Could not init thread pool.");
    log_err(-1, __func__, "pthread_attr_init failed. Could not init thread pool.");
    return rc;
    }

  pthread_attr_setdetachstate(&(*pool)->tp_attr,PTHREAD_CREATE_DETACHED);

  /* if threads are static, create them all now */
  if ((*pool)->tp_max_idle_secs < 0)
    {
    for (i = 0; i < (*pool)->tp_min_threads; i++)
      {
      if ((rc = create_work_thread()) != 0)
        {
        return(rc);
        }

      (*pool)->tp_nthreads++;
      }
    }

  return(rc);
  } /* END initialize_threadpool() */


void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int type, const char *func_name, const char *msg, int o) {}
