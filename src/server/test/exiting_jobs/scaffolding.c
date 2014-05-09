#include <stdlib.h>
#include <stdio.h>

#include "pbs_job.h"
#include "exiting_jobs.h"
#include "batch_request.h"

bool exit_called = false;
int LOGLEVEL;

container::item_container<job_exiting_retry_info *>  exiting_jobs_info;


job *svr_find_job(char *jobid, int a)
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


int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void on_job_exit(batch_request *preq, char *jobid) {}

void force_purge_work(job *pjob) {}

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

