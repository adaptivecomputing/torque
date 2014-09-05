#include <stdlib.h>
#include <stdio.h>
#include "machine.hpp"
#include "log.h"
#include "pbs_error.h"

int hardware_style;

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

Core::~Core()
  {
  }

Core::Core()
  {
  }

int Core::initializeCore(hwloc_obj_t core_obj, hwloc_topology_t topology)
    {
    /* We now need to find all of the processing units associated with this core */
    this->id = core_obj->logical_index;
    this->core_cpuset = core_obj->allowed_cpuset;
    this->core_nodeset = core_obj->allowed_nodeset;
    hwloc_bitmap_list_snprintf(this->core_cpuset_string, MAX_CPUSET_SIZE, this->core_cpuset);
    hwloc_bitmap_list_snprintf(this->core_nodeset_string, MAX_NODESET_SIZE, this->core_nodeset);return(0);
    this->totalThreads = hwloc_get_nbobjs_inside_cpuset_by_type(topology, this->core_cpuset, HWLOC_OBJ_PU);
    return(PBSE_NONE);
    }


int get_machine_total_memory(hwloc_topology_t topology, unsigned long *memory)
  {
  *memory = 123456789;
  return(PBSE_NONE);
  }








