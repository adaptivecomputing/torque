#include <string>
#include <vector>
#include <errno.h>
#include <hwloc.h>
#include "pbs_config.h"

#ifdef PENABLE_LINUX_CGROUPS

#ifdef MIC
#include <hwloc/intel-mic.h>
#endif

#ifdef NVIDIA_GPUS
#ifdef NVML_API
#include <hwloc/nvml.h>
#endif
#endif

#include "machine.hpp"
#include "pbs_error.h"
#include "log.h"

using namespace std;


/* 26 August 2014
 * Currently Intel and AMD NUMA hardware
 * put their NUMA node and Socket in a different order.
 * For AMD the Socket is at a higher depth than
 * the NUMA node. Higher means a lower number. i.e 
 * the socket will be at depth 1 and the NUMA node
 * at depth 2. Usually the NUMA nodes are children
 * of the socket. 
 * Intel has the NUMA node at the higher level with the
 * Socket as a child of the NUMA node.
 * get_hardware_style returns a 1 to indicate the Intel style
 * and 2 to indicate the AMD style.
 * Note that it is inteded that this function can change to accomodate
 * future changes in hardware configurations.
 */


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

int get_machine_total_memory(hwloc_topology_t topology, hwloc_uint64_t *memory)
  {
  hwloc_obj_t machine_obj;
  hwloc_obj_t prev = NULL;

  /* This is a non NUMA architecture. We need to put the memory on the NUMA Chip but 
   * there is not a NUMA node on this machine. So get the memory from the machine object
   * and assign it to the Chip */
  machine_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_MACHINE, prev);
  if (machine_obj == NULL)
    {
    printf("Cannot get machine object\n");
    return(PBSE_SYSTEM);
    }

  *memory = machine_obj->memory.total_memory;

  return(PBSE_NONE);
  }


  Machine& Machine::operator= (const Machine& newMachine)
    {
    if (this == &newMachine)
      return *this;

    totalSockets = newMachine.totalSockets;
    sockets = newMachine.sockets;
    totalMemoryInBytes = newMachine.totalMemoryInBytes;
    totalChips = newMachine.totalChips;
    totalCores = newMachine.totalCores;
    totalThreads = newMachine.totalThreads;
    availableSockets = newMachine.availableSockets;
    availableChips = newMachine.availableChips;
    availableCores = newMachine.availableCores;
    availableThreads = newMachine.availableThreads;
    return *this;
    }

  Machine::Machine()
    { 
    totalSockets = 0;
    totalMemoryInBytes = 0;
    totalChips = 0;
    totalCores = 0;
    totalThreads = 0;
    availableSockets = 0;
    availableChips = 0;
    availableCores = 0;
    availableThreads = 0;
    memset(allowed_cpuset_string, 0, MAX_CPUSET_SIZE);
    memset(allowed_nodeset_string, 0, MAX_NODESET_SIZE);
    }


  Machine::~Machine()
    {
    }


  int Machine::initializeNonNUMAMachine(hwloc_obj_t obj, hwloc_topology_t topology)
    {
    hwloc_obj_t prev = NULL;
    hwloc_obj_t socket_obj;

    isNUMA = false;
    /* because we do not have NUMA hardware set socket to 1 and numa_chip to 1 */
    totalSockets = 1;
    totalChips = 1;
    availableSockets = 1;
    availableChips = 1;

    /* get the socket information */
    socket_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_SOCKET, prev);
    if (socket_obj == NULL)
      {
      /* What went wrong? Is this ancient hardware? No socket? hmm.*/
      printf("hwloc did not find a socket for this machine\n");
      return(-1);
      /* return(PBSE_SYSTEM)*/
      }

    Socket non_numa_socket;

    non_numa_socket.initializeNonNUMASocket(socket_obj, topology);

    this->sockets.push_back(non_numa_socket);

    }

  int Machine::initializeMachine(hwloc_topology_t topology)
    {
    hwloc_obj_t obj;
    hwloc_obj_t prev = NULL;
    hwloc_const_cpuset_t machine_cpuset;
    hwloc_const_nodeset_t machine_nodeset;

    /* Get the Machine object */
    obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_MACHINE, prev);
    if (obj == NULL)
      {
      log_err(-1, __func__, "failed to get Machine object");
      /*return(PBSE_SYSTEM);*/
      return(PBSE_SYSTEM);
      }
    
    /* We have the machine object. Fill in what we know */
    totalMemoryInBytes = obj->memory.total_memory;
    machine_cpuset = hwloc_topology_get_allowed_cpuset(topology);
    machine_nodeset = hwloc_topology_get_allowed_nodeset(topology);
    /* initialize the machines cpuset and nodeset strings */
    hwloc_bitmap_list_snprintf(allowed_cpuset_string, MAX_CPUSET_SIZE, machine_cpuset); 
    hwloc_bitmap_list_snprintf(allowed_nodeset_string, MAX_NODESET_SIZE, machine_nodeset); 

    totalSockets = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_SOCKET);
    totalChips = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_NODE);
    totalCores = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_CORE);
    totalThreads = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_PU);
    availableSockets = totalSockets; 
    availableChips   = totalChips; 
    availableCores   = totalCores;
    availableThreads = totalThreads;


    /* Check to see if we have a NUMA architecture */
    /* if totalChips is 0 this is not NUMA hardware */
    if (totalChips == 0)
      {
      initializeNonNUMAMachine(obj, topology);
      isNUMA = false;
      return(PBSE_NONE);
      }
    
    /* This is NUMA Hardware */
    isNUMA = true;

    /* Next determine if we are an Intel style setup
     * or and AMD type setup. Intel tends to have
     * the NUMA node at a higher level than the 
     * socket whereas AMD has the socket at the higher level
     */
    int style;

    style = get_hardware_style(topology);
    hwloc_obj_t socket_obj;

    prev = NULL;
    while ((socket_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_SOCKET, prev)) != NULL)
      {
      if (style == INTEL)
        {
        Socket intel_socket;
        intel_socket.initializeIntelSocket(socket_obj, topology);
        }
      else if (style == AMD)
        {
        Socket amd_socket;
        amd_socket.initializeAMDSocket(socket_obj, topology);
        this->sockets.push_back(amd_socket);
        }
      else
        {
        /* Something failed. Return the error */
        return(style);
        }

      prev = socket_obj;
      }
    
    return(PBSE_NONE);
    }

  hwloc_uint64_t Machine::getTotalMemory()
    {
    return(this->totalMemoryInBytes);
    }

  int Machine::getNumberOfSockets()
    {
    return(this->totalSockets);
    }

  int Machine::getTotalChips()
    {
    return(this->totalChips);
    }
  
  int Machine::getTotalCores()
    {
    return(this->totalCores);
    }

  int Machine::getTotalThreads()
    {
    return(this->totalThreads);
    }


  int Machine::getAvailableSockets()
    {
    return(this->availableSockets);
    }

  int Machine::getAvailableChips()
    {
    return(this->availableChips);
    }

  int Machine::getAvailableCores()
    {
    return(this->availableCores);
    }

  int Machine::getAvailableThreads()
    {
    return(this->availableThreads);
    }


#endif /* PENABLE_LINUX26_CPUSETS */
