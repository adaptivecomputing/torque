#include <stdio.h>
#include <stdlib.h>

#include "pbs_job.h"

int kill_job(

  job        *pjob,              /* I */
  int         sig,               /* I */
  const char *killer_id_name,    /* I - process name of calling routine */
  const char *why_killed_reason) /* I - reason for killing */

  {
  return(0);
  }

int kill_task(

  job  *pjob,   /* I */
  task *ptask,  /* I */
  int   sig,    /* I */
  int   pg)     /* I (1=signal process group, 0=signal master process only) */

  {
  return(0);
  }

job *mom_find_job(

  const char *jobid)

  {
  return(NULL);
  }

task *task_find(

  job        *pjob,
  tm_task_id  taskid)

  {
  return(NULL);
  }

task *pbs_task_create(

  job        *pjob,
  tm_task_id  taskid)

  {
  return(NULL);
  }

