#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include "machine.hpp"
#include "log.h"
#include "pbs_error.h"
#include "allocation.hpp"
#include "complete_req.hpp"

int num_for_host;
int num_tasks_fit;
int num_placed;
int hardware_style;
int my_req_count;
int called_free_task;
int called_place_task;

char mom_alias[1024];

const int MEM_INDICES = 1;
const int CPU_INDICES = 1;

void log_err(int errnum, const char *routine, const char *text)
  {
  }

Socket::Socket()
  {
  }

Socket::~Socket()
  {
  }

int Socket::initializeAMDSocket(hwloc_obj_t socket_obj, hwloc_topology_t topology)
  {
  return(PBSE_NONE);
  }

int Socket::initializeNonNUMASocket(hwloc_obj_t obj, hwloc_topology_t topology)
  {
  return(PBSE_NONE);
  }

int Socket::initializeIntelSocket(hwloc_obj_t obj, hwloc_topology_t topology)
  {
  return(PBSE_NONE);
  }

Core::~Core()
  {
  }

Chip::~Chip()
  {
  }

PCI_Device::~PCI_Device()
  {
  }

void Socket::displayAsString(std::stringstream &output) const {}

bool Socket::free_task(const char *jobid)
  {
  called_free_task++;
  return(true);
  }

int Socket::place_task(const char *jobid, const req &r, allocation &a, int to_place)
  {
  called_place_task++;
  return(num_placed);
  }

int Socket::getAvailableChips()
  {
  return(1);
  }

int Socket::how_many_tasks_fit(const req &r)

  {
  return(num_tasks_fit);
  }

int Socket::getAvailableThreads()
  {
  return(1);
  }

int Socket::getAvailableCores()
  {
  return(0);
  }

bool Socket::is_available() const
  {
  return(false);
  }

complete_req::complete_req() {}

int complete_req::req_count() const
  {
  return(my_req_count);
  }

const req &complete_req::get_req(int index) const
  {
  static req r;

  return(r);
  }

req::req() {}
int req::get_num_tasks_for_host(

  const std::string &hostname) const

  {
  return(num_for_host);
  }


allocation::allocation(const allocation &other) {}
allocation::allocation() {}
void allocation::place_indices_in_string(std::string &out, int which) {}

void PCI_Device::displayAsString(std::stringstream &out) const {}
