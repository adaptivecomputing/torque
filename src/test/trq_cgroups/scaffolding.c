#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <sys/ioctl.h> /* winsize */
#include <termios.h> /* termios */

#include "pbs_ifl.h" /* PBS_MAXHOSTNAME */
#include "log.h" /* LOG_BUF_SIZE */
#include "pbs_job.h" /* job, task */
#include "complete_req.hpp"
#include "machine.hpp"
#include "numa_node.hpp"

bool global_trq_cg_signal_tasks_ok = false;

char mom_alias[1];

char log_buffer[LOG_BUF_SIZE];

bool per_task;

int is_login_node;

int LOGLEVEL = 0;

Machine::Machine() {}
Machine::~Machine() {}

PCI_Device::~PCI_Device() {}

Socket::~Socket() {}

Chip::~Chip() {}

Core::~Core() {}

Machine this_node;

req::req() {}

bool req::is_per_task() const

  {
  return(per_task);
  }

void req::get_task_host_name(

  std::string &task_host,
  unsigned int task_index)

  {}
   
int Machine::getTotalChips() const
  {
  return(0);
  }

int Machine::getTotalThreads() const
  {
  return(0);
  }

bool task_hosts_match(const char *one, const char *two)
  {
  return(true);
  }

allocation::allocation(

  const allocation &alloc) : cpu_indices(alloc.cpu_indices), memory(alloc.memory), cpus(alloc.cpus)

  {
  this->jobid = alloc.jobid;
  }

allocation::allocation() : cpu_indices(), memory(0), cpus(0) 

  {
  this->jobid[0] = '\0'; 
  }

allocation &allocation::operator =(const allocation &other) 
  {
  return(*this);
  }

void log_err(int errnum, const char *routine, const char *text) {}

int rmdir_ext(const char *dirname, int retry_limit)
  {
  return(0);
  }

void get_device_indices(
  
  const char *device_str, 
  std::vector<int> &device_indices, 
  const char *suffix)

  {}

int complete_req::req_count() const
  {
  return(0);
  }

bool have_incompatible_dash_l_resource(
    
    pbs_attribute *pattr) 

  {
  return(false);
  }

int req::get_task_allocation(

  unsigned int  index,  
  allocation &task_allocation) const

  {
  return(PBSE_NONE);
  }

int req::get_req_allocation_count()

  {
  return(0);
  }

unsigned int complete_req::get_num_reqs()
  {
  return(1);
  }

req &complete_req::get_req(int i)
  {
  static req r;

  return(r);
  }

int req::getTaskCount() const

  {
  return(0);
  }

void log_event(

  int         eventtype,
  int         objclass,
  const char *objname,
  const char *text)

  {
  if ((objname != NULL) && (strcmp(objname, "trq_cg_signal_tasks") == 0))
    {
    if ((text != NULL) && (strcmp(text, "sending signal 15 to pid 9999 in cpuset .") == 0))
      global_trq_cg_signal_tasks_ok = true;
    }
  }

void translate_range_string_to_vector(

  const char       *range_string,
  std::vector<int> &indices)

  {
  }

