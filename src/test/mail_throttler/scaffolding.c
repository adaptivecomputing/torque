
#include <stdio.h>
#include <string.h>

#include "pbs_job.h"


job::job() 
  {
  memset(this->ji_wattr, 0, sizeof(this->ji_wattr));
  memset(&this->ji_qs, 0, sizeof(this->ji_qs));
  }
job::~job() {}

int job::set_str_attr(int index, char *str)
  {
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  this->ji_wattr[index].at_val.at_str = str;
  return(PBSE_NONE);
  }

int job::set_resc_attr(int index, std::vector<resource> *resources)
  {
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  this->ji_wattr[index].at_val.at_ptr = resources;
  return(PBSE_NONE);
  }

const char *job::get_jobid() const
  {
  return(this->ji_qs.ji_jobid);
  }

const char *job::get_str_attr(int index) const
  {
  const char *str = NULL;

  if ((this->ji_wattr[index].at_flags & ATR_VFLAG_SET) != 0)
    str = this->ji_wattr[index].at_val.at_str;

  return(str);
  }

std::vector<resource> *job::get_resc_attr(int index)
  {
  std::vector<resource> *resc = NULL;

  if ((this->ji_wattr[index].at_flags & ATR_VFLAG_SET) != 0)
    resc = (std::vector<resource> *)this->ji_wattr[index].at_val.at_ptr;

  return(resc);
  }

const char *job::get_queue() const
  {
  return(this->ji_qs.ji_queue);
  }

bool job::is_attr_set(int index) const
  {
  return((this->ji_wattr[index].at_flags & ATR_VFLAG_SET) != 0);
  }

void job::set_queue(const char *queue)
  {
  snprintf(this->ji_qs.ji_queue, sizeof(this->ji_qs.ji_queue), "%s", queue);
  }

void job::set_jobid(const char *jid)
  {
  snprintf(this->ji_qs.ji_jobid, sizeof(this->ji_qs.ji_jobid), "%s", jid);
  }
