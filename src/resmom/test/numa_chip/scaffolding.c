#include <stdlib.h>
#include <stdio.h>
#include "machine.hpp"
#include "log.h"
#include "pbs_error.h"

const char  *use_cores = "usecores";
std::string  my_placement_type;
int          hardware_style;

const char *place_node = "node";
const char *place_socket = "socket";
const char *place_numa = "numachip";
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

PCI_Device::~PCI_Device()
  {
  }

void PCI_Device::displayAsString(std::stringstream &out) const {}

int get_machine_total_memory(hwloc_topology_t topology, unsigned long *memory)
  {
  *memory = 123456789;
  return(PBSE_NONE);
  }

req::req() : mem(0) {}

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
  return(my_placement_type);
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

  return(0);
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
