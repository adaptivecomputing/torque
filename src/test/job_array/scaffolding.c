#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "array.h"
#include "pbs_job.h"
#include "server.h"
#include "pbs_error.h"
#include "mutex_mgr.hpp"

long max_slot = NO_SLOT_LIMIT;
long idle_max_slot = 400;
long array_size;
bool unset;

int svr_setjobstate(

  job *pjob,            /* I (modified) */
  int  newstate,        /* I */
  int  newsubstate,     /* I */
  int  has_queue_mutex) /* I */

  {
  return(PBSE_NONE);
  }

void log_event(

  int         eventtype,
  int         objclass,
  const char *objname,
  const char *text)

  {
  }

bool set_array_depend_holds(

  job_array *pa)

  {
  return(false);
  }

job::job() {}
job::~job() {}

job *svr_find_job(

  const char *jobid,      /* I */
  int         get_subjob) /* I */

  {
  job *pjob = new job();

  return(pjob);
  }

int array_save(
    
  job_array *pa)

  {
  return(PBSE_NONE);
  }

int translate_range_string_to_vector(

  const char       *range_string,
  std::vector<int> &indices)

  {
  if (range_string == NULL)
    return(-1);

  if (!isdigit(*range_string))
    return(-1);

  indices.clear();
  for (int i = 0; i < 10; i++)
    indices.push_back(i);

  return(PBSE_NONE);
  }

int job_save(

  job *pjob,  /* pointer to job structure */
  int  updatetype, /* 0=quick, 1=full, 2=new     */
  int  mom_port)   /* if 0 ignore otherwise append to end of job name. this is for multi-mom mode */

  {
  return(PBSE_NONE);
  }

int get_svr_attr_l(

  int   attr_index,
  long *l)

  {
  if (unset == true)
    return(-1);

  if (attr_index == SRV_ATR_MaxSlotLimit)
    *l = max_slot;
  else if (attr_index == SRV_ATR_IdleSlotLimit)
    *l = idle_max_slot;
  else if (attr_index == SRV_ATR_MaxArraySize)
    *l = array_size;

  return(PBSE_NONE);
  }

int get_svr_attr_b(int index, bool *b)
  {
  return(0);
  }

int create_and_queue_array_subjob(
    
  job_array   *pa,
  mutex_mgr   &array_mgr,
  job         *template_job,
  mutex_mgr   &template_job_mgr,
  int          index,
  std::string &prev_job_id,
  bool         place_hold)

  {
  char buf[1024];

  sprintf(buf, "%d.roshar", index);
  pa->job_ids[index] = strdup(buf);

  return(PBSE_NONE);
  }
