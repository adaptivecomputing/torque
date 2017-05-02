#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include "machine.hpp"
#include "log.h"
#include "pbs_error.h"
#include "req.hpp"
#include "allocation.hpp"

int hardware_style;
float tasks;
int placed;
int called_place;
int called_store_pci;
int called_spread_place;
int called_spread_place_cores;
int called_spread_place_threads;
bool oscillate = false;
bool avail_oscillate = false;
bool make_socket_exclusive = true;
bool completely_free = true;
int place_amount = 1;
int json_chip;
int placed_all;
int partially_placed;
std::string my_placement_type;

const char *place_node = "node";
const char *place_socket = "socket";
const char *place_numa_node = "numanode";
const char *place_core = "core";
const char *place_thread = "thread";
const int   exclusive_legacy = 6;

const int exclusive_none = 0;
const int exclusive_node = 1;
const int exclusive_socket = 2;
const int exclusive_chip = 3;
const int exclusive_core = 4;
const int exclusive_thread = 5;

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

int Chip::free_core_count() const
  {
  return(6);
  }

Chip::Chip(int np, int &es_remainder, int &pn_remainder)
  {
  }

void Chip::setMemory(hwloc_uint64_t mem)
  {
  }

hwloc_uint64_t Chip::getMemory() const
  {
  return(this->memory);
  }

Chip::Chip(const std::string &json_layout, std::vector<std::string> &valid_ids)
  {
  json_chip++;
  }

Chip::Chip(const Chip &other)
  {
  }

Chip &Chip::operator =(const Chip &other)
  {
  return(*this);
  }

Chip::Chip()
  {
  }

Chip::~Chip()
  {
  }

void Chip::aggregate_allocations(std::vector<allocation> &master_list) {}

void Chip::save_allocations(const Chip &other) {}

void Chip::displayAsJson(

  std::stringstream &out,
  bool               jobs) const

  {
  }

int Chip::get_total_gpus() const
  {
  return(0);
  }


bool Chip::spread_place(

  req        &r,
  allocation &master,
  allocation &remaining,
  allocation &remainder)

  {
  called_spread_place++;
  return(true);
  }

bool Chip::is_completely_free() const
  {
  return(completely_free);
  }

void Chip::remove_last_allocation(const char *jobid) {}

bool Chip::partially_place_task(

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
  else if (place_amount == 3)
    {
    // place in two tries
    if (master.cpus == 0)
      {
      int half = remaining.cpus / 2;
      master.cpus = half;
      remaining.cpus -= half;
      
      half = remaining.memory / 2;
      master.memory = half;
      remaining.memory -= half;
      }
    else
      {
      remaining.cpus -= master.cpus;
      remaining.memory -= master.memory;
      }
    }

  partially_placed++;

  return(true);
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

double Chip::how_many_tasks_fit(req const &r, int place_type) const
  {
  return(tasks);
  }



int Chip::place_task(req &r, allocation &a, int to_place, const char *hostname)

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

void Chip::place_all_execution_slots(req &r, allocation &task_alloc)
  {
  placed_all++;
  }

bool Chip::has_socket_exclusive_allocation() const
  {
  return(make_socket_exclusive);
  }

allocation::allocation(const allocation &other) : memory(0), cpus(0), gpus(0), mics(0) {}
allocation::allocation(const char *jobid) : memory(0), cpus(0), gpus(0), mics(0) {}
int allocation::add_allocation(const allocation &other) 
  
  {
  return(0);
  }

allocation::allocation(const req &r) : memory(1024), cpus(4), gpus(0), mics(0)
  {
  }

void allocation::set_host(const char *hostname) {}

bool allocation::fully_placed() const
  {
  return((this->cpus == 0) && (this->memory == 0) && (this->mics == 0) && (this->gpus == 0));
  }

bool allocation::partially_placed(
    
  const req &r) const

  {
  return (false);
  }

req::req() : numa_nodes(1) {}

void req::record_allocation(const allocation &a) {}
std::string req::getPlacementType() const

  {
  return(my_placement_type);
  }

int req::get_numa_nodes() const
  {
  return(this->numa_nodes);
  }

allocation::allocation() : memory(0), cpus(0), gpus(0), mics(0) {}

void allocation::set_place_type(

  const std::string &placement_str)

  {
  if (placement_str == place_node)
    this->place_type = exclusive_node;
  else if (placement_str.find(place_socket) == 0)
    this->place_type = exclusive_socket;
  else if (placement_str.find(place_numa_node) == 0)
    this->place_type = exclusive_chip;
  else if (placement_str.find(place_core) == 0)
    this->place_type = exclusive_core;
  else if (placement_str.find(place_thread) == 0)
    this->place_type = exclusive_thread;
  else
    this->place_type = exclusive_none;
  } // END set_place_type()

void allocation::adjust_for_spread(unsigned int quantity, bool find_remainder) {}

void allocation::adjust_for_remainder(allocation &remainder) {}

void allocation::clear() {}
  
bool Chip::spread_place_threads(

  req         &r,
  allocation  &task_alloc,
  int         &cores_per_task_remaining,
  int         &lprocs_per_task_remaining,
  int         &gpus_remaining,
  int         &mics_remaining)

  {
  cores_per_task_remaining--;
  lprocs_per_task_remaining--;
  called_spread_place_threads++;
  return(true);
  }

bool Chip::spread_place_cores(

  req         &r,
  allocation  &task_alloc,
  int         &cores_per_task_remaining,
  int         &lprocs_per_task_remaining,
  int         &gpus_remaining,
  int         &mics_remaining)

  {
  cores_per_task_remaining--;
  lprocs_per_task_remaining--;
  called_spread_place_cores++;
  return(true);
  }

int Chip::get_available_gpus() const
  {
  return(0);
  }

int Chip::get_available_mics() const
  {
  return(0);
  }

