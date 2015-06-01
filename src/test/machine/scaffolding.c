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
int called_partially_place;
int called_fits_on_socket;
int called_store_pci;
int json_socket;
bool socket_fit;
bool partially_placed;

char mom_alias[1024];

const int MEM_INDICES = 1;
const int CPU_INDICES = 1;

void log_err(int errnum, const char *routine, const char *text)
  {
  }

Socket::Socket(const std::string &json_layout)
  {
  json_socket++;
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

bool Socket::store_pci_device_appropriately(PCI_Device &d, bool force)
  {
  called_store_pci++;
  return(false);
  }

Core::~Core()
  {
  }

Chip::~Chip()
  {
  }

void Socket::displayAsString(std::stringstream &output) const {}

bool Socket::free_task(const char *jobid)
  {
  called_free_task++;
  return(true);
  }

bool Socket::fits_on_socket(const allocation &remaining) const
  {
  called_fits_on_socket++;
  return(socket_fit);
  }

bool Socket::partially_place(allocation &remaining, allocation &master)
  {
  called_partially_place++;
  return(partially_placed);
  }

int Socket::place_task(const char *jobid, const req &r, allocation &a, int to_place)
  {
  called_place_task++;
  return(num_placed);
  }

int Socket::getTotalChips() const
  {
  return(1);
  }

int Socket::getAvailableChips() const
  {
  return(1);
  }

int Socket::how_many_tasks_fit(const req &r, int place_type) const

  {
  return(num_tasks_fit);
  }

int Socket::getAvailableThreads() const
  {
  return(1);
  }

int Socket::getTotalThreads() const
  {
  return(1);
  }

int Socket::getTotalCores() const
  {
  return(1);
  }

void Socket::update_internal_counts(vector<allocation> &allocs) {}

int Socket::getAvailableCores() const
  {
  return(0);
  }

bool Socket::is_available() const
  {
  return(false);
  }

void Socket::displayAsJson(std::stringstream &out, bool jobs) const {}

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

void complete_req::set_hostlists(const char *job_id, const char *hostlists)
  {
  }

complete_req::complete_req(list_link &l) {}

req::req() {}
req::req(const req &other) {}

req &req::operator =(const req &other)
  {
  return(*this);
  }

int req::get_num_tasks_for_host(

  const std::string &name) const

  {
  return(num_for_host);
  }

std::string req::getPlacementType() const
  {
  return("");
  }


allocation::allocation(const char *jid)
  {
  }
allocation::allocation(const allocation &other) {}
allocation::allocation() {}
allocation::allocation(const req &r) {}
void allocation::set_place_type(const std::string &place) {}
void allocation::place_indices_in_string(std::string &out, int which) {}

PCI_Device::~PCI_Device() {}
PCI_Device::PCI_Device() {}
PCI_Device::PCI_Device(const PCI_Device &other) {}

void PCI_Device::displayAsString(std::stringstream &out) const {}
PCI_Device &PCI_Device::operator=(const PCI_Device &other)
  {
  return(*this);
  }
