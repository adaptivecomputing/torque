#include <string>
#include <vector>
#include <map>
#include <errno.h>

#include "pbs_config.h"
#include "log.h"

#ifdef PENABLE_LINUX_CGROUPS
#include <hwloc.h>

#ifdef MIC
#include <hwloc/intel-mic.h>
#endif

#ifdef NVIDIA_GPUS
#ifdef NVML_API
#include <hwloc/nvml.h>
#include <nvml.h>

#endif
#endif

#include "machine.hpp"
#include "pbs_error.h"
#include "log.h"
#include "complete_req.hpp"
#include "utils.h"

using namespace std;

const int   ALL_TASKS = -1;

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

  hardwareStyle = newMachine.hardwareStyle;
  totalSockets = newMachine.totalSockets;
  sockets = newMachine.sockets;
  totalMemory = newMachine.totalMemory;
  totalChips = newMachine.totalChips;
  totalCores = newMachine.totalCores;
  totalThreads = newMachine.totalThreads;
  availableSockets = newMachine.availableSockets;
  availableChips = newMachine.availableChips;
  availableCores = newMachine.availableCores;
  availableThreads = newMachine.availableThreads;
  return *this;
  }



/*
 * update_internal_counts()
 *
 * Populates my allocations vector and gets sockets to update it's counts
 */

void Machine::update_internal_counts()

  {
  std::vector<allocation> allocs;

  for (unsigned int i = 0; i < this->sockets.size(); i++)
    this->sockets[i].update_internal_counts(allocs);

  for (unsigned int i = 0; i < allocs.size(); i++)
    this->allocations.push_back(allocs[i]);

  this->availableSockets = 0;
  this->totalChips = 0;
  this->totalCores = 0;
  this->totalThreads = 0;

  for (unsigned int i = 0; i < this->sockets.size(); i++)
    {
    if (this->sockets[i].is_available())
      this->availableSockets++;

    this->totalChips += this->sockets[i].getTotalChips();
    this->totalCores += this->sockets[i].getTotalCores();
    this->totalThreads += this->sockets[i].getTotalThreads();
    }

  } // END update_internal_counts()



/*
 * Builds a copy of the machine's layout in from json which has no whitespace but 
 * if it did it'd look like:
 *
 * {"node" : {
 *   "socket" : {
 *     "os_index" : <index>,
 *     "numanode" : {
 *       "os_index" : <index>,
 *       "cores" : <core range string>,
 *       "threads" : <thread range string>,
 *       "mem" : <memory in kb>,
 *       "gpus" : <gpu range string>,
 *       "mics" : <mic range string>
 *       }
 *     [, "numanode" ... ]
 *     }
 *   [, "socket" ...]
 *   }
 * }
 *
 * mics and gpus are optional and only present if they actually exist on the node
 *
 */

Machine::Machine(const std::string &json_layout,
                 std::vector<std::string> &valid_ids) : hardwareStyle(0), totalMemory(0),
                                                        totalSockets(0), totalChips(0),
                                                        totalCores(0), totalThreads(0), 
                                                        availableSockets(0), availableChips(0),
                                                        availableCores(0), availableThreads(0),
                                                        sockets(), NVIDIA_device(), allocations()

  {
  const char *socket_str = "\"socket\":{";
  std::size_t socket_begin = json_layout.find(socket_str);

  while (socket_begin != std::string::npos)
    {
    std::size_t next = json_layout.find(socket_str, socket_begin + 1);
    std::string one_socket = json_layout.substr(socket_begin, next - socket_begin);

    Socket s(one_socket, valid_ids);
    this->sockets.push_back(s);
    this->totalSockets++;

    socket_begin = next;
    }

  update_internal_counts();
  }



Machine::Machine(

  int np,
  int numa_nodes,
  int sockets) : hardwareStyle(0), totalMemory(0), totalSockets(sockets), totalChips(numa_nodes),
                 totalCores(np), totalThreads(np), availableSockets(sockets),
                 availableChips(numa_nodes), availableCores(np), availableThreads(np), sockets(),
                 NVIDIA_device(), allocations()

  {
  int np_remainder = np % sockets;
  for (int i = 0; i < sockets; i++)
    {
    Socket s(np / sockets, numa_nodes / sockets, np_remainder);
    this->sockets.push_back(s);
    }
  
  memset(allowed_cpuset_string, 0, MAX_CPUSET_SIZE);
  memset(allowed_nodeset_string, 0, MAX_NODESET_SIZE);
  }



Machine::Machine() : hardwareStyle(0), totalMemory(0), totalSockets(0), totalChips(0),
                     totalCores(0), totalThreads(0), availableSockets(0), availableChips(0),
                     availableCores(0), availableThreads(0), sockets(), NVIDIA_device(),
                     allocations()
  { 
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
  /* because we do not have NUMA hardware set socket to 1 and numa_node to 1 */
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

  return(PBSE_NONE);
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
    return(PBSE_SYSTEM);
    }
  
  /* We have the machine object. Fill in what we know */
  totalMemory = obj->memory.total_memory / 1024;
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
#ifdef NVIDIA_GPUS
    initializeNVIDIADevices(obj, topology);
#endif
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
      this->sockets.push_back(intel_socket);
      this->hardwareStyle = style;
      }
    else if (style == AMD)
      {
      Socket amd_socket;
      amd_socket.initializeAMDSocket(socket_obj, topology);
      this->sockets.push_back(amd_socket);
      this->hardwareStyle = style;
      }
    else
      {
      /* Something failed. Return the error */
      return(style);
      }

    prev = socket_obj;
    }
  
#ifdef NVIDIA_GPUS
    initializeNVIDIADevices(obj, topology);
#endif
  return(PBSE_NONE);
  }

hwloc_uint64_t Machine::getTotalMemory() const
  {
  return(this->totalMemory);
  }

hwloc_uint64_t Machine::getAvailableMemory() const
  {
  hwloc_uint64_t available = 0;

  for (size_t i = 0; i < this->sockets.size(); i++)
    available += this->sockets[i].getAvailableMemory();

  return(available);
  }

int Machine::getTotalSockets() const
  {
  return(this->totalSockets);
  }

int Machine::get_total_gpus() const
  {
  int total_gpus = 0;

  for (size_t i = 0; i < this->sockets.size(); i++)
    total_gpus += this->sockets[i].get_total_gpus();

  return(total_gpus);
  }

int Machine::getTotalChips() const
  {
  return(this->totalChips);
  }

int Machine::getTotalCores() const
  {
  return(this->totalCores);
  }

int Machine::getTotalThreads() const
  {
  return(this->totalThreads);
  }

int Machine::getAvailableSockets() const
  {
  return(this->availableSockets);
  }

int Machine::getDedicatedSockets() const
  {
  return(this->totalSockets - this->availableSockets);
  }

int Machine::getAvailableChips() const
  {
  int available = 0;
 
  for (unsigned int i = 0; i < this->sockets.size(); i++)
    available += this->sockets[i].getAvailableChips();

  return(available);
  }

int Machine::getDedicatedChips() const
  {
  return(this->totalChips - this->getAvailableChips());
  }

int Machine::getAvailableCores() const
  {
  int available = 0;

  for (unsigned int i = 0; i < this->sockets.size(); i++)
    available += this->sockets[i].getAvailableCores();

  return(available);
  }

int Machine::getDedicatedCores() const
  {
  return(this->totalCores - this->getAvailableCores());
  }

int Machine::getAvailableThreads() const
  {
  int available = 0;

  for (unsigned int i = 0; i < this->sockets.size(); i++)
    available += this->sockets[i].getAvailableThreads();

  return(available);
  }

int Machine::getDedicatedThreads() const
  {
  return(this->totalThreads - this->getAvailableThreads());
  }

void Machine::displayAsJson(
    
  stringstream &out,
  bool          include_jobs) const

  {
  out << "{\"node\":{";

  for (unsigned int i = 0; i < this->sockets.size(); i++)
    {
    if (i > 0)
      out << ",";
    this->sockets[i].displayAsJson(out, include_jobs);
    }

  out << "}}";
  }

int Machine::getHardwareStyle() const
  {
  return(this->hardwareStyle);
  }


void Machine::displayAsString(
    
  stringstream &out) const

  {
  out << "Machine (" << this->totalMemory << "KB)\n";
  
  for (unsigned int i = 0; i < this->sockets.size(); i++)
    this->sockets[i].displayAsString(out);
  
  for (unsigned int i = 0; i < this->NVIDIA_device.size(); i++)
    this->NVIDIA_device[i].displayAsString(out);
  }
  
void Machine::setMemory(
    
  long long mem)

  {
  long long per_socket = mem / this->sockets.size();
  this->totalMemory = mem;

  for (unsigned int i = 0; i < this->sockets.size(); i++)
    this->sockets[i].setMemory(per_socket);
  }

void Machine::insertNvidiaDevice(

  PCI_Device& device)
  {
  this->NVIDIA_device.push_back(device);
  }

// This is meant to only be used for unit tests
void Machine::addSocket(

  int count)

  {
  for (int i = 0; i < count; i++)
    {
    Socket s;
    this->sockets.push_back(s);
    this->totalSockets++;
    }
  }

// This is meant to only be used for unit tests
void Machine::setIsNuma(

  bool is_numa)

  {
  this->isNUMA = is_numa;
  }


void Machine::place_all_execution_slots(
  
  req        &r,
  allocation &master,
  const char *hostname)

  {
  allocation task_alloc(master.jobid.c_str());

  task_alloc.set_host(hostname);
  task_alloc.cores_only = (r.getThreadUsageString() == use_cores);
    
  for (unsigned int i = 0; i < this->sockets.size(); i++)
    {
    this->sockets[i].place_all_execution_slots(r, task_alloc);
    this->availableSockets--;
    }

  task_alloc.place_type = exclusive_node;

  r.record_allocation(task_alloc);
  master.add_allocation(task_alloc);
  } // END place_all_execution_slots()



/*
 * compare_remaining_values()
 *
 * Looks at what couldn't be placed in the allocation remaining and logs an appropriate message.
 * @param remaining - the allocation that couldn't be completely placed.
 * @param caller - the name of the calling function.
 */

void Machine::compare_remaining_values(

  allocation &remaining,
  const char *caller) const

  {
  char        log_buf[LOCAL_LOG_BUF_SIZE];
  std::string error;

  if (remaining.cpus != 0)
    {
    sprintf(log_buf, "Couldn't place %d cpus, have %d/%d cores and threads available. ",
      remaining.cpus, this->getAvailableCores(), this->getAvailableThreads());
    error += log_buf;
    }

  if (remaining.memory != 0)
    {
    sprintf(log_buf, "Couldn't place %lukb memory, have %lukb available. ",
      remaining.memory, (unsigned long)this->getAvailableMemory());
    error += log_buf;
    }

  if (remaining.mics != 0)
    {
    sprintf(log_buf, "Couldn't place %d mics. ", remaining.mics);
    error += log_buf;
    }

  if (remaining.gpus != 0)
    {
    sprintf(log_buf, "Couldn't place %d gpus. ", remaining.gpus);
    error += log_buf;
    }

  if (remaining.place_cpus != 0)
    {
    sprintf(log_buf, "Couldn't place %d place cpus, have %d/%d cores and threads available. ",
      remaining.place_cpus, this->getAvailableCores(), this->getAvailableThreads());
    error += log_buf;
    }
  
  log_err(-1, caller, error.c_str());
  } // END compare_remaining_values()



/*
 * place_remaining()
 *
 * Places any tasks remaining - these tasks don't fit within a single numa node
 *
 * @param r - the req we're placing
 * @param master - the recording allocation for the entire job
 * @param remaining_tasks - the number of tasks remaining to be placed for this req
 */

void Machine::place_remaining(

  req         &r,
  allocation  &master,
  int         &remaining_tasks,
  const char  *hostname)

  {
  // This will take care of any that still fit within a socket
  while (remaining_tasks > 0)
    {
    bool fit_somewhere = false;
    allocation remaining(r);
    allocation task_alloc(master.jobid.c_str());

    task_alloc.cores_only = master.cores_only;

      
    for (unsigned int j = 0; j < this->sockets.size(); j++)
      {
      if (this->sockets[j].fits_on_socket(remaining) == false)
        continue;

      if (remaining.place_type == exclusive_socket)
        this->availableSockets--;

      this->sockets[j].partially_place(remaining, task_alloc);

      task_alloc.set_host(hostname);
      r.record_allocation(task_alloc);
      master.add_allocation(task_alloc);
      fit_somewhere = true;
      break;
      }

    // Exit the loop if the task doesn't fit on any socket individually
    if (fit_somewhere == false)
      break;

    remaining_tasks--;
    }

  // Finally, place any that don't fit on a single socket
  while (remaining_tasks > 0)
    {
    bool fit_somewhere = false;
    allocation remaining(r);
    allocation task_alloc(master.jobid.c_str());
    
    // At this point, we want to use cores or threads, whatever is available. (exclusive_legacy
    // will only use cores.) if it is set to exclusive_legacy let it get cores first. We will get 
    // threads later

    /* this is for legacy jobs. */
    if ((master.cpus != 0) &&
        (master.place_type == exclusive_legacy2))
      remaining.cpus = master.cpus;

    for (unsigned int j = 0; j < this->sockets.size(); j++)
      {
      if (remaining.place_type == exclusive_socket)
        this->availableSockets--;

      if (this->sockets[j].partially_place(remaining, task_alloc) == true)
        {
        fit_somewhere = true;

        task_alloc.set_host(hostname);
        r.record_allocation(task_alloc);
        master.add_allocation(task_alloc);
        break;
        }
      }

    /* This piece of code is meant to finish for the legacy -l requests 
       where the number of ppn requested is greater than the number of
       cores on a node. If we enter this section we used up all of the cores 
       and now we need to grab threads.
     */
    if ((master.place_type == exclusive_legacy) && (remaining.cpus > 0))
      {
      r.set_placement_type(place_legacy2);
      master.set_place_type(place_legacy2);
      master.cpus = remaining.cpus;
      remaining.place_type = master.place_type;

      for (unsigned int j = 0; j < this->sockets.size(); j++)
        {
        if (remaining.place_type == exclusive_socket)
          this->availableSockets--;

        if (this->sockets[j].partially_place(remaining, task_alloc) == true)
          {
          fit_somewhere = true;

          task_alloc.set_host(hostname);
          r.record_allocation(task_alloc);
          master.add_allocation(task_alloc);
          break;
          }
        }

      /* we need to set the req back to its original place_type incase there is another node for this req */
      r.set_placement_type(place_legacy);
      master.set_place_type(place_legacy);
      }

    // If it didn't get placed, log what prevented it from being placed
    if (fit_somewhere == false)
      {
      compare_remaining_values(remaining, __func__);
      break;
      }
   
    remaining_tasks--;
    }

  } // END place_remaining()


int Machine::spread_place_pu(

  req        &r,
  allocation &master,
  int         tasks_for_node,
  const char *hostname)

  {
  int   pu_per_task = 0;
  int   lprocs_per_task = r.getExecutionSlots();
  int   tasks_placed = 0;
  int   tasks_for_this_node = tasks_for_node;

  if (pu_per_task > this->totalCores)
    return(PBSE_IVALREQ);


  if (master.place_type == exclusive_core)
    pu_per_task = r.getPlaceCores();
  else if (master.place_type == exclusive_thread)
    pu_per_task = r.getPlaceThreads();

  for (unsigned int i = 0; i < tasks_for_node; i++)
    {
    int gpus_remaining;
    int mics_remaining;
    int pu_per_task_remaining;
    int lprocs_per_task_remaining;
    bool partial_place = false;
    allocation task_alloc(master.jobid.c_str());
    task_alloc.set_place_type(r.getPlacementType());

    pu_per_task_remaining = pu_per_task;
    lprocs_per_task_remaining = lprocs_per_task;
    gpus_remaining = r.get_gpus();
    mics_remaining = r.getMics();

    for (unsigned int j = 0; j < this->totalSockets; j++)
      {
      if (this->sockets[j].how_many_tasks_fit(r, master.place_type) < tasks_for_this_node)
        continue;

      bool fits = false;

      fits = this->sockets[j].spread_place_pu(r, task_alloc, pu_per_task_remaining, lprocs_per_task_remaining,
                                              gpus_remaining, mics_remaining);
      if (fits == true)
        {
        if ((pu_per_task_remaining == 0) && (lprocs_per_task_remaining == 0) &&
             (gpus_remaining == 0) && (mics_remaining == 0))
          {
          partial_place = true;
          break;
          }
        }
      }

    if (partial_place == true)
      tasks_placed++;
    else
      {
      for (unsigned int j = 0; j < this->totalSockets; j++)
        {
        bool fits = false;

        fits = this->sockets[j].spread_place_pu(r, task_alloc, pu_per_task_remaining, lprocs_per_task_remaining,
                                              gpus_remaining, mics_remaining);
        if (fits == true)
          {
          if ((pu_per_task_remaining == 0) && (lprocs_per_task_remaining == 0) &&
             (gpus_remaining == 0) && (mics_remaining == 0))
            {
            partial_place = true;
            tasks_placed++;
            break;
            }
          }
        }
      }


    task_alloc.set_host(hostname);
    r.record_allocation(task_alloc);
    master.add_allocation(task_alloc);
    tasks_for_this_node--;
    }

  if (tasks_placed != tasks_for_node)
    return(PBSE_IVALREQ);

  return(PBSE_NONE);
  } /* END spread_place_pu() */



/*
 * spread_place()
 *
 * Places r tasks_for_node times, spreading it appropriately
 *
 * @param r - the req we're placing
 * @param master - the master allocation
 * @param tasks_for_node - the number of times we should place r
 * @return PBSE_NONE on success
 */

int Machine::spread_place(

  req        &r,
  allocation &master,
  int         tasks_for_node,
  const char *hostname)

  {
  bool chips = false;
  int  quantity = r.get_sockets();
  int  tasks_placed = 0;
  char log_buf[LOCAL_LOG_BUF_SIZE];

  if (quantity == 0)
    {
    chips = true;
    quantity = r.get_numa_nodes();
    }
 
  // Handle the case for place=node
  if (quantity == 0)
    {
    quantity = this->sockets.size();
    chips = false;
    }
  else
    {
    // Make sure we are grabbing enough memory
    unsigned long mem_needed = r.get_memory_per_task();
    unsigned long mem_count = 0;
    int           mem_quantity = 0;

    for (size_t i = 0; i < this->sockets.size() && mem_needed > mem_count; i++)
      {
      if (chips == true)
        {
        unsigned long diff = mem_needed - mem_count;
        int           numa_nodes_required = 0;
        mem_count += this->sockets[i].get_memory_for_completely_free_chips(diff, numa_nodes_required);
        mem_quantity += numa_nodes_required;
        }
      else if (this->sockets[i].is_completely_free())
        {
        mem_count += this->sockets[i].getMemory();

        mem_quantity++;
        }

      if (mem_quantity > quantity)
        quantity = mem_quantity;
      }

    // If we don't have enough memory, reject the job
    if (mem_needed > mem_count)
      {
      snprintf(log_buf, sizeof(log_buf),
        "Job %s requires at least %lukb memory from host %s, but only %lukb is available",
        master.jobid.c_str(), mem_needed, hostname, mem_count);
      log_err(-1, __func__, log_buf);

      return(PBSE_RESCUNAV);
      }
    }

  // Spread the placement evenly across the number of sockets or numa nodes
  for (int i = 0; i < tasks_for_node; i++)
    {
    bool partial_place = false;
    allocation task_alloc(master.jobid.c_str());
    task_alloc.set_place_type(r.getPlacementType());
    allocation remaining(r);
    allocation remainder(r);

    remaining.adjust_for_spread(quantity, false);
    remainder.adjust_for_spread(quantity, true);

    for (int j = 0; j < quantity; j++)
      {

      for (unsigned int s = 0; s < this->sockets.size(); s++)
        {
        if (this->sockets[s].spread_place(r, task_alloc, remaining, remainder, chips))
          {
          partial_place = true;

          if ((task_alloc.place_type == exclusive_socket) ||
              (task_alloc.place_type == exclusive_node))
            this->availableSockets--;

          break;
          }
        }

      if (partial_place == false)
        break;
      }

    if (partial_place == true)
      {
      tasks_placed++;

      task_alloc.set_host(hostname);
      r.record_allocation(task_alloc);
      master.add_allocation(task_alloc);
      }
    }

  if (tasks_placed != tasks_for_node)
    {
    snprintf(log_buf, sizeof(log_buf),
      "Job %s needed to place %d tasks on node %s, but could only place %d",
      master.jobid.c_str(), tasks_for_node, hostname, tasks_placed);
    log_err(-1, __func__, log_buf);

    return(PBSE_IVALREQ);
    }

  return(PBSE_NONE);
  } // END spread_place()



int Machine::fit_tasks_within_sockets(
    
  req        &r,
  allocation &job_alloc,
  const char *hostname,
  int        &remaining_tasks)

  {
  for (unsigned int i = 0; i < this->sockets.size() && remaining_tasks > 0; i++)
    {
    int placed = this->sockets[i].place_task(r, job_alloc, remaining_tasks, hostname);
    if (placed != 0)
      {
      remaining_tasks -= placed;
      
      if (job_alloc.place_type == exclusive_socket)
        this->availableSockets--;
      }
    }

  return(PBSE_NONE);
  } // END fit_tasks_within_sockets()



/*
 * place_job()
 *
 * @param pjob - 
 * @param cpu_string - 
 * @param mem_string -
 * @param hostname - 
 * @return PBSE_NONE on success
 */

int Machine::place_job(

  job         *pjob,
  cgroup_info &cgi,
  const char  *hostname,
  bool         legacy_vmem)

  {
  int rc = PBSE_NONE;

  complete_req *cr = (complete_req *)pjob->ji_wattr[JOB_ATR_req_information].at_val.at_ptr;
  int           num_reqs = cr->req_count();
  vector<int>   partially_place;
  allocation    job_alloc(pjob->ji_qs.ji_jobid);
  vector<req>   to_split;
  char          log_buf[LOCAL_LOG_BUF_SIZE];

  // See if the tasks fit completely on a socket, and if they do then place them there
  for (int i = 0; i < num_reqs; i++)
    {
    req  &r = cr->get_req(i);
    int   tasks_for_node = r.get_num_tasks_for_host(hostname);
    bool  placed = false;

    if (tasks_for_node == 0)
      continue;
      
    // This only has to be correct while placing. If it changes later it won't cause problems.
    job_alloc.set_place_type(r.getPlacementType());

    if (r.get_execution_slots() == ALL_EXECUTION_SLOTS)
      {
      place_all_execution_slots(r, job_alloc, hostname);
      }
    else if ((job_alloc.place_type == exclusive_node) ||
             (job_alloc.place_type == exclusive_socket) ||
             (job_alloc.place_type == exclusive_chip))
      {
      if ((rc = spread_place(r, job_alloc, tasks_for_node, hostname)) != PBSE_NONE)
        return(rc);
      }
    else
      {
      for (unsigned int j = 0; j < this->sockets.size(); j++)
        {
        if (this->sockets[j].how_many_tasks_fit(r, job_alloc.place_type) >= tasks_for_node)
          {
          // place the req entirely on this socket
          placed = true;
          if (job_alloc.place_type == exclusive_socket)
            this->availableSockets--;

          // Placing 0 tasks is an error
          if (this->sockets[j].place_task(r, job_alloc, tasks_for_node, hostname) == 0)
            {
            snprintf(log_buf, sizeof(log_buf),
              "Socket %u for node %s was thought to fit at least %d tasks, but couldn't fit any",
              j, hostname, tasks_for_node);
            log_err(-1, __func__, log_buf);

            return(-1);
            }

          break;
          }
        }

      if (placed == false)
        {
        // mark this as a task that must be partially placed
        partially_place.push_back(i);
        }
      }
    }

  // If any reqs were marked to be placed partially in different sockets then 
  // place them now
  for (unsigned int i = 0; i < partially_place.size(); i++)
    {
    req  &r = cr->get_req(partially_place[i]);
    int   remaining_tasks = r.get_num_tasks_for_host(hostname);
    
    // This only has to be correct while placing. If it changes later it won't cause problems.
    job_alloc.set_place_type(r.getPlacementType());
    
    this->fit_tasks_within_sockets(r, job_alloc, hostname, remaining_tasks);

    if (remaining_tasks > 0)
      {
      // exclusive_legacy will place only using cores. exclusive_legacy2 will use threads. If
      // we can't place using only cores, then we should try threads before partially placing
      // the job, as well as during.
      // At this point, all of the tasks that fit within 1 numa node have been placed.
      // Now place any leftover tasks
      place_remaining(r, job_alloc, remaining_tasks, hostname);
      }

    if (remaining_tasks > 0)
      {
      // this allocation so that the cleanup happens correctly
      snprintf(log_buf, sizeof(log_buf),
        "Job %s could not place %d tasks from req %d on host %s",
        pjob->ji_qs.ji_jobid, remaining_tasks, partially_place[i], hostname);
      log_err(-1, __func__, log_buf);

      rc = -1;
      }
    }

  job_alloc.place_indices_in_string(cgi);

  this->allocations.push_back(job_alloc);
  
  return(rc);
  } // END place_job()



void Machine::free_job_allocation(

  const char *job_id)

  {
  std::vector<int> to_remove;

  for (unsigned int i = 0; i < this->sockets.size(); i++)
    {
    bool previously_used = this->sockets[i].is_available() == false;

    if ((this->sockets[i].free_task(job_id) == true) &&
        (previously_used == true))
      this->availableSockets++;
    }

  // Remove from my allocations
  for (unsigned int i = 0; i < this->allocations.size(); i++)
    if (this->allocations[i].jobid == job_id)
      to_remove.push_back(i);
  
  for (size_t i = 0; i < to_remove.size(); i++)
    // Subtract i because we are dynamically changing the vector as we erase, removing 1 element
    // each time
    this->allocations.erase(this->allocations.begin() + to_remove[i] - i);
  } // END free_job_allocation()



void Machine::store_device_on_appropriate_chip(
    
  PCI_Device &device,
  bool        no_info)

  {
  if ((this->isNUMA == false) ||
      (no_info == true))
    {
    this->sockets[0].store_pci_device_appropriately(device, true);
    }
  else
    {
    for (unsigned int i = 0; i < this->sockets.size(); i++)
      {
      if (this->sockets[i].store_pci_device_appropriately(device, false))
        break;
      }
    }
  }



void Machine::populate_job_ids(

  std::vector<std::string> &job_ids) const

  {
  for (unsigned int i = 0; i < this->allocations.size(); i++)
    job_ids.push_back(this->allocations[i].jobid);
  } // END populate_job_ids()



bool Machine::check_if_possible(

  int &sockets,
  int &numa_nodes,
  int &cores,
  int &threads) const

  {
  bool possible = true;

  if (this->totalSockets >= sockets)
    sockets = 0;
  else
    possible = false;

  if (this->totalChips >= numa_nodes)
    numa_nodes = 0;
  else
    possible = false;

  if (this->totalCores >= cores)
    cores = 0;
  else
    possible = false;

  if (this->totalThreads >= threads)
    threads = 0;
  else
    possible = false;

  return(possible);
  } // END check_if_possible()



int Machine::how_many_tasks_can_be_placed(

  req &r) const

  {
  float         can_place = 0.0;
  allocation    a;
  int           sockets = r.get_sockets();
  a.set_place_type(r.getPlacementType());

  if (sockets > 1)
    {
    for (unsigned int j = 0; j < this->totalSockets; j++)
      if (this->sockets[j].how_many_tasks_fit(r, a.place_type) > 0)
        can_place += 1;

    can_place /= sockets;
    }
  else
    {
    for (unsigned int j = 0; j < this->totalSockets; j++)
      can_place += this->sockets[j].how_many_tasks_fit(r, a.place_type);
    }

  return(can_place);
  } // END place_as_many_as_possible()



void Machine::save_allocations(

  const Machine &other)

  {
  this->allocations = other.allocations;

  for (size_t s = 0; s < other.sockets.size() && s < this->sockets.size(); s++)
    {
    this->sockets[s].save_allocations(other.sockets[s]);
    }
  } // END save_allocations()



#endif /* PENABLE_LINUX_CGROUPS */
