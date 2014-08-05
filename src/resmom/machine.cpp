#include <pbs_config.h>
#include <vector>
#include <errno.h>
#include "hwloc.h"
#include "machine.hpp"

using namespace std;

#ifdef PENABLE_LINUX26_CPUSETS

  Machine& Machine::operator= (const Machine& newMachine)
    {
    if (this == &newMachine)
      return *this;

    numberOfSockets = newMachine.numberOfSockets;
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
    numberOfSockets = 0;
    totalMemoryInBytes = 0;
    totalChips = 0;
    totalCores = 0;
    totalThreads = 0;
    availableSockets = 0;
    availableChips = 0;
    availableCores = 0;
    availableThreads = 0;
    }


  int Machine::initializeMachine(hwloc_topology_t topology)
    {
    int topo_depth;
    hwloc_obj_t obj;

    /* Get the total depth of the topology and then
       traverse it to find our resources */
    topo_depth = hwloc_topology_get_depth(topology);

    for (int depth = 0; depth < topo_depth; depth++)
      {
      bool done = false;

      for (int i = 0; i < hwloc_get_nbobjs_by_depth(topology, depth); i++)
        {
        obj = hwloc_get_obj_by_depth(topology, depth, i);
        
        switch (obj->type)
          {
            case HWLOC_OBJ_MACHINE:
              {
              totalMemoryInBytes = obj->memory.total_memory;
              break;
              }

            case HWLOC_OBJ_SOCKET:
              {
              Socket newSocket;

              newSocket.initializeSocket(obj);
              this->isNUMA = newSocket.isNUMA;
              this->sockets.push_back(newSocket);
              if (this->isNUMA == true)
                {
                this->numberOfSockets++;
                this->availableSockets++;
                this->totalChips += newSocket.getTotalChips();
                this->availableChips += newSocket.getTotalChips();
                }
              else
                {
                this->numberOfSockets = 1;
                this->totalChips = 1; 
                this->availableChips =1;
                this->availableSockets = 1;
                }

              this->totalCores += newSocket.getTotalCores();
              this->totalThreads += newSocket.getTotalProcessors();
              this->availableCores += newSocket.getTotalCores();
              this->availableThreads += newSocket.getTotalProcessors();
              
              if (obj->next_sibling == NULL)
                done = true;
              break;
              }

            case HWLOC_OBJ_NODE:
              {
              Socket newSocket;
              Chip   newChip;

              newChip.initializeChip(obj);
              newSocket.initializeIntelSocket(obj->first_child, newChip);
              this->numberOfSockets++;
              this->availableSockets++;
              this->totalChips++;
              this->availableChips++;
              this->totalCores += newSocket.getTotalCores();
              this->totalThreads += newSocket.getTotalProcessors();
              this->availableCores += newSocket.getTotalCores();
              this->availableThreads += newSocket.getTotalProcessors();

              if (obj->next_sibling == NULL)
                done = true;
 
              break;
              }
              

            default:
              break;
          }
        }

        if (done == true)
          break;
      }

      return(0);
    }

  Machine::~Machine()
    {
    }

  hwloc_uint64_t Machine::getTotalMemory()
    {
    return(this->totalMemoryInBytes);
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

  int Machine::getNumberOfSockets()
    {
    return(this->numberOfSockets);
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

#endif
