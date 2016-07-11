#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "job_func.h"
#include "work_task.h"
#include "threadpool.h"
#include "completed_jobs_map.h"

completed_jobs_map_class completed_jobs_map;

bool exit_called = false;
extern bool job_id_exists_rc;
int LOGLEVEL = 10;

threadpool_t *task_pool;

int svr_job_purge(struct job *pjob, int keepSpoolFiles)
  {
  return(0);
  }

job *svr_find_job(char const* p, int i)
  {
  return(NULL);
  }

void *handle_complete_second_time(work_task *wt)
  {
  return(NULL);
  }

bool job_id_exists(

  const std::string &job_id_string,
  int   *rcode)

  {
  return(job_id_exists_rc);
  }

int enqueue_threadpool_request(

  void         *(*func)(void *),
  void         *arg,
  threadpool_t *tp)

  {
  return(0);
  }

void log_event(

  int         eventtype,
  int         objclass,
  const char *objname,
  const char *text)

  {
  }
