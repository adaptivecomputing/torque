#include <stdlib.h>
#include <stdio.h>

#include "hash_map.h"
#include "pbs_job.h"
#include "exiting_jobs.h"
#include "batch_request.h"
#include "hash_table.h"

int LOGLEVEL;
hash_map *exiting_jobs_info;

int add_to_hash_map(hash_map *hm, void *obj, char *key)
  {
  if (!strcmp(key, "1.tom"))
    return(0);
  else
    return(-1);
  }

void *get_remove_from_hash_map(hash_map *hm, char *key)
  {
  if (!strcmp(key, "2.napali"))
    return(calloc(1, sizeof(job_exiting_retry_info)));
  else
    return(NULL);
  }


int remove_from_hash_map(hash_map *hm, char *key)
  {
  if (!strcmp(key, "3.napali"))
    return(PBSE_NONE);
  else
    return(KEY_NOT_FOUND);
  }

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


void *next_from_hash_map(hash_map *hm, int *iter)
  {
  job_exiting_retry_info *jeri;
    
  if (*iter < 2)
    {
    jeri = calloc(1, sizeof(job_exiting_retry_info));

    if (*iter == -1)
      snprintf(jeri->jobid, sizeof(jeri->jobid), "1.napali");
    else if (*iter == 0)
      snprintf(jeri->jobid, sizeof(jeri->jobid), "5.napali");
    else if (*iter == 1)
      snprintf(jeri->jobid, sizeof(jeri->jobid), "%d.napali", 10+*iter);

    (*iter)++;
    return(jeri);
    }
  else
    return(NULL);
  }

int unlock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
  {
  return(0);
  }

void on_job_exit(batch_request *preq, char *jobid) {}

void force_purge_work(job *pjob) {}

void log_event(int eventtype, int objclass, const char *objname, char *text) {}
