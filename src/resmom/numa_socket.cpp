#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <errno.h>
#include "pbs_config.h"
#include "pbs_error.h"
#include "log.h"

using namespace std;

#ifdef PENABLE_LINUX_CGROUPS
#include "machine.hpp"
#include <hwloc.h>

Socket::Socket() : id (0), memory(0), totalThreads(0), socket_exclusive(false), totalCores(0)
  {
  memset(socket_cpuset_string, 0, MAX_CPUSET_SIZE);
  memset(socket_nodeset_string, 0, MAX_NODESET_SIZE);
  }

Socket::~Socket()
  {
  id = -1;
  }

/* initializeAMDSocket. 
 * AMD sockets tend to have a hierarcy of socket->numa_node with multimple
 * numa_nodes per socket. This method works on the assumption of this type
 * of architecture
 */
int Socket::initializeAMDSocket(hwloc_obj_t socket_obj, hwloc_topology_t topology)
  {
  hwloc_obj_t chip_obj;
  hwloc_obj_t prev = NULL;

  this->id = socket_obj->logical_index;
  this->memory = socket_obj->memory.total_memory / 1024;
  this->available_memory = this->memory;
  this->socket_cpuset = socket_obj->allowed_cpuset; 
  this->socket_nodeset = socket_obj->allowed_nodeset;
  hwloc_bitmap_list_snprintf(this->socket_cpuset_string, MAX_CPUSET_SIZE, this->socket_cpuset);
  hwloc_bitmap_list_snprintf(this->socket_nodeset_string, MAX_NODESET_SIZE, this->socket_nodeset);

  this->totalCores = hwloc_get_nbobjs_inside_cpuset_by_type(topology, this->socket_cpuset, HWLOC_OBJ_CORE);
  this->totalThreads = hwloc_get_nbobjs_inside_cpuset_by_type(topology, this->socket_cpuset, HWLOC_OBJ_PU);
  this->availableCores = this->totalCores;
  this->availableThreads = this->totalThreads;

  /* Get the NUMA Chips for this Socket */
  while ((chip_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_NODE, prev)) != NULL)
    {
    int is_in_subtree;

    is_in_subtree = hwloc_obj_is_in_subtree(topology, chip_obj, socket_obj);
    if (is_in_subtree)
      {
      Chip new_chip;

      new_chip.initializeChip(chip_obj, topology);
      this->chips.push_back(new_chip);
      }
    prev = chip_obj;
    }

  return(PBSE_NONE);
  }


/* Intel NUMA sockets have a hierarchy of numa_node->socket. Usually just one socket per
   numa->node. This method works on this assumption for the hardware setup. */
int Socket::initializeIntelSocket(hwloc_obj_t socket_obj, hwloc_topology_t topology)
  {
  hwloc_obj_t chip_obj;
  hwloc_obj_t prev = NULL;

  this->id = socket_obj->logical_index;
  this->memory = socket_obj->memory.total_memory / 1024;
  this->available_memory = this->memory;
  this->socket_cpuset = socket_obj->allowed_cpuset; 
  this->socket_nodeset = socket_obj->allowed_nodeset;
  hwloc_bitmap_list_snprintf(this->socket_cpuset_string, MAX_CPUSET_SIZE, this->socket_cpuset);
  hwloc_bitmap_list_snprintf(this->socket_nodeset_string, MAX_NODESET_SIZE, this->socket_nodeset);

  this->totalCores = hwloc_get_nbobjs_inside_cpuset_by_type(topology, this->socket_cpuset, HWLOC_OBJ_CORE);
  this->totalThreads = hwloc_get_nbobjs_inside_cpuset_by_type(topology, this->socket_cpuset, HWLOC_OBJ_PU);
  this->availableCores = this->totalCores;
  this->availableThreads = this->totalThreads;

  /* Get the NUMA Chips for this Socket */
  while ((chip_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_NODE, prev)) != NULL)
    {
    int is_in_subtree;

    is_in_subtree = hwloc_obj_is_in_subtree(topology, socket_obj, chip_obj);
    if (is_in_subtree)
      {
      Chip new_chip;

      new_chip.initializeChip(chip_obj, topology);
      this->chips.push_back(new_chip);
      }
    prev = chip_obj;

    }

  return(PBSE_NONE);
  }

/* Socket::initializeNonNUMASocket:
 * If the hardware is not NUMA then there will be no NUMA node
 * on the hardware. This method accounts for no NUMA node and creates
 * a placehodler NUMA Chip class which behaves as if there were
 * a single NUMA Chip on the hardware.
 */
int Socket::initializeNonNUMASocket(hwloc_obj_t obj, hwloc_topology_t topology)
  {
  /* This is not a NUMA architecture. So we make a NUMA chip as a place holder */
  Chip numaChip;

  numaChip.initializeNonNUMAChip(obj, topology);

  this->totalCores = numaChip.getTotalCores();
  this->totalThreads = numaChip.getTotalThreads();
  this->availableCores = numaChip.getAvailableCores();
  this->availableThreads = numaChip.getAvailableThreads();

  get_machine_total_memory(topology, &this->memory);
  this->memory /= 1024; // make kb
  this->available_memory = this->memory;
  this->chips.push_back(numaChip);
  return(PBSE_NONE);
  }

hwloc_uint64_t Socket::getMemory()
  {
  return(this->memory);
  }

int Socket::getTotalCores() const
  {
  return(this->totalCores);
  }

int Socket::getTotalThreads() const
  {
  return(this->totalThreads);
  }

int Socket::getTotalChips() const
  {
  return(this->chips.size());
  }

int Socket::getAvailableChips() const
  {
  int available_chips = 0;
  std::vector<Chip>::iterator chip_iter;

  if (socket_exclusive == false)
    {
    for (unsigned int i = 0; i < this->chips.size(); i++)
      {
      if (this->chips[i].chipIsAvailable() == true)
        {
        available_chips++;
        }
      }
    }

  return(available_chips);
  }

int Socket::getAvailableCores() const
  {
  int available = 0;

  if (socket_exclusive == false)
    {
    for (unsigned int i = 0; i < this->chips.size(); i++)
      available += this->chips[i].getAvailableCores();
    }

  return(available);
  }

int Socket::getAvailableThreads() const
  {
  int available = 0;

  if (socket_exclusive == false)
    {
    for (unsigned int i = 0; i < this->chips.size(); i++)
      available += this->chips[i].getAvailableThreads();
    }

  return(available);
  }

hwloc_uint64_t Socket::getAvailableMemory() const

  {
  hwloc_uint64_t available = 0;

  if (socket_exclusive == false)
    {
    for (unsigned int i = 0; i < this->chips.size(); i++)
      available += this->chips[i].getAvailableMemory();
    }

  return(available);
  }

void Socket::setMemory(
    
  hwloc_uint64_t memory)

  {
  this->memory = memory;
  }

void Socket::setId(

  int id)

  {
  this->id = id;
  }

void Socket::addChip()

  {
  Chip c;
  this->chips.push_back(c);
  }

void Socket::displayAsString(

  stringstream &out) const

  {
  out << "  Socket " << this->id << " (" << this->memory << "KB)\n";

  for (unsigned int i = 0; i < this->chips.size(); i++)
    this->chips[i].displayAsString(out);
  } // END displayAsString()



/*
 * how_many_tasks_fit()
 *
 * Checks req r to see how many of its tasks could possibly fit on this socket
 * @param r (I) - the req object to check
 * @return - the number of tasks from r that could be placed on this socket
 */

int Socket::how_many_tasks_fit(

  const req &r,
  int        place_type) const

  {
  int num_that_fit = 0;

  if ((socket_exclusive == false) &&
      ((place_type != exclusive_socket) ||
       (this->is_available() == true)))
    {
    for (unsigned int i = 0; i < this->chips.size(); i++)
      num_that_fit += this->chips[i].how_many_tasks_fit(r, place_type);

    if ((num_that_fit > 1) &&
        (r.getPlacementType() == place_socket))
      num_that_fit = 1;
    }

  return(num_that_fit);
  } // END how_many_tasks_fit()



/*
 * place_task()
 *
 * Places as many tasks as available, up to to_place, on this socket.
 * @param jobid - the job whose tasks we're placing
 * @param r - the req we're pulling tasks from
 * @param master - the allocation for the entire job
 * @param to_place - the maximum number of tasks that should be placed
 * @return the number of tasks placed
 */

int Socket::place_task(

  const char *jobid,
  const req  &r,
  allocation &master,
  int         to_place)

  {
  int        tasks_to_place = to_place;
  allocation a(jobid);

  if ((master.place_type != exclusive_socket) ||
      (this->is_available() == true))
    {
    if (socket_exclusive == false)
      {
      // Attempt to fit all of tasks on a single numa chip if possible
      for (unsigned int i = 0; i < this->chips.size() && tasks_to_place > 0; i++)
        {
        if (this->chips[i].how_many_tasks_fit(r, master.place_type) >= to_place)
          tasks_to_place -= this->chips[i].place_task(jobid, r, a, to_place);
        }

      // place tasks if they didn't fit in a single numa chip
      if (tasks_to_place > 0)
        {
        for (unsigned int i = 0; i < this->chips.size() && tasks_to_place > 0; i++)
          tasks_to_place -= this->chips[i].place_task(jobid, r, a, tasks_to_place);
        }

      this->availableCores -= a.cores;
      this->availableThreads -= a.threads;
      this->available_memory -= a.memory;

      if (master.place_type == exclusive_socket)
        this->socket_exclusive = true;

      master.add_allocation(a);
      }
    }

  return(to_place - tasks_to_place);
  } // END place_task



/*
 * free_task()
 *
 * Frees all objects in use for the job identified by jobid
 *
 * @param jobid - the jobid of the job that has ended
 * @return true if this socket is now completely free
 */

bool Socket::free_task(

  const char *jobid)

  {
  bool completely_free = true;
  for (unsigned int i = 0; i < this->chips.size(); i++)
    {
    if (this->chips[i].free_task(jobid) == false)
      completely_free = false;
    }

  if (completely_free)
    socket_exclusive = false;

  return(completely_free);
  } // END free_task()



/*
 * is_available()
 *
 * @return true if the Socket is completely free, else false
 * NOTE: false doesn't mean there is no space, it means some space is taken
 */

bool Socket::is_available() const

  {
  bool available = true;

  if (socket_exclusive == false)
    {
    for (unsigned int i = 0; i < this->chips.size(); i++)
      {
      if (this->chips[i].chipIsAvailable() == false)
        {
        available = false;
        break;
        }
      }
    }
  else
    available = false;

  return(available);
  } // END is_available()



/*
 * fits_on_socket()
 * Determines if the task identified by remaining can fit completely on this socket
 * @param remaining - an allocation with the information for whats left for the task
 * @return - true if the allocation will completely fit on this socket, false otherwise
 */

bool Socket::fits_on_socket(
    
  const allocation &remaining) const

  {
  bool fits = false;
  if (this->getAvailableMemory() >= remaining.memory)
    {
    if ((remaining.cores_only == true) &&
        (this->getAvailableCores() >= remaining.cpus))
      fits = true;
    else if ((remaining.cores_only == false) &&
             (this->getAvailableThreads() >= remaining.cpus))
      fits = true;
    }

  return(fits);
  } // END fits_on_socket()



/*
 * partially_place()
 *
 * Partially places the task whose needs are identified by remaining on the 
 * chips that are part of this socket.
 * @param remaining (I/O) - the amount remaining to place for this task
 * @param master (O) - the complete allocation for this job
 * @return - true if the task was fully placed, false if partially placed
 */

bool Socket::partially_place(

  allocation &remaining,
  allocation &master)

  {
  bool fully_placed = false;

  for (unsigned int i = 0; i < this->chips.size(); i++)
    {
    this->chips[i].partially_place_task(remaining, master);

    if ((remaining.cpus == 0) &&
        (remaining.memory == 0))
      {
      fully_placed = true;
      break;
      }
    }

  return(fully_placed);
  }



bool Socket::store_pci_device_appropriately(

  PCI_Device &device,
  bool        force)

  {
  bool stored = false;
  for (unsigned int i = 0; i < this->chips.size(); i++)
    {
    if (this->chips[i].store_pci_device_appropriately(device, force) == true)
      {
      stored = true;
      break;
      }
    }

  return(stored);
  }


#endif /* PENABLE_LINUX_CGROUPS */  
