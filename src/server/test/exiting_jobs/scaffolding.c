#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "pbs_job.h"
#include "exiting_jobs.h"
#include "batch_request.h"
#include "id_map.hpp"

bool exit_called = false;
int LOGLEVEL;

pthread_mutex_t *exiting_jobs_info_mutex;

job *svr_find_job(const char *jobid, int a)
  {
  static job pjob;

  if (!strcmp(jobid, "1.napali"))
    {
    strcpy(pjob.ji_qs.ji_jobid, "1.napali");
    return(&pjob);
    }
  else if (!strcmp(jobid, "5.napali"))
    {
    strcpy(pjob.ji_qs.ji_jobid, "5.napali");
    pjob.ji_qs.ji_state = JOB_STATE_COMPLETE;
    return(&pjob);
    }
  else
    return(NULL);
  }

job *svr_find_job_by_id(int id)
  {
  static job pjob;

  if (id == 1)
    {
    strcpy(pjob.ji_qs.ji_jobid, "1.napali");
    return(&pjob);
    }
  else if (id == 5)
    {
    strcpy(pjob.ji_qs.ji_jobid, "5.napali");
    pjob.ji_qs.ji_state = JOB_STATE_COMPLETE;
    return(&pjob);
    }

  return(NULL);
  }


int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void on_job_exit(batch_request *preq, char *jobid) {}

void force_purge_work(job *pjob) {}

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

id_map::id_map() 
  {
  }

id_map::~id_map() 
  {
  }

const char *id_map::get_name(int id)
  {
  if (id == 1)
    return("1.napali");
  else if (id == 5)
    return("5.napali");
  else if (id == -1)
    return(NULL);
  else
    {
    char buf[100];
    sprintf(buf, "%d.napali", id);
    return(strdup(buf));
    }
  }

id_map job_mapper;
