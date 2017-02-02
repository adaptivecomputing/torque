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

int lock_ji_mutex(svr_job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int unlock_ji_mutex(svr_job *pjob, const char *id, const char *msg, int logging)
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

svr_job *job_alloc(void)
  {
  return(new svr_job());
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

pbs_net_t job::get_ji_momaddr() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_momaddr);
  }

unsigned short job::get_ji_momport() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_momport);
  }

void job::set_jobid(const char *jobid)
  {
  strcpy(this->ji_qs.ji_jobid, jobid);
  }

int job::get_attr_flags(int index) const
  {
  return(this->ji_wattr[index].at_flags);
  }

const char *job::get_str_attr(int index) const
  {
  return(this->ji_wattr[index].at_val.at_str);
  }

const char *job::get_jobid() const
  {
  return(this->ji_qs.ji_jobid);
  }

int job::get_substate() const
  {
  return(this->ji_qs.ji_substate);
  }

int job::get_state() const
  {
  return(this->ji_qs.ji_state);
  }

void job::unset_attr(int index)
  {
  this->ji_wattr[index].at_flags = 0;
  }

bool job::is_attr_set(int index) const
  {
  return((this->ji_wattr[index].at_flags & ATR_VFLAG_SET) != 0);
  }

const char *job::get_fileprefix() const
  {
  return(this->ji_qs.ji_fileprefix);
  }

int job::set_long_attr(int index, long l)
  {
  this->ji_wattr[index].at_val.at_long = l;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

int job::set_str_attr(int index, char *str)
  {
  this->ji_wattr[index].at_val.at_str = str;
  return(PBSE_NONE);
  }

long job::get_long_attr(int index) const
  {
  return(this->ji_wattr[index].at_val.at_long);
  }

time_t job::get_start_time() const
  {
  return(this->ji_qs.ji_stime);
  }

pbs_attribute *job::get_attr(int index)
  {
  return(this->ji_wattr + index);
  }

job::job() 
  {
  memset(&this->ji_qs, 0, sizeof(this->ji_qs));
  memset(this->ji_wattr, 0, sizeof(this->ji_wattr));
  }
job::~job() {}
svr_job::svr_job() {}
svr_job::~svr_job() {}
