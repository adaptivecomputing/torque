#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "numa_node.hpp"


int  reserve_called;
int  remove_called;
int  get_job_indices_called;
bool does_it_fit;
int  recover_mode;
int  recover_called;


numa_node::numa_node(const char *path, int index)
  {
  this->my_index = index;
  }

bool numa_node::completely_fits(int cpus, unsigned long memory) const
  {
  return(does_it_fit);
  }

void numa_node::recover_reservation(int cpus, unsigned long memory, const char *jobid, allocation &a)
  {
  recover_called++;

  if (recover_mode == 0)
    {
    a.cpus = cpus;
    a.memory = memory;
    }
  else if (recover_mode == 1)
    {
    a.cpus = cpus / 2;
    a.memory = memory / 2;
    }
  }

numa_node::numa_node(

  const numa_node &nn) : total_cpus(nn.total_cpus), total_memory(nn.total_memory),
                         available_memory(nn.available_memory), available_cpus(nn.available_cpus),
                         my_index(nn.my_index), cpu_indices(nn.cpu_indices), cpu_avail(nn.cpu_avail),
                         allocations(nn.allocations)

  {
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

void numa_node::reserve(
    
  int            num_cpus,
  unsigned long  memory,
  const char    *jobid,
  allocation    &alloc)

  {
  reserve_called++;
  }


void numa_node::get_job_indices(

  const char       *jobid,
  std::vector<int> &indices,
  bool              cpus) const

  {
  get_job_indices_called++;
  }

void numa_node::remove_job(const char *jobid)
  {
  remove_called++;
  }
