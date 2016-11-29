#include <stdlib.h>
#include <stdio.h>

#include "pbs_job.h"
#include "log.h"
#include "id_map.hpp"
#include "log.h"

bool exit_called = false;
all_jobs        array_summary;
char                   server_name[PBS_MAXSERVERNAME + 1]; /* host_name[:service|port] */
all_jobs        alljobs;
int                    LOGLEVEL;

void log_event(int type, int cls, const char *ident, const char *msg) {}

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  return(0);
  }

int lock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int is_svr_attr_set(int index)
  {
  return(0);
  }

int get_svr_attr_str(int index, char **str)
  {
  return(0);
  }

job *job_alloc(void)
  {
  return(new job());
  }

id_map::id_map() 
  {
  }

id_map::~id_map() 
  {
  }

const char *id_map::get_name(int id)
  {
  return(NULL);
  }

job::job() {}
job::~job() {}
