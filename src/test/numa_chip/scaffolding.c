#include <stdlib.h>
#include <stdio.h>
#include "machine.hpp"
#include "log.h"
#include "pbs_error.h"
#include "machine.hpp"

const char  *use_cores = "usecores";
std::string  my_placement_type;
std::string  thread_type;
int          hardware_style;
int          recorded = 0;

const int    MIC_TYPE = 0;
const int    GPU = 1;

const char *place_node = "node";
const char *place_socket = "socket";
const char *place_numa_node = "numanode";
const char *place_core = "core";
const char *place_thread = "thread";
const char *place_legacy = "legacy";
const char *place_legacy2 = "legacy2";

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

PCI_Device::PCI_Device() 
  {
  static int pci_count = 0;

  pci_count++;
  if (pci_count % 2)
    this->type = MIC_TYPE;
  else
    this->type = GPU;

  this->id = pci_count;
  this->busy = false;
  strcpy(this->cpuset_string, "0");
  }

void PCI_Device::set_type(int type)
  {
  this->type = type;
  }

void PCI_Device::setId(int id)
  {
  this->id = id;
  }

PCI_Device::~PCI_Device() {}

PCI_Device::PCI_Device(const PCI_Device &other) 
  
  {
  this->type = other.type;
  this->id = other.id;
  this->busy = other.busy;
  strcpy(this->cpuset_string, other.cpuset_string);
  }

void PCI_Device::displayAsString(std::stringstream &out) const {}

int PCI_Device::get_type() const

  {
  return(this->type);
  }

PCI_Device &PCI_Device::operator=(const PCI_Device &other)
  {
  this->type = other.type;
  this->id = other.id;
  this->busy = other.busy;
  strcpy(this->cpuset_string, other.cpuset_string);
  return(*this);
  }

const char *PCI_Device::get_cpuset() const
  {
  return(this->cpuset_string);
  }

void PCI_Device::set_state(bool in_use)
  {
  this->busy = in_use;
  }

int PCI_Device::get_id() const
  {
  return(this->id);
  }

bool PCI_Device::is_busy() const
  {
  return(this->busy);
  }

int get_machine_total_memory(hwloc_topology_t topology, unsigned long *memory)
  {
  *memory = 123456789;
  return(PBSE_NONE);
  }

req::req() : mem_per_task(0), cores(0), threads(0), gpus(0), mics(0), task_count(1) {}

unsigned long long req::get_memory_per_task() const
  {
  return(this->mem_per_task);
  }

int req::getTaskCount() const
  {
  return(this->task_count);
  }

int req::getPlaceCores() const
  {
  return(this->cores);
  }

int req::getPlaceThreads() const
  {
  return(this->threads);
  }

std::string req::getPlacementType() const
  {
  return(my_placement_type);
  }

std::string req::getThreadUsageString() const
  {
  return(thread_type);
  }


int req::getExecutionSlots() const
  {
  return(this->execution_slots);
  }

int req::set_value(const char *name, const char *value, bool is_default)
  {
  if (!strcmp(name, "lprocs"))
    this->execution_slots = atoi(value);
  else if (!strcmp(name, "memory"))
    this->mem_per_task = strtol(value, NULL, 10);
  else if (!strcmp(name, "total_memory"))
    {
    this->total_mem = strtol(value, NULL, 10);
    this->mem_per_task = this->total_mem / this->task_count;
    }
  else if (!strcmp(name, "gpus"))
    this->gpus = atoi(value);
  else if (!strcmp(name, "mics"))
    this->mics = atoi(value);
  else if (!strcmp(name, "task_count"))
    this->task_count = atoi(value);

  return(0);
  }

int req::getMics() const
  {
  return(this->mics);
  }
 
int req::get_gpus() const
  {
  return(this->gpus);
  }

void req::record_allocation(const allocation &a)
  {
  recorded++;
  }

int is_whitespace(

  char c)

  {
  if ((c == ' ')  ||
      (c == '\n') ||
      (c == '\t') ||
      (c == '\r') ||
      (c == '\f'))
    return(TRUE);
  else
    return(FALSE);
  } /* END is_whitespace */

#ifdef MIC
int Chip::initializeMICDevices(hwloc_obj_t chip_obj, hwloc_topology_t topology)
  {
  return(0);
  }
#endif
