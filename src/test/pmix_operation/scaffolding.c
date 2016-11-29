#include <stdio.h>
#include <stdlib.h>

#include "pbs_job.h"
#include "log.h"

char         mom_alias[PBS_MAXHOSTNAME + 1];
int          LOGLEVEL = 10;
char         log_buffer[LOG_BUF_SIZE];

// sensing variables
int notified = 0;
int killed_task = 0;

void log_event(

  int         eventtype,
  int         objclass,
  const char *objname,
  const char *text)

  {
  }

int kill_task(

  job  *pjob,   /* I */
  task *ptask,  /* I */
  int   sig,    /* I */
  int   pg)     /* I (1=signal process group, 0=signal master process only) */

  {
  killed_task++;
  return(0);
  }

task *task_find(

  job        *pjob,
  tm_task_id  taskid)

  {
  static task ptask;
  return(&ptask);
  }

void log_err(

  int         errnum,  /* I (errno or PBSErrno) */
  const char *routine, /* I */
  const char *text)    /* I */

  {
  }

void remote_notify_pmix_operation(
    
  job                *pjob, 
  const char         *remote_host,
  const std::string  &data,
  int                 pmix_op,
  struct sockaddr_in *dont_contact)

  {
  notified++;
  }

bool am_i_mother_superior(

  const job &pjob)

  {
  bool mother_superior = ((pjob.ji_nodeid == 0) && ((pjob.ji_qs.ji_svrflags & JOB_SVFLG_HERE) != 0));
    
  return(mother_superior);
  }

task::~task()
  {
  }
