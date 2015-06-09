#include <stdlib.h>
#include <stdio.h>
#include "machine.hpp"
#include "log.h"
#include "pbs_error.h"

const char  *use_cores = "usecores";
std::string  my_placement_type;
std::string  thread_type;
int          hardware_style;

const int    MIC_TYPE = 0;
const int    GPU = 1;

const char *place_node = "node";
const char *place_socket = "socket";
const char *place_numa_node = "numanode";
const char *place_core = "core";

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

req::req() : mem(0), gpus(0), mics(0) {}

unsigned long req::getMemory() const
  {
  return(this->mem);
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

int req::set_value(const char *name, const char *value)
  {
  if (!strcmp(name, "lprocs"))
    this->execution_slots = atoi(value);
  else if (!strcmp(name, "memory"))
    this->mem = atoi(value);
  else if (!strcmp(name, "gpus"))
    this->gpus = atoi(value);
  else if (!strcmp(name, "mics"))
    this->mics = atoi(value);

  return(0);
  }

int req::getMics() const
  {
  return(this->mics);
  }
 
int req::getGpus() const
  {
  return(this->gpus);
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

void translate_range_string_to_vector(

  const char       *range_string,
  std::vector<int> &indices)

  {
  char *str = strdup(range_string);
  char *ptr = str;
  int   prev;
  int   curr;

  while (*ptr != '\0')
    {
    prev = strtol(ptr, &ptr, 10);
    
    if (*ptr == '-')
      {
      ptr++;
      curr = strtol(ptr, &ptr, 10);

      while (prev <= curr)
        {
        indices.push_back(prev);

        prev++;
        }

      if ((*ptr == ',') ||
          (is_whitespace(*ptr)))
        ptr++;
      }
    else
      {
      indices.push_back(prev);

      if ((*ptr == ',') ||
          (is_whitespace(*ptr)))
        ptr++;
      }
    }

  free(str);
  } /* END translate_range_string_to_vector() */



void add_range_to_string(

  std::string &range_string,
  int          begin,
  int          end)

  {
  char buf[MAXLINE];

  if (begin == end)
    {
    if (range_string.size() == 0)
      sprintf(buf, "%d", begin);
    else
      sprintf(buf, ",%d", begin);
    }
  else
    {
    if (range_string.size() == 0)
      sprintf(buf, "%d-%d", begin, end);
    else
      sprintf(buf, ",%d-%d", begin, end);
    }

  range_string += buf;
  } // END add_range_to_string()



void translate_vector_to_range_string(

  std::string            &range_string,
  const std::vector<int> &indices)

  {
  // range_string starts empty
  range_string.clear();

  if (indices.size() == 0)
    return;

  int first = indices[0];
  int prev = first;

  for (unsigned int i = 1; i < indices.size(); i++)
    {
    if (indices[i] == prev + 1)
      {
      // Still in a consecutive range
      prev = indices[i];
      }
    else
      {
      add_range_to_string(range_string, first, prev);

      first = prev = indices[i];
      }
    }

  // output final piece
  add_range_to_string(range_string, first, prev);
  } // END translate_vector_to_range_string()
