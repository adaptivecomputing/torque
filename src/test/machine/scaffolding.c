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
int my_core_count;
int called_free_task;
int called_place_task;
int called_partially_place;
int called_fits_on_socket;
int called_store_pci;
int called_spread_place;
int called_spread_place_cores;
int json_socket;
int sockets;
int numa_node_count;
int exec_slots;
int placed_all;
const int exclusive_socket = 2;
const int exclusive_node = 1;
const int exclusive_chip = 3;
const int exclusive_core = 4;
const int exclusive_thread = 4;
bool socket_fit;
bool partially_placed;
bool spreaded = true;
int my_placement_type;
int req_mem;
int sock_mem;

char mom_alias[1024];

const int MEM_INDICES = 1;
const int CPU_INDICES = 1;
const char *use_cores = "usecores";
const int ALL_EXECUTION_SLOTS = -1;
const char *place_legacy = "legacy";
const char *place_legacy2 = "legacy2";
const int exclusive_legacy = 6;
const int exclusive_legacy2 = 7;

void log_err(int errnum, const char *routine, const char *text)
  {
  }

Socket::Socket(int np, int numa_nodes, int &es_remainder)
  {
  }

void Socket::setMemory(hwloc_uint64_t mem)
  {
  this->memory = mem;
  }

hwloc_uint64_t Socket::getMemory() const
  {
  return(sock_mem);
  }

hwloc_uint64_t Socket::get_memory_for_completely_free_chips(

  unsigned long diff,
  int           &count) const

  {
  count = 1;
  return(diff);
  }

bool Socket::is_completely_free() const
  {
  return(true);
  }

Socket::Socket(const std::string &json_layout, std::vector<std::string> &valid_ids)
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

void Socket::place_all_execution_slots(req &r, allocation &task_alloc)
  {
  placed_all++;
  }

bool Socket::spread_place_pu(

  req         &r,
  allocation  &task_alloc,
  int         &cores_per_task_remaining,
  int         &lprocs_per_task_remaining,
  int         &gpus_remaining,
  int         &mics_remaining)
 
  {
  called_spread_place_cores++;

  return(spreaded);
  }

bool Socket::spread_place(
    
  req        &r,
  allocation &master,
  int         execution_slots_per,
  int        &execution_slots_remainder,
  bool        chips)

  {
  called_spread_place++;

  return(spreaded);
  }

Socket &Socket::operator =(const Socket &other)
  {
  return(*this);
  }

Core::~Core()
  {
  }

Chip::~Chip()
  {
  }

Chip::Chip(const Chip &other) {}

Chip &Chip::operator =(const Chip &other)
  {
  return(*this);
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

int Socket::place_task(req &r, allocation &a, int to_place, const char *hostname)
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

float Socket::how_many_tasks_fit(const req &r, int place_type) const

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

unsigned long req::getMemory() const
  {
  return(req_mem);
  }

int req::getPlaceCores() const
  {
  return(my_core_count);
  }

int req::getPlaceThreads() const
  {
  return(my_core_count);
  }


complete_req::complete_req() {}

int complete_req::req_count() const
  {
  return(my_req_count);
  }

req &complete_req::get_req(int index)
  {
  static req r;

  return(r);
  }

void req::set_placement_type(const std::string &type)
  {
  }

void complete_req::set_hostlists(const char *job_id, const char *hostlists)
  {
  }

complete_req::complete_req(void *resc, int num_ppn, bool legacy) {}

req::req() {}
req::req(const req &other) {}

int req::getExecutionSlots() const
  {
  return(4);
  }

int req::get_sockets() const
  {
  return(sockets);
  }

int req::get_numa_nodes() const
  {
  return(numa_node_count);
  }

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

void req::record_allocation(allocation const &a) {}

int req::get_execution_slots() const
  {
  return(exec_slots);
  }

std::string req::getThreadUsageString() const
  {
  return(this->thread_usage_str);
  }

allocation::allocation(const char *jid)
  {
  this->jobid = jid;
  }
allocation::allocation(const allocation &other) 
  {
  this->jobid = other.jobid;
  }
allocation::allocation() {}
allocation::allocation(const req &r) {}

void allocation::set_place_type(const std::string &place)
  {
  this->place_type = my_placement_type;
  }

void allocation::place_indices_in_string(std::string &out, int which) {}

int allocation::add_allocation(allocation const &a) 
  {
  return(0);
  }

void allocation::set_host(const char *hostname)
  {
  this->hostname = hostname;
  }

allocation &allocation::operator =(const allocation &other)
  {
  return(*this);
  }

PCI_Device::~PCI_Device() {}
PCI_Device::PCI_Device() {}
PCI_Device::PCI_Device(const PCI_Device &other) {}

void PCI_Device::displayAsString(std::stringstream &out) const {}
PCI_Device &PCI_Device::operator=(const PCI_Device &other)
  {
  return(*this);
  }

int req::getMics() const
  {
  return(0);
  }

int req::getGpus() const
  {
  return(0);
  }

job::job() {}
job::~job() {}

int  Machine::initializeNVIDIADevices(hwloc_obj_t machine_obj, hwloc_topology_t topology)
  {
  return(0);
  }

