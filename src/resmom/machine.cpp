#include <string>
#include <vector>
#include <map>
#include <errno.h>
#include "pbs_config.h"

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
#include "mom_server_lib.h"
#include "complete_req.hpp"

using namespace std;

const int   ALL_TASKS = -1;
extern char mom_alias[];


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

Machine::Machine() : totalMemory(0), totalSockets(0), totalChips(0), totalCores(0),
                     totalThreads(0), availableSockets(0), availableChips(0),
                     availableCores(0), availableThreads(0)
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

  return(PBSE_NONE);
  }

#ifdef NVIDIA_GPUS
hwloc_obj_t Machine::get_non_nvml_device(hwloc_topology_t topology, nvmlDevice_t device)
  {
  hwloc_obj_t osdev;
  nvmlReturn_t nvres;
  nvmlPciInfo_t pci;

  if (!hwloc_topology_is_thissystem(topology)) 
    {
    errno = EINVAL;
    return NULL;
    }

    nvres = nvmlDeviceGetPciInfo(device, &pci);
    if (NVML_SUCCESS != nvres)
      return NULL;

    osdev = NULL;
    while ((osdev = hwloc_get_next_osdev(topology, osdev)) != NULL) 
      {
      hwloc_obj_t pcidev = osdev->parent;
      if (strncmp(osdev->name, "card", 4))
        continue;
      if (pcidev
          && pcidev->type == HWLOC_OBJ_PCI_DEVICE
          && pcidev->attr->pcidev.domain == pci.domain
          && pcidev->attr->pcidev.bus == pci.bus
          && pcidev->attr->pcidev.dev == pci.device
          && pcidev->attr->pcidev.func == 0)
        return osdev;
      }

  return(NULL);
  }


int Machine::initializeNVIDIADevices(hwloc_obj_t machine_obj, hwloc_topology_t topology)
  {
  nvmlReturn_t rc;

  /* Initialize the NVML handle. 
   *
   * nvmlInit should be called once before invoking any other methods in the NVML library. 
   * A reference count of the number of initializations is maintained. Shutdown only occurs 
   * when the reference count reaches zero.
   * */
  rc = nvmlInit();
  if (rc != NVML_SUCCESS && rc != NVML_ERROR_ALREADY_INITIALIZED)
    {
    log_nvml_error(rc, NULL, __func__);
    return(PBSE_NONE);
    }

  unsigned int device_count = 0;

  /* Get the device count. */
  rc = nvmlDeviceGetCount(&device_count);
  if (rc == NVML_SUCCESS)
    {
    nvmlDevice_t gpu;

    /* Get the nvml device handle at each index */
    for (unsigned int idx = 0; idx < device_count; idx++)
      {
      rc = nvmlDeviceGetHandleByIndex(idx, &gpu);

      if (rc != NVML_SUCCESS)
        {
        /* TODO: get gpuid from nvmlDevice_t struct */
        log_nvml_error(rc, NULL, __func__);
        }

      /* Use the hwloc library to determine device locality */
      hwloc_obj_t gpu_obj;
      hwloc_obj_t ancestor_obj;
      int is_in_tree;
  
      gpu_obj = hwloc_nvml_get_device_osdev(topology, gpu);
      if (gpu_obj == NULL)
        {
        /* This was not an nvml device. We will look for a "card" device (GeForce or Quadra) */
        gpu_obj = this->get_non_nvml_device(topology, gpu);
        if (gpu_obj == NULL)
        continue;
        }
        
      /* The ancestor was not a numa chip. Is it the machine? */
      ancestor_obj = hwloc_get_ancestor_obj_by_type(topology, HWLOC_OBJ_MACHINE, gpu_obj);
      if (ancestor_obj != NULL)
        {
        PCI_Device new_device;
  
        new_device.initializePCIDevice(gpu_obj, idx, topology);

        store_device_on_appropriate_chip(new_device);
        }
      }
    }
  else
    {
    log_nvml_error(rc, NULL, __func__);
    }

  /* Shutdown the NVML handle. 
   *
   * nvmlShutdown should be called after NVML work is done, once for each call to nvmlInit() 
   * A reference count of the number of initializations is maintained. Shutdown only occurs when 
   * the reference count reaches zero. For backwards compatibility, no error is reported if 
   * nvmlShutdown() is called more times than nvmlInit().
   * */
  rc = nvmlShutdown();
  if (rc != NVML_SUCCESS)
    {
    log_nvml_error(rc, NULL, __func__);
    }

  return(PBSE_NONE);
  }

#endif /* NVIDIA_GPUS */


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
  
#ifdef NVIDIA_GPUS
    initializeNVIDIADevices(obj, topology);
#endif
  return(PBSE_NONE);
  }

hwloc_uint64_t Machine::getTotalMemory()
  {
  return(this->totalMemory);
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
  int available = 0;
 
  for (unsigned int i = 0; i < this->sockets.size(); i++)
    available += this->sockets[i].getAvailableChips();

  return(available);
  }

int Machine::getAvailableCores()
  {
  int available = 0;

  for (unsigned int i = 0; i < this->sockets.size(); i++)
    available += this->sockets[i].getAvailableCores();

  return(available);
  }

int Machine::getAvailableThreads()
  {
  int available = 0;

  for (unsigned int i = 0; i < this->sockets.size(); i++)
    available += this->sockets[i].getAvailableThreads();

  return(available);
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
  this->totalMemory = mem;
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
    }
  }

// This is meant to only be used for unit tests
void Machine::setIsNuma(

  bool is_numa)

  {
  this->isNUMA = is_numa;
  }



void Machine::place_remaining(

  vector<req>  to_split,
  allocation  &a)

  {
  vector<int>   very_spread;

  // This will take care of any that still fit within a socket
  for (unsigned int i = 0; i < to_split.size(); i++)
    {
    const req &r = to_split[i];
    bool       not_placed = true;

    for (unsigned int j = 0; j < this->sockets.size(); j++)
      {
      allocation remaining(r);
      if (this->sockets[j].fits_on_socket(remaining))
        {
        if (this->sockets[j].is_available() == true)
          this->availableSockets--;

        this->sockets[j].partially_place(remaining, a);
        not_placed = false;
        break;
        }
      }

    if (not_placed == true)
      very_spread.push_back(i);
    }

  // Finally, place any that don't fit on a socket
  for (unsigned int i = 0; i < very_spread.size(); i++)
    {
    const req  &r = to_split[very_spread[i]];
    allocation  remaining(r);

    for (unsigned int j = 0; j < this->sockets.size(); j++)
      {
      if (this->sockets[j].is_available() == true)
        this->availableSockets--;

      if (this->sockets[j].partially_place(remaining, a) == true)
        break;
      }
    }
  } // END place_remaining()



int Machine::place_job(

  job    *pjob,
  string &cpu_string,
  string &mem_string)

  {
  if (pjob->ji_wattr[JOB_ATR_req_information].at_val.at_ptr == NULL)
    {
    // Initialize a complete_req from the -l resource request
    pjob->ji_wattr[JOB_ATR_req_information].at_val.at_ptr = new complete_req(pjob->ji_wattr[JOB_ATR_resource].at_val.at_list);
    }

  complete_req *cr = (complete_req *)pjob->ji_wattr[JOB_ATR_req_information].at_val.at_ptr;
  int           num_reqs = cr->req_count();
  vector<int>   partially_place;
  allocation    a(pjob->ji_qs.ji_jobid);
  vector<req>   to_split;

  // See if the tasks fit completely on a socket, and if they do then place them there
  for (int i = 0; i < num_reqs; i++)
    {
    const req &r = cr->get_req(i);
    int        tasks_for_node = r.get_num_tasks_for_host(mom_alias);
    bool       placed = false;

    if (tasks_for_node == 0)
      continue;

    a.set_place_type(r.getPlacementType());

    for (unsigned int j = 0; j < this->sockets.size(); j++)
      {
      if (this->sockets[j].how_many_tasks_fit(r, a.place_type) >= tasks_for_node)
        {
        // place the job entirely on this socket
        placed = true;
        if (this->sockets[j].is_available() == true)
          this->availableSockets--;
        this->sockets[j].place_task(pjob->ji_qs.ji_jobid, r, a, tasks_for_node);
        break;
        }
      }

    if (placed == false)
      {
      // mark this as a task that must be partially placed
      partially_place.push_back(i);
      }
    }

  // If any reqs were marked to be placed partially in different sockets then 
  // place them now
  for (unsigned int i = 0; i < partially_place.size(); i++)
    {
    const req &r = cr->get_req(partially_place[i]);
    int        remaining_tasks = r.get_num_tasks_for_host(mom_alias);
    bool       change = false;
    bool       not_placed = true;
    
    for (unsigned int j = 0; j < this->sockets.size() && remaining_tasks > 0; j++)
      {
      if (this->sockets[j].is_available() == true)
        change = true;
      int placed = this->sockets[j].place_task(pjob->ji_qs.ji_jobid, r, a, remaining_tasks);
      if (placed != 0)
        {
        remaining_tasks -= placed;
        if (change == true)
          this->availableSockets--;
        }
      }

    if (remaining_tasks > 0)
      {
      for (int i = 0; i < remaining_tasks; i++)
        to_split.push_back(r);
      }
    }

  // At this point, all of the reqs that fit within 1 numa node have been placed.
  // Now place any leftover tasks
  place_remaining(to_split, a);

  a.place_indices_in_string(mem_string, MEM_INDICES);
  a.place_indices_in_string(cpu_string, CPU_INDICES);

  this->allocations.push_back(a);
  
  return(PBSE_NONE);
  } // END place_job()



/*
 * get_jobs_cpusets()
 *
 * Gets the cpusets corresponding to this job_id so that the child can place them
 * @param job_id (I) - the id of the job
 * @param cpus (O) - put the string representing the cpus here
 * @param mems (O) - put the string representing the memory nodes here
 * @return PBSE_NONE if we found the job, -1 otherwise
 */

int Machine::get_jobs_cpusets(

  const char *job_id,
  string     &cpus,
  string     &mems)

  {
  int rc = -1;

  for (unsigned int i = 0; i < this->allocations.size(); i++)
    {
    if (!strcmp(this->allocations[i].jobid, job_id))
      {
      this->allocations[i].place_indices_in_string(mems, MEM_INDICES);
      this->allocations[i].place_indices_in_string(cpus, CPU_INDICES);
      rc = PBSE_NONE;
      }
    }

  return(rc);
  } // END get_jobs_cpusets()



void Machine::free_job_allocation(

  const char *job_id)

  {
  for (unsigned int i = 0; i < this->sockets.size(); i++)
    {
    if ((this->sockets[i].is_available() == false) &&
        (this->sockets[i].free_task(job_id) == true))
      this->availableSockets++;
    }

  // Remove from my allocations
  int index = -1;

  for (unsigned int i = 0; i < this->allocations.size(); i++)
    {
    if (!strcmp(this->allocations[i].jobid, job_id))
      {
      index = i;
      break;
      }
    }

  if (index != -1)
    this->allocations.erase(this->allocations.begin() + index);
  } // END free_job_allocation()



void Machine::store_device_on_appropriate_chip(
    
  PCI_Device &device)

  {
  if (this->isNUMA == false)
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

#endif /* PENABLE_LINUX_CGROUPS */
