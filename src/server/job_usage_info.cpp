
#include <stdio.h>

#include "pbs_nodes.h"

job_usage_info::job_usage_info(const char *id)
  {
  snprintf(this->jobid, sizeof(this->jobid), "%s", id);
  }

bool job_usage_info::operator ==(

  const job_usage_info &jui)

  {
  if (!strcmp(jui.jobid, this->jobid))
    return(true);
  else
    return(false);
  }
