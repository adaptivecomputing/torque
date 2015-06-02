#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include "machine.hpp"
#include "log.h"
#include "pbs_error.h"
#include "req.hpp"
#include "allocation.hpp"

int hardware_style;
int tasks;
int placed;
int called_place;
int called_store_pci;
bool oscillate = false;
bool avail_oscillate = false;
int place_amount = 1;
int json_chip;
std::string my_placement_type;
const char *place_socket = "socket";

const int exclusive_socket = 2;

void log_err(int errnum, const char *routine, const char *text)
  {
  }

int get_hardware_style(hwloc_topology_t topology)
  {
  hwloc_obj_t node_obj;
  hwloc_obj_t socket_obj;
  int node_depth;
  int socket_depth;

  node_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_NODE, NULL); 
  if (node_obj == NULL)
    {
    return(NON_NUMA);
    }
  node_depth = node_obj->depth;

  socket_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_SOCKET, NULL); 
  if (socket_obj == NULL)
    {
    log_err(-1, __func__, "failed to get node object");
    return(PBSE_SYSTEM);
    }
  socket_depth = socket_obj->depth;

  if (socket_depth < node_depth)
    return(AMD);
  else
    return(INTEL);
  }

PCI_Device::PCI_Device() {}
PCI_Device::~PCI_Device()
  {
  }
PCI_Device::PCI_Device(const PCI_Device &other) {}
PCI_Device &PCI_Device::operator=(const PCI_Device &other)
  {
  return(*this);
  }

Core::~Core()
  {
  }

Chip::Chip(const std::string &json_layout)
  {
  json_chip++;
  }

Chip::Chip()
  {
  }

Chip::~Chip()
  {
  }

void Chip::aggregate_allocations(std::vector<allocation> &master_list) {}

void Chip::displayAsJson(

  std::stringstream &out,
  bool               jobs) const

  {
  }

void Chip::partially_place_task(

  allocation &remaining,
  allocation &master)

  {
  if (place_amount == 1)
    {
    remaining.memory = 0;
    remaining.cpus = 0;
    }
  else if (place_amount == 2)
    {
    remaining.memory /= 2;
    remaining.cpus /= 2;
    }
  }

int Chip::initializeNonNUMAChip(hwloc_obj_t obj, hwloc_topology_t topology)
  {
  return(PBSE_NONE);
  }

int Chip::initializeChip(hwloc_obj_t obj, hwloc_topology_t topology)
  {
  return(PBSE_NONE);
  }

hwloc_uint64_t Chip::getAvailableMemory() const
  {
  return(12);
  }

int Chip::getAvailableCores() const
  {
  return(6);
  }

bool  Chip::chipIsAvailable() const
  {
  static bool result = false;
  
  if (avail_oscillate)
    {
    result = !result;
    return(result);
    }
  else
    return(true);
  }

int Chip::getTotalCores() const
  {
  return(6);
  }

int Chip::getAvailableThreads() const
  {
  return(6);
  }

int Chip::getTotalThreads() const
  {
  return(6);
  }

int get_machine_total_memory(hwloc_topology_t topology, unsigned long *memory)
  {
  *memory = 123456789;
  return(PBSE_NONE);
  }

int Chip::how_many_tasks_fit(req const &r, int place_type) const
  {
  return(tasks);
  }

int Chip::place_task(const char *jobid, const req &r, allocation &a, int to_place)

  {
  called_place++;
  return(placed);
  }

bool Chip::free_task(const char *jobid)
  {
  static int count = 0;

  if (oscillate)
    {
    return(count++ % 2 == 0);
    }
  return(true);
  }

void Chip::displayAsString(std::stringstream &out) const {}

bool Chip::store_pci_device_appropriately(PCI_Device &d, bool force)
  {
  called_store_pci++;
  return(force);
  }

allocation::allocation(const allocation &other) {}
allocation::allocation(const char *jobid) {}
int allocation::add_allocation(const allocation &other) 
  
  {
  return(0);
  }

req::req() {}
std::string req::getPlacementType() const

  {
  return(my_placement_type);
  }

allocation::allocation() {}
