
#ifndef JOB_HOST_DATA_HPP
#define JOB_HOST_DATA_HPP

#include <string>

class job_host_data
  {
  std::string hostname;
  int         allocated_execution_slots;
  int         current_execution_slot_usage;

  public:
  job_host_data(
      
    const std::string &host,
    int                max_slots) : hostname(host), allocated_execution_slots(max_slots),
                                    current_execution_slot_usage(0) 

    {
    }

  job_host_data() : hostname(), allocated_execution_slots(0), current_execution_slot_usage(0)
    {
    }

  void decrement_usage(int to_decrement)
    {
    this->current_execution_slot_usage -= to_decrement;
    }

  void increment_usage(int to_increment)
    {
    this->current_execution_slot_usage += to_increment;
    }

  bool can_fit(int potential_to_use) const
    {
    return(this->allocated_execution_slots - this->current_execution_slot_usage >= potential_to_use);
    }

  const char *get_hostname() const
    {
    return(this->hostname.c_str());
    }
  };

#endif

