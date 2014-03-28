
#include <stdio.h>

#include "pbs_nodes.h"

job_usage_info::job_usage_info(int internal_job_id)
  {
  this->internal_job_id = internal_job_id;
  }

bool job_usage_info::operator ==(

  const job_usage_info &jui)

  {
  if (this->internal_job_id == jui.internal_job_id)
    return(true);
  else
    return(false);
  }
