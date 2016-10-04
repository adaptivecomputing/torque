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

Socket::Socket() : id (0), memory(0), totalCores(0), totalThreads(0), availableCores(0),
                   availableThreads(0), chips(), socket_exclusive(false)
  {
  memset(socket_cpuset_string, 0, MAX_CPUSET_SIZE);
  memset(socket_nodeset_string, 0, MAX_NODESET_SIZE);
  }



Socket::Socket(
    
  int  execution_slots,
  int  numa_nodes,
  int &es_remainder) : id (0), memory(0), totalCores(execution_slots),
                       totalThreads(execution_slots), availableCores(0), availableThreads(0),
                       chips(), socket_exclusive(false)

  {
  memset(socket_cpuset_string, 0, MAX_CPUSET_SIZE);
  memset(socket_nodeset_string, 0, MAX_NODESET_SIZE);

  int per_numa_remainder = execution_slots % numa_nodes;
  
  for (int i = 0; i < numa_nodes; i++)
    {
    Chip c(execution_slots / numa_nodes, es_remainder, per_numa_remainder);
    this->chips.push_back(c);
    }
  }



/*
 * Builds a copy of the machine's layout in from json which has no whitespace but 
 * if it did it'd look like:
 *
 * "socket" : {
 *   "os_index" : <index>,
 *   "numanode" : {
 *     "os_index" : <index>,
 *     "cores" : <core range string>,
 *     "threads" : <thread range string>,
 *     "mem" : <memory in kb>,
 *     "gpus" : <gpu range string>,
 *     "mics" : <mic range string>
 *     }
 *   [, "numanode" ... ]
 * }
 *
 * mics and gpus are optional and only present if they actually exist on the node
 *
 */

Socket::Socket(

  const std::string &json_layout,
  std::vector<std::string> &valid_ids) : id(0), memory(0), totalCores(0), totalThreads(0),
                                         availableCores(0), availableThreads(0), chips(),
                                         socket_exclusive(false)

  {
  const char *chip_str = "\"numanode\":{";
  const char *os_str = "\"os_index\":";
  std::size_t chip_begin = json_layout.find(chip_str);
  std::size_t os_begin = json_layout.find(os_str);
  
  memset(socket_cpuset_string, 0, MAX_CPUSET_SIZE);
  memset(socket_nodeset_string, 0, MAX_NODESET_SIZE);

  if ((os_begin == std::string::npos) ||
      (os_begin > chip_begin))
    return;
  else
    {
    std::string os = json_layout.substr(os_begin + strlen(os_str));
    this->id = strtol(os.c_str(), NULL, 10);
    }

  while (chip_begin != std::string::npos)
    {
    std::size_t next = json_layout.find(chip_str, chip_begin + 1);
    std::string one_chip = json_layout.substr(chip_begin, next - chip_begin);

    Chip c(one_chip, valid_ids);
    this->chips.push_back(c);

    chip_begin = next;
    }
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
int Socket::initializeAMDSocket(
    
  hwloc_obj_t      socket_obj,
  hwloc_topology_t topology)

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
int Socket::initializeIntelSocket(
    
  hwloc_obj_t      socket_obj,
  hwloc_topology_t topology)

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



Socket &Socket::operator=(
    
  const Socket &other)

  {
  this->id = other.id;
  this->memory = other.memory;
  this->available_memory = other.available_memory;
  this->totalCores = other.totalCores;
  this->totalThreads = other.totalThreads;
  this->availableCores = other.availableCores;
  this->availableThreads = other.availableThreads;
  this->chips = other.chips;

  memcpy(this->socket_cpuset_string, other.socket_cpuset_string, sizeof(this->socket_cpuset_string));
  memcpy(this->socket_nodeset_string, other.socket_nodeset_string, 
         sizeof(this->socket_nodeset_string));

  this->socket_exclusive = other.socket_exclusive;

  if (strlen(this->socket_cpuset_string))
    hwloc_bitmap_list_snprintf(this->socket_cpuset_string, MAX_CPUSET_SIZE, this->socket_cpuset);

  if (strlen(this->socket_nodeset_string))
    hwloc_bitmap_list_snprintf(this->socket_nodeset_string, MAX_NODESET_SIZE, this->socket_nodeset);

  return(*this);
  }



hwloc_uint64_t Socket::getMemory() const
  {
  hwloc_uint64_t mem = 0;

  for (size_t i = 0; i < this->chips.size(); i++)
    mem += this->chips[i].getMemory();

  return(mem);
  }



/*
 * get_memory_for_completely_free_chips()
 *
 * Figures out how much memory this socket has in completely free chips
 *
 * @param mem_required - the amount of memory needed
 * @param numa_count - a count of the free nodes used
 */

hwloc_uint64_t Socket::get_memory_for_completely_free_chips(
    
  unsigned long  mem_required,
  int           &numa_count) const
  {
  hwloc_uint64_t mem = 0;

  numa_count = 0;

  for (size_t i = 0; i < this->chips.size() && mem < mem_required; i++)
    {
    if (this->chips[i].is_completely_free())
      {
      mem += this->chips[i].getMemory();
      numa_count++;
      }
    }

  return(mem);
  }

int Socket::getTotalCores() const
  {
  int total = 0;
  for (unsigned int i = 0; i < this->chips.size(); i++)
    {
    total += this->chips[i].getTotalCores();
    }

  return(total);
  }

int Socket::getTotalThreads() const
  {
  int total = 0;
  for (unsigned int i = 0; i < this->chips.size(); i++)
    {
    total += this->chips[i].getTotalThreads();
    }

  return(total);
  }

int Socket::getTotalChips() const
  {
  return(this->chips.size());
  }

int Socket::getAvailableChips() const
  {
  int available_numa_nodes = 0;
  std::vector<Chip>::iterator chip_iter;

  if (this->socket_exclusive == false)
    {
    for (unsigned int i = 0; i < this->chips.size(); i++)
      {
      if (this->chips[i].chipIsAvailable() == true)
        {
        available_numa_nodes++;
        }
      }
    }

  return(available_numa_nodes);
  }

int Socket::getAvailableCores() const
  {
  int available = 0;

  if (this->socket_exclusive == false)
    {
    for (unsigned int i = 0; i < this->chips.size(); i++)
      available += this->chips[i].getAvailableCores();
    }

  return(available);
  }

int Socket::get_free_cores() const
  {
  int free_cores = 0;
  
  if (this->socket_exclusive == false)
    {
    for (unsigned int i = 0; i < this->chips.size(); i++)
      free_cores += this->chips[i].free_core_count();
    }

  return(free_cores);
  }

int Socket::getAvailableThreads() const
  {
  int available = 0;

  if (this->socket_exclusive == false)
    {
    for (unsigned int i = 0; i < this->chips.size(); i++)
      available += this->chips[i].getAvailableThreads();
    }

  return(available);
  }

hwloc_uint64_t Socket::getAvailableMemory() const

  {
  hwloc_uint64_t available = 0;

  if (this->socket_exclusive == false)
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
  this->available_memory = memory;
  
  if (this->chips.size() != 0)
    {
    long long per_chip = memory / this->chips.size();
  
    for (unsigned int i = 0; i < this->chips.size(); i++)
      this->chips[i].setMemory(per_chip);
    }
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



void Socket::displayAsJson(

  stringstream &out,
  bool          include_jobs) const

  {
  out << "\"socket\":{\"os_index\":" << this->id;

  for (unsigned int i = 0; i < this->chips.size(); i++)
    {
    out << ",";
    this->chips[i].displayAsJson(out, include_jobs);
    }

  out << "}";
  } // END displayAsJson()



void Socket::update_internal_counts(

  std::vector<allocation> &allocs)

  {
  for (unsigned int i = 0; i < this->chips.size(); i++)
    {
    this->chips[i].aggregate_allocations(allocs);

    if (this->chips[i].has_socket_exclusive_allocation())
      this->socket_exclusive = true;
    }
  }



/*
 * how_many_tasks_fit()
 *
 * Checks req r to see how many of its tasks could possibly fit on this socket
 * @param r (I) - the req object to check
 * @return - the number of tasks from r that could be placed on this socket
 */

float Socket::how_many_tasks_fit(

  const req &r,
  int        place_type) const

  {
  float num_that_fit = 0;

  if ((this->socket_exclusive == false) &&
      ((place_type != exclusive_socket) ||
       (this->is_available() == true)))
    {
    int numa = r.get_numa_nodes();

    if (numa > 1)
      {
      // If numa > 1, place_type = exclusive_numa, so each chip is 1 piece at most.
      for (unsigned int i = 0; i < this->chips.size(); i++)
        if (this->chips[i].how_many_tasks_fit(r, place_type) >= 1.0)
          num_that_fit += 1;

      num_that_fit /= numa;
      }
    else
      {
      for (unsigned int i = 0; i < this->chips.size(); i++)
        num_that_fit += this->chips[i].how_many_tasks_fit(r, place_type);

      if ((num_that_fit > 1) &&
          (r.getPlacementType() == place_socket))
        num_that_fit = 1;
      }
    }

  return(num_that_fit);
  } // END how_many_tasks_fit()



/*
 * place_all_execution_slots()
 *
 */

void Socket::place_all_execution_slots(
    
  req        &r,
  allocation &task_alloc)
  
  {
  this->socket_exclusive = true;

  for (unsigned int i = 0; i < this->chips.size(); i++)
    this->chips[i].place_all_execution_slots(r, task_alloc);
  } // END place_all_execution_slots()


bool Socket::spread_place_pu(

  req         &r,
  allocation  &task_alloc,
  int         &pu_per_task_remaining,
  int         &lprocs_per_task_remaining,
  int         &gpus_remaining,
  int         &mics_remaining)

  {
  bool placed = false;

  for (unsigned int i = 0; i < this->chips.size(); i++)
    {
    bool fits = false;

    if (task_alloc.place_type == exclusive_core)
      fits = this->chips[i].spread_place_cores(r, task_alloc, pu_per_task_remaining, lprocs_per_task_remaining,
                                                 gpus_remaining, mics_remaining);
    else if (task_alloc.place_type == exclusive_thread)
      fits = this->chips[i].spread_place_threads(r, task_alloc, pu_per_task_remaining, lprocs_per_task_remaining,
                                                 gpus_remaining, mics_remaining);
    if (fits == true)
      {
      if ((pu_per_task_remaining == 0) && (lprocs_per_task_remaining == 0))
        {
        placed = true;
        break;
        }
      }
    }

  return(placed);
  }


/*
 * spread_place()
 *
 * @param r - the req we're placing
 * @param master - the master allocation for the whole job
 * @param execution_slots_per - the number of execution slots to reserve for the socket
 * @param execution_slots_remainder - extra execution slots that need to be acquired
 */

bool Socket::spread_place(

  req        &r,
  allocation &task_alloc,
  int         execution_slots_per,
  int        &execution_slots_remainder,
  bool        chip)

  {
  bool placed = false;
  int  numa_nodes_required  = 1;
  int  per_numa = execution_slots_per;
  int  per_numa_remainder = 0;

  // We must either be completely free or be placing on just one chip
  if ((this->is_completely_free()) ||
      (chip == true))
    {
    if (chip == false)
      {
      // If we're placing at the socket level, divide execution_slots_per by the number 
      // of chips and place multiple times
      per_numa_remainder = per_numa % this->chips.size();
      per_numa /= this->chips.size();
      numa_nodes_required = this->chips.size();
    
      this->socket_exclusive = true;
      }

    for (int c = 0; c < numa_nodes_required; c++)
      {
      for (unsigned int i = 0; i < this->chips.size(); i++)
        {
        if (per_numa_remainder > 0)
          {
          if (this->chips[i].spread_place(r, task_alloc, per_numa + 1, execution_slots_remainder))
            {
            placed = true;
            per_numa_remainder--;
            break;
            }
          }
        else
          {
          if (this->chips[i].spread_place(r, task_alloc, per_numa, execution_slots_remainder))
            {
            placed = true;
            break;
            }
          }
        }
      }
    }

  return(placed);
  } // END spread_place()



/*
 * place_task()
 *
 * Places as many tasks as available, up to to_place, on this socket.
 * @param r - the req we're pulling tasks from
 * @param master - the allocation for the entire job
 * @param to_place - the maximum number of tasks that should be placed
 * @param hostname - the name of the host where we're placing
 * @return the number of tasks placed
 */

int Socket::place_task(

  req        &r,
  allocation &master,
  int         to_place,
  const char *hostname)

  {
  int        tasks_to_place = to_place;
  allocation a(master.jobid.c_str());

  a.place_type = master.place_type;

  if ((master.place_type != exclusive_socket) ||
      (this->is_available() == true))
    {
    if (this->socket_exclusive == false)
      {
      // Attempt to fit all tasks from this req on a single numa chip if possible
      for (unsigned int i = 0; i < this->chips.size() && tasks_to_place > 0; i++)
        {
        if (this->chips[i].how_many_tasks_fit(r, master.place_type) >= to_place)
          {
          int placed = this->chips[i].place_task(r, a, to_place, hostname);
          tasks_to_place -= placed;

          if ((placed != 0) &&
              (master.place_type == exclusive_socket))
            break;
          }
        }

      // Place remaining tasks
      if (tasks_to_place > 0)
        {
        // This loop places all tasks from the req that fit inside a single chip
        for (unsigned int i = 0; i < this->chips.size() && tasks_to_place > 0; i++)
          {
          int placed = this->chips[i].place_task(r, a, tasks_to_place, hostname);
          tasks_to_place -= placed;
          
          if ((placed != 0) &&
              (master.place_type == exclusive_socket))
            break;
          }

        // Finally, place tasks that must span numa chips. Let's only try if there's more than
        // one chip
        if (this->chips.size() > 1)
          {
          while (tasks_to_place > 0)
            {
            allocation task_alloc(master.jobid.c_str());
            allocation remaining(r);

            std::vector<int> partially_placed_indices;

            task_alloc.cores_only = master.cores_only;

            for (unsigned int i = 0; i < this->chips.size() && remaining.fully_placed() == false; i++)
              {
              if (this->chips[i].partially_place_task(remaining, task_alloc) == true)
                partially_placed_indices.push_back(i);
              }

            if (remaining.fully_placed() == true)
              {
              tasks_to_place--;
              task_alloc.set_host(hostname);
              a.add_allocation(task_alloc);
              r.record_allocation(task_alloc);
              }
            else
              {
              if (remaining.partially_placed(r) == true)
                {
                // Just remove the last task in order to not remove other, valid tasks from the chip
                for (size_t i = 0; i < partially_placed_indices.size(); i++)
                  this->chips[partially_placed_indices[i]].remove_last_allocation(master.jobid.c_str());
                }

              // We aren't going to make any more progress, move on
              break;
              }
            }
          }
        }

      if (to_place != tasks_to_place)
        {
        this->availableCores -= a.cores;
        this->availableThreads -= a.threads;
        this->available_memory -= a.memory;

        if (master.place_type == exclusive_socket)
          this->socket_exclusive = true;

        master.add_allocation(a);
        }
      }
    }

  return(to_place - tasks_to_place);
  } // END place_task()



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
    this->socket_exclusive = false;

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
  return(!this->socket_exclusive);
  } // END is_available()



/*
 * is_completely_free()
 *
 * @return true if there are no jobs using any part of this socket
 */

bool Socket::is_completely_free() const
  {
  bool completely_free = true;
  for (unsigned int i = 0; i < this->chips.size(); i++)
    {
    if (this->chips[i].is_completely_free() == false)
      {
      completely_free = false;
      break;
      }
    }

  return(completely_free);
  } // END is_completely_free()



/*
 * fits_on_socket()
 * Determines if the task identified by remaining can fit completely on this socket
 * by memory and execution slots
 *
 * @param remaining - an allocation with the information for whats left for the task
 * @return - true if the allocation will completely fit on this socket, false otherwise
 */

bool Socket::fits_on_socket(
    
  const allocation &remaining) const

  {
  bool fits = false;
  if (this->getAvailableMemory() >= remaining.memory)
    {
    int max_cpus = remaining.cpus;
    if (remaining.place_cpus > 0)
      max_cpus = remaining.place_cpus;

    if ((remaining.cores_only == true) &&
        (this->get_free_cores() >= max_cpus))
      fits = true;
    else if (remaining.place_type == exclusive_legacy)
      {
      if (this->get_free_cores() >= max_cpus)
        fits = true;
      }
    else if ((remaining.cores_only == false) &&
             (this->getAvailableThreads() >= max_cpus))
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
 * @param task_alloc (O) - the complete allocation for this task
 * @return - true if the task was fully placed, false if partially placed
 */

bool Socket::partially_place(

  allocation &remaining,
  allocation &task_alloc)

  {
  bool fully_placed = false;

  for (unsigned int i = 0; i < this->chips.size(); i++)
    {
    this->chips[i].partially_place_task(remaining, task_alloc);

    if (remaining.fully_placed() == true)
      {
      fully_placed = true;
      break;
      }
    }

  return(fully_placed);
  } // END partially_place()



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

int Socket::get_gpus_remaining()
  {
  int gpus_available = 0;
  for (unsigned int i = 0; i < this->chips.size(); i++)
    {
    gpus_available += this->chips[i].get_available_gpus();
    }

  return(gpus_available);
  }

int Socket::get_mics_remaining()
  {
  int mics_available = 0;
  for (unsigned int i = 0; i < this->chips.size(); i++)
    {
    mics_available += this->chips[i].get_available_mics();
    }

  return(mics_available);
  }


#endif /* PENABLE_LINUX_CGROUPS */  
