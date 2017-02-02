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

svr_job *svr_find_job(const char *jobid, int a)
  {
  static svr_job pjob;

  if (!strcmp(jobid, "1.napali"))
    {
    pjob.set_jobid("1.napali");
    return(&pjob);
    }
  else if (!strcmp(jobid, "5.napali"))
    {
    pjob.set_jobid("5.napali");
    pjob.set_state(JOB_STATE_COMPLETE);
    return(&pjob);
    }
  else
    return(NULL);
  }

svr_job *svr_find_job_by_id(int id)
  {
  static svr_job pjob;

  if (id == 1)
    {
    pjob.set_jobid("1.napali");
    return(&pjob);
    }
  else if (id == 5)
    {
    pjob.set_jobid("5.napali");
    pjob.set_state(JOB_STATE_COMPLETE);
    return(&pjob);
    }

  return(NULL);
  }


int unlock_ji_mutex(svr_job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void on_job_exit(batch_request *preq, char *jobid) {}

void force_purge_work(svr_job *pjob) {}

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

int job::get_state() const
  {
  return(this->ji_qs.ji_state);
  }

void job::set_jobid(const char *jobid)
  {
  strcpy(this->ji_qs.ji_jobid, jobid);
  }

void job::set_state(int state)
  {
  this->ji_qs.ji_state = state;
  }

job::job() 
  {
  memset(&this->ji_qs, 0, sizeof(this->ji_qs));
  memset(this->ji_wattr, 0, sizeof(this->ji_wattr));
  }
job::~job() {}
svr_job::svr_job() {}
svr_job::~svr_job() {}
