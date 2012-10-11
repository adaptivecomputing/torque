#include <stdlib.h>
#include <stdio.h>

#include "hash_map.h"
#include "pbs_job.h"

int add_to_hash_map(hash_map *hm, void *obj, char *key)
  {
  return(0);
  }

void *get_remove_from_hash_map(hash_map *hm, char *key)
  {
  static job pjob;

  strcpy(pjob->ji_qs.ji_jobid, "1.napali");

  if (!strcmp(key, pjob->ji_qs.ji_jobid))
    return(&pjob);
  else
    return(NULL);
  }


int remove_from_hash_map(hash_map *hm, char *key)
  {
  return(PBSE_NONE);
  }

job *svr_find_job(char *jobid, int a)
  {
  static job pjob;

  strcpy(pjob->ji_qs.ji_jobid, "1.napali");

  if (!strcmp(jobid, "1.napali"))
    return(&pjob);
  else
    return(NULL);
  }


void *next_from_hash_map(hash_map *hm, int *iter)
  {
  static job_exiting_retry_info jeri;

  if (*iter < 2)
    {
    snprintf(jeri->jobid, sizeof(jeri->jobid), "%d.napali", 10+*iter);
    *iter++;
    return(&jeri);
    }
  else
    return(NULL);
  }

int unlock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
  {
  return(0);
  }

