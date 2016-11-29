
#include <stdio.h>

#include "job_usage_info.hpp"

job_usage_info::job_usage_info(
    
  int internal_jid) : internal_job_id(internal_jid), est()

  {
  }



bool job_usage_info::operator ==(

  const job_usage_info &jui)

  {
  if (this->internal_job_id == jui.internal_job_id)
    return(true);
  else
    return(false);
  }



job_usage_info &job_usage_info::operator =(
    
  const job_usage_info &other_jui)

  {
  if (this == &other_jui)
    return(*this);

  this->internal_job_id = other_jui.internal_job_id;
  this->est = other_jui.est;

  return(*this);
  }



job_reservation_info::job_reservation_info(
    
  const job_reservation_info &other) : node_id(other.node_id), port(other.port), est(other.est)

  {
  }



job_reservation_info::job_reservation_info() : node_id(0), port(0), est()

  {
  }



job_reservation_info &job_reservation_info::operator =(
    
  const job_reservation_info &other)

  {
  if (this == &other)
    return(*this);

  this->node_id = other.node_id;
  this->port = other.port;
  this->est = other.est;
    
  return(*this);
  }


