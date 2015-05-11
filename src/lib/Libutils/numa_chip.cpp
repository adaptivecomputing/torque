#include <iostream>
#include <string>
#include <vector>
#include <errno.h>
#include "pbs_config.h"

#ifdef PENABLE_LINUX_CGROUPS
#include "machine.hpp"
#include <hwloc.h>

#ifdef MIC
#include <hwloc/intel-mic.h>
#endif

#ifdef NVIDIA_GPUS
#ifdef NVML_API
#include <hwloc/nvml.h>
#endif
#endif

#include "pbs_error.h"
#include "log.h"
#include "utils.h"

using namespace std;

#define INTEL 1
#define AMD   2


Chip::Chip() : totalThreads(0), totalCores(0), id(0), availableThreads(0), availableCores(0),
               total_gpus(0), available_gpus(0), total_mics(0), available_mics(0),
               chip_exclusive(false), available_memory(0)
  {
  memset(chip_cpuset_string, 0, MAX_CPUSET_SIZE);
  memset(chip_nodeset_string, 0, MAX_NODESET_SIZE);
  }



void capture_until_close_quote(

  char        **start,
  std::string  &storage)

  {
  if ((start == NULL) ||
      (*start == NULL))
    return;

  char *val = *start;
  char *ptr = strchr(val, '"');

  // Make sure we found a close quote and this wasn't an empty string
  if ((ptr != NULL) &&
      (ptr != val))
    {
    storage = val;
    storage.erase(ptr - val);
    *start = ptr + 1; // add 1 to move past the close quote
    }
  }



void Chip::parse_values_from_json_string(

  const std::string &json_layout,
  std::string       &cores,
  std::string       &threads,
  std::string       &gpus,
  std::string       &mics)

  {
  char        *work_str = strdup(json_layout.c_str());
  char        *ptr = strstr(work_str, "os_index\":");
  char        *val = work_str;
  char        *close_quote;

  if (ptr != NULL)
    {
    val = ptr + strlen("os_index\":");
    this->id = strtol(val, &val, 10);
    }

  if ((ptr = strstr(val, "cores\":")) != NULL)
    {
    val = ptr + strlen("cores\":") + 1; // add 1 for the open quote
    capture_until_close_quote(&val, cores);
    }

  if ((ptr = strstr(val, "threads\":")) != NULL)
    {
    val = ptr + strlen("threads\":") + 1; // add 1 for the open quote
    capture_until_close_quote(&val, threads);
    }

  if ((ptr = strstr(val, "mem\":")) != NULL)
    {
    val = ptr + strlen("mem\":");
    this->memory = strtol(val, &val, 10);
    this->available_memory = this->memory;
    }

  if ((ptr = strstr(val, "gpus\":")) != NULL)
    {
    val = ptr + strlen("gpus\":") + 1;
    capture_until_close_quote(&val, gpus);
    }

  if ((ptr = strstr(val, "mics\":")) != NULL)
    {
    val = ptr + strlen("mics\":") + 1;
    capture_until_close_quote(&val, mics);
    }

  free(work_str);
  } // END parse_values_from_json_string()



void Chip::initialize_cores_from_strings(

  std::string &cores_str,
  std::string &threads_str)

  {
  std::vector<int> core_indices;
  std::vector<int> thread_indices;
  int              ratio;

  translate_range_string_to_vector(cores_str.c_str(), core_indices);
  translate_range_string_to_vector(threads_str.c_str(), thread_indices);

  ratio = thread_indices.size() / core_indices.size();
  unsigned int j = 0;

  for (unsigned int i = 0; i < core_indices.size(); i++)
    {
    Core c;

    c.add_processing_unit(CORE, core_indices[i]);
    this->totalThreads++;

    for (int t = 0; t < ratio; t++)
      {
      c.add_processing_unit(THREAD, thread_indices[j++]);
      this->totalThreads++;
      }

    this->cores.push_back(c);
    }
  
  this->totalCores = this->cores.size();
  this->availableCores = this->totalCores;
  this->availableThreads = this->totalThreads;
  } // END initialize_cores_from_strings()



void Chip::initialize_accelerators_from_strings(

  std::string &gpus,
  std::string &mics)

  {
  std::vector<int> gpu_indices;
  std::vector<int> mic_indices;

  translate_range_string_to_vector(gpus.c_str(), gpu_indices);
  translate_range_string_to_vector(mics.c_str(), mic_indices);

  for (unsigned int i = 0; i < gpu_indices.size(); i++)
    {
    PCI_Device p;
    p.set_type(GPU);
    p.setId(gpu_indices[i]);
    this->total_gpus++;
    this->devices.push_back(p);
    }

  for (unsigned int i = 0; i < mic_indices.size(); i++)
    {
    PCI_Device p;
    p.set_type(MIC_TYPE);
    p.setId(mic_indices[i]);
    this->total_mics++;
    this->devices.push_back(p);
    }

  this->available_gpus = this->total_gpus;
  this->available_mics = this->total_mics;
  } // END initialize_accelerators_from_strings()



/*
 * Creates a numa chip from this json 
 *
 * "numachip" : {
 *   "os_index" : <index>,
 *   "cores" : <core range string>,
 *   "threads" : <thread range string>,
 *   "mem" : <memory in kb>,
 *   "gpus" : <gpu range string>,
 *   "mics" : <mic range string>
 *   }
 *
 * mics and gpus are optional and only present if they actually exist on the node
 */

Chip::Chip(
   
  const std::string &json_layout) : totalThreads(0), totalCores(0), id(0), availableThreads(0),
                                    availableCores(0), total_gpus(0), available_gpus(0),
                                    total_mics(0), available_mics(0), chip_exclusive(false),
                                    available_memory(0)

  {
  memset(chip_cpuset_string, 0, MAX_CPUSET_SIZE);
  memset(chip_nodeset_string, 0, MAX_NODESET_SIZE);

  if (json_layout.size() == 0)
    return;

  std::string cores;
  std::string threads;
  std::string gpus;
  std::string mics;

  parse_values_from_json_string(json_layout, cores, threads, gpus, mics);

  initialize_cores_from_strings(cores, threads);
  
  initialize_accelerators_from_strings(gpus, mics);
  } // End JSON constructor



Chip::~Chip()
  {
  id = -1;
  }

/* initializeNonNUMAChip will initialize an instance of a NUMA Chip and then populate
   it with all of the cores and threads in the system as if there were a NUMA Chip */
int Chip::initializeNonNUMAChip(hwloc_obj_t socket_obj, hwloc_topology_t topology)
  {
  hwloc_obj_t core_obj;
  hwloc_obj_t prev = NULL;
  int rc;

  rc = get_machine_total_memory(topology, &this->memory);
  if (rc != 0)
    {
    log_err(-1, __func__, "could not get memory for NonNUMAChip");
    /* Not fatal for now. Go on */
    }
  else
    {
    // Store the memory in kb
    this->memory = this->memory / 1024;
    }

  this->available_memory = this->memory;

  while ((core_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_CORE, prev)) != NULL)
    {
    Core newCore;

    newCore.initializeCore(core_obj, topology);
    this->cores.push_back(newCore);
    this->totalThreads += newCore.getNumberOfProcessingUnits();
    prev = core_obj;
    }

  this->totalCores = this->cores.size();
  this->availableCores = this->totalCores;
  this->availableThreads = this->totalThreads;
  this->chip_cpuset = hwloc_topology_get_allowed_cpuset(topology);
  this->chip_nodeset = hwloc_topology_get_allowed_nodeset(topology);
  hwloc_bitmap_list_snprintf(chip_cpuset_string, MAX_CPUSET_SIZE, this->chip_cpuset);
  hwloc_bitmap_list_snprintf(chip_nodeset_string, MAX_CPUSET_SIZE, this->chip_nodeset);

  if (this->totalCores == this->totalThreads)
    {
    this->isThreaded = false;
    }
  else
    {
    this->isThreaded = true;
    }
 
  this->initializePCIDevices(NULL, topology);
  
  return(PBSE_NONE);
  }

int Chip::initializeChip(hwloc_obj_t chip_obj, hwloc_topology_t topology)
  {
  hwloc_obj_t core_obj;
  hwloc_obj_t prev;

  this->id = chip_obj->logical_index;
  // Store the memory in kb
  this->memory = chip_obj->memory.local_memory / 1024;
  this->available_memory = this->memory;
  this->chip_cpuset = chip_obj->allowed_cpuset;
  this->chip_nodeset = chip_obj->allowed_nodeset;
  hwloc_bitmap_list_snprintf(this->chip_cpuset_string, MAX_CPUSET_SIZE, this->chip_cpuset);
  hwloc_bitmap_list_snprintf(this->chip_nodeset_string, MAX_CPUSET_SIZE, this->chip_nodeset);
  this->totalCores = hwloc_get_nbobjs_inside_cpuset_by_type(topology, this->chip_cpuset, HWLOC_OBJ_CORE);
  this->totalThreads = hwloc_get_nbobjs_inside_cpuset_by_type(topology, this->chip_cpuset, HWLOC_OBJ_PU);
  this->availableCores = this->totalCores;
  this->availableThreads = this->totalThreads;

  if (this->totalCores == this->totalThreads)
    {
    this->isThreaded = false;
    }
  else
    {
    this->isThreaded = true;
    }

  /* Find all the cores that belong to this numa chip */
  prev = NULL;
  while ((core_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_CORE, prev)) != NULL)
    {
    int is_in_subtree;

    is_in_subtree = hwloc_obj_is_in_subtree(topology, core_obj, chip_obj);
    if (is_in_subtree)
      {
      Core new_core;

      new_core.initializeCore(core_obj, topology);
      this->cores.push_back(new_core);
      }
    prev = core_obj;
    }

  this->initializePCIDevices(chip_obj, topology);

  return(PBSE_NONE);
  }

int Chip::getTotalCores() const
  {
  return(this->totalCores);
  }

int Chip::getTotalThreads() const
  {
  return(this->totalThreads);
  }

int Chip::getAvailableCores() const
  {
  if (this->chip_exclusive == true)
    return(0);
  else
    return(this->availableCores);
  }

int Chip::getAvailableThreads() const
  {
  if (this->chip_exclusive == true)
    return(0);
  else
    return(this->availableThreads);
  }

hwloc_uint64_t Chip::getAvailableMemory() const
  {
  return(this->available_memory);
  }

int Chip::get_available_mics() const
  {
  return(this->available_mics);
  }

int Chip::get_available_gpus() const
  {
  return(this->available_gpus);
  }

int Chip::get_id() const
  {
  return(this->id);
  }

hwloc_uint64_t Chip::getMemory() const
  {
  return(this->memory);
  }

bool Chip::chipIsAvailable() const
  {
  if ((this->availableThreads == this->totalThreads) &&
      (this->availableCores == this->totalCores) &&
      (this->available_memory == this->memory))
    return(true);

  return(false);
  }

#ifdef MIC
int Chip::initializeMICDevices(hwloc_obj_t chip_obj, hwloc_topology_t topology)
  {
  /* Get any Intel MIC devices */
  /* start at indes 0 and go until hwloc_intel_mic_get_device_osdev_by_index
     returns NULL */
  for (int idx = 0; ; idx++)
    {
    hwloc_obj_t mic_obj;
    hwloc_obj_t ancestor_obj;
    int is_in_tree;

    mic_obj = hwloc_intel_mic_get_device_osdev_by_index(topology, idx);
    if (mic_obj == NULL)
      break;

    if (chip_obj == NULL) 
      { /* this came from the Non NUMA */
      PCI_Device new_device;

      new_device.initializePCIDevice(mic_obj, idx, topology);
      this->devices.push_back(new_device);
      }
    else
      {
      PCI_Device new_device;
      ancestor_obj = hwloc_get_ancestor_obj_by_type(topology, HWLOC_OBJ_NODE, mic_obj);
      if (ancestor_obj != NULL)
        {
        if (ancestor_obj->logical_index == chip_obj->logical_index)
          {
          PCI_Device new_device;

          new_device.initializePCIDevice(mic_obj, idx, topology);
          this->devices.push_back(new_device);
          this->total_mics++;
          this->available_mics++;
          }
        }
      }
    }
  return(PBSE_NONE);
  }
#endif


int Chip::initializePCIDevices(hwloc_obj_t chip_obj, hwloc_topology_t topology)
  {
  /* See if there are any accelerators */

#ifdef MIC
  this->initializeMICDevices(chip_obj, topology);
#endif

  return(PBSE_NONE);
  }


void Chip::displayAsJson(

  stringstream &out) const

  {
  std::vector<int> core_indices;
  std::vector<int> thread_indices;
  std::vector<int> mic_indices;
  std::vector<int> gpu_indices;
  std::string      core_range;
  std::string      thread_range;
  std::string      mic_range;
  std::string      gpu_range;

  for (unsigned int i = 0; i < this->cores.size(); i++)
    {
    for (unsigned int j = 0; j < this->cores[i].indices.size(); j++)
      {
      if (this->cores[i].indices[j] != this->cores[i].id)
        thread_indices.push_back(this->cores[i].indices[j]);
      else
        core_indices.push_back(this->cores[i].indices[j]);
      }
    }

  translate_vector_to_range_string(core_range, core_indices);
  translate_vector_to_range_string(thread_range, thread_indices);

  // Get a list of my accelerators, if any
  for (unsigned int i = 0; i < this->devices.size(); i++)
    {
    if (this->devices[i].get_type() == MIC_TYPE)
      mic_indices.push_back(this->devices[i].get_id());
    else
      gpu_indices.push_back(this->devices[i].get_id());
    }
  
  translate_vector_to_range_string(gpu_range, gpu_indices);
  translate_vector_to_range_string(mic_range, mic_indices);

  // Format the output as json
  out << "\"numachip\":{\"os_index\":" << this->id << ",\"cores\":\"" << core_range;
  out << "\",\"threads\":\"" << thread_range << "\",\"mem\":" << this->memory;

  if (gpu_range.size() != 0)
    out << ",\"gpus\":\"" << gpu_range << "\"";

  if (mic_range.size() != 0)
    out << ",\"mics\":\"" << mic_range << "\"";

  // close the json
  out << "}";
  } // END displayAsJson()



void Chip::displayAsString(

  stringstream &out) const

  {
  out << "    Chip " << this->id << " (" << this->memory << "KB)\n";

  for (unsigned int i = 0; i < this->cores.size(); i++)
    this->cores[i].displayAsString(out);
  
  for (unsigned int i = 0; i < this->devices.size(); i++)
    this->devices[i].displayAsString(out);
  } // END displayAsString() 



void Chip::setMemory(

  hwloc_uint64_t memory)

  {
  this->memory = memory;
  this->available_memory = memory;
  }

void Chip::setCores(

  int cores)

  {
  this->totalCores = cores;
  this->availableCores = cores;
  }

void Chip::setThreads(

  int threads)

  {
  this->totalThreads = threads;
  this->availableThreads = threads;
  }

void Chip::setChipAvailable(

  bool available)

  {
  this->chip_exclusive = !available;
  }

void Chip::setId(

  int id)

  {
  this->id = id;
  }

// This function is for unit testing
void Chip::make_core(
    
  int id)

  {
  Core c;
  c.id = id;
  c.totalThreads = 2;
  c.free = true;
  c.indices.push_back(id);
  c.indices.push_back(id + 12);
  c.is_index_busy.push_back(false);
  c.is_index_busy.push_back(false);
  c.processing_units_open = 2;
  this->cores.push_back(c);
  }



// This is used only for unit tests
void Chip::set_cpuset(
    
  const char *cpuset_string)

  {
  snprintf(this->chip_cpuset_string, sizeof(this->chip_cpuset_string), "%s", cpuset_string);
  }



/*
 * how_many_tasks_fit()
 *
 * Determines how many tasks from req r fit on this chip
 * @param r - the req we're examining
 * @return the number of tasks that fit. This can be 0
 */

int Chip::how_many_tasks_fit(

  const req &r,
  int        place_type) const

  {
  int cpu_tasks;
  int gpu_tasks;
  int mic_tasks;
  int mem_tasks = 0;

  // Consider exclusive socket and node the same as exclusive chip for our purposes
  if ((place_type == exclusive_socket) ||
      (place_type == exclusive_node))
    place_type = exclusive_chip;

  if ((this->chip_exclusive == false) &&
      ((place_type != exclusive_chip) ||
       (this->chipIsAvailable()) == true))
    {
    if (r.getThreadUsageString() == use_cores)
      cpu_tasks = this->availableCores / r.getExecutionSlots();
    else
      cpu_tasks = this->availableThreads / r.getExecutionSlots();

    long long memory = r.getMemory();

    // Memory isn't required for submission
    if (memory != 0)
      {
      mem_tasks = this->available_memory / memory;

      // return the lower of the two values
      if (mem_tasks > cpu_tasks)
        mem_tasks = cpu_tasks;
      }
    else
      mem_tasks = cpu_tasks;

    int gpus = r.getGpus();
    if (gpus > 0)
      {
      gpu_tasks = this->available_gpus / gpus;
      if (mem_tasks > gpu_tasks)
        mem_tasks = gpu_tasks;
      }

    int mics = r.getMics();
    if (mics > 0)
      {
      mic_tasks = this->available_mics / mics;
      if (mem_tasks > mics)
        mem_tasks = mic_tasks;
      }
    
    if ((place_type == exclusive_chip) &&
        (mem_tasks > 1))
      mem_tasks = 1;
    }
    
  return(mem_tasks);
  } // END how_many_tasks_fit()



/*
 * place_task_by_cores()
 *
 * places the task, knowing that we must use only cores
 *
 * @param execution_slots_per_task - the number of cores to place for this task
 * @param a - the allocation we're marking these used for
 */

void Chip::place_task_by_cores(

  int         execution_slots_per_task,
  allocation &a)

  {
  // Get the core indices we will use
  unsigned int j = 0;
  for (int i = 0; i < execution_slots_per_task; i++)
    {
    while (j < this->cores.size())
      {
      if (this->cores[j].free == true)
        {
        this->cores[j].mark_as_busy(this->cores[j].id);
        a.cpu_indices.push_back(this->cores[j].id);
        a.cores++;
        this->availableCores--;
        this->availableThreads -= this->cores[j].totalThreads;
        a.threads += this->cores[j].totalThreads;
        j++;

        break;
        }
      else
        j++;
      }
    }

  a.cpus += a.cores;
  } // END place_task_by_cores()



/*
 * place_task_by_threads()
 *
 * places the task, knowing that we can use threads
 *
 * @param execution_slots_per_task - the number of cores to place for this task
 * @param a - the allocation we're marking these used for
 */

void Chip::place_task_by_threads(

  int         execution_slots_per_task,
  allocation &a)

  {
  // Place for being able to use threads
  int slots_left = execution_slots_per_task;
  // Get the core indices we will use
  for (unsigned int j = 0; j < this->cores.size() && slots_left > 0; j++)
    {
    int index;

    if (this->cores[j].free == true)
      {
      this->availableCores--;
      a.cores++;
      }

    while ((slots_left > 0) && 
           ((index = this->cores[j].get_open_processing_unit()) != -1))
      {
      this->availableThreads--;
      slots_left--;
      a.threads++;
      a.cpu_indices.push_back(index);
      }
    }

  a.cpus += a.threads;
  } // END place_task_by_threads()



/*
 * task_will_fit()
 *
 * Checks if a task identified by mem_per_task, execution_slots_per_task, 
 * and cores_only will completely fit on this core
 *
 * @param mem_per_task - the amount of memory this task requires
 * @param execution_slots_per_tsak - the number of execution slots this task requires
 * @param cores_only - true if we should use only cores for this task
 * @return true if the task can completely fit on this chip's available space
 */

bool Chip::task_will_fit(

  const req &r) const

  {
  bool           fits = false;
  int            execution_slots_per_task = r.getExecutionSlots();
  hwloc_uint64_t mem_per_task = r.getMemory();
  int            gpus_per_task = r.getGpus();
  int            mics_per_task = r.getMics();
  bool           cores_only = (r.getThreadUsageString() == use_cores);

  if ((this->available_memory >= mem_per_task) &&
      (this->available_gpus >= gpus_per_task) &&
      (this->available_mics >= mics_per_task))
    {
    if (cores_only == true)
      {
      if (this->availableCores >= execution_slots_per_task)
        fits = true;
      }
    else
      {
      if (this->availableThreads >= execution_slots_per_task)
        fits = true;
      }
    }

  return(fits);
  } // END task_will_fit()



/*
 * place_task()
 *
 * Places as many tasks up to to_place from req r on this chip as are available.
 * @param jobid - the jobid whose task's we're placing
 * @param r - the req from which we're placing tasks
 * @param master - the master allocation for the socket this chip belongs to
 * @param to_place the maximum number of tasks to place
 * @return the number of tasks placed
 */

int Chip::place_task(

  const char *jobid,
  const req  &r,
  allocation &master,
  int         to_place)

  {
  allocation     a(jobid);
  int            tasks_placed = 0;
  int            execution_slots_per_task = r.getExecutionSlots();
  hwloc_uint64_t mem_per_task = r.getMemory();
  int            practical_place = master.place_type;

  // Practically, we should treat place=node, place=socket, and
  // place=numachip as the same
  if ((practical_place == exclusive_socket) ||
      (practical_place == exclusive_node))
    practical_place = exclusive_chip;

  if ((practical_place != exclusive_chip) ||
      (this->chipIsAvailable() == true))
    {
    if (this->chip_exclusive == false)
      {
      if (r.getThreadUsageString() == use_cores)
        a.cores_only = true;
      else
        a.cores_only = false;

      for (; tasks_placed < to_place; tasks_placed++)
        {
        if (task_will_fit(r) == false)
          break;

        this->available_memory -= mem_per_task;
        a.memory += mem_per_task;
        a.cpus += execution_slots_per_task;
        
        if (a.cores_only == true)
          place_task_by_cores(execution_slots_per_task, a);
        else
          place_task_by_threads(execution_slots_per_task, a);

        allocation remaining(r);

        place_accelerators(remaining, a);

        if (practical_place == exclusive_chip)
          {
          tasks_placed++;
          break;
          }
        }

      if ((practical_place == exclusive_chip) &&
          (tasks_placed > 0))
        this->chip_exclusive = true;
      } // if chip_exclusive == false

    if (tasks_placed > 0)
      {
      // Add this as a memory node
      a.mem_indices.push_back(this->id);
      this->allocations.push_back(a);
      master.add_allocation(a);
      }
    }

  return(tasks_placed);
  } // END place_task()



int Chip::reserve_accelerator(

  int type)

  {
  int index = -1;

  for (unsigned int i = 0; i < this->devices.size(); i++)
    {
    if ((type == this->devices[i].get_type()) &&
        (this->devices[i].is_busy() == false))
      {
      this->devices[i].set_state(true);
      if (type == MIC_TYPE)
        this->available_mics--;
      else
        this->available_gpus--;

      index = this->devices[i].get_id();
      break;
      }
    }

  return(index);
  } // END reserve_accelerator()



/*
 * place_accelerators()
 *
 */

void Chip::place_accelerators(

  allocation &remaining,
  allocation &a)

  {
  int i;

  for (i = 0; i < remaining.gpus; i++)
    {
    int index = this->reserve_accelerator(GPU);

    if (index < 0)
      break;

    a.gpu_indices.push_back(index);
    }

  remaining.gpus -= i;

  for (i = 0; i < remaining.mics; i++)
    {
    int index = this->reserve_accelerator(MIC_TYPE);

    if (index < 0)
      break;

    a.mic_indices.push_back(index);
    }

  remaining.mics -= i;
  } // END place_accelerators()



void Chip::free_accelerator(

  int index,
  int type)

  {
  for (unsigned int i = 0; i < this->devices.size(); i++)
    {
    if ((this->devices[i].get_type() == type) &&
        (this->devices[i].get_id() == index))
      {
      this->devices[i].set_state(false);
      if (type == MIC_TYPE)
        this->available_mics++;
      else
        this->available_gpus++;
      }
    }
  } // END free_accelerator()



void Chip::free_accelerators(

  allocation &a)

  {
  for (unsigned int i = 0; i < a.gpu_indices.size(); i++)
    this->free_accelerator(a.gpu_indices[i], GPU);

  for (unsigned int i = 0; i < a.mic_indices.size(); i++)
    this->free_accelerator(a.mic_indices[i], MIC_TYPE);

  } // END free_accelerators()



/*
 * partially_place_task()
 *
 * Places whatever can be placed from the task specified by remaining onto this chip
 * @param remaining (I/O) - specifies how much of the task remains to be placed. Updated.
 * @param master (O) - the allocation for the entire job
 */

void Chip::partially_place_task(

  allocation &remaining,
  allocation &master)

  {
  allocation     a(master.jobid);

  // handle memory
  if (remaining.memory > this->available_memory)
    {
    a.memory = this->available_memory;
    remaining.memory -= this->available_memory;
    this->available_memory = 0;
    }
  else
    {
    this->available_memory -= remaining.memory;
    a.memory = remaining.memory;
    remaining.memory = 0;
    }

  if (remaining.cores_only == true)
    place_task_by_cores(remaining.cpus, a);
  else
    place_task_by_threads(remaining.cpus, a);

  place_accelerators(remaining, a);
  
  if ((a.cpus > 0) ||
      (a.memory > 0) ||
      (a.gpu_indices.size() > 0) ||
      (a.mic_indices.size() > 0))
    {
    a.mem_indices.push_back(this->id);

    this->allocations.push_back(a);
    master.add_allocation(a);
    }

  remaining.cpus -= a.cpus;
  master.add_allocation(a);
  
  // Practically, we should treat place=node, place=socket, and
  // place=numachip as the same
  if ((master.place_type == exclusive_socket) ||
      (master.place_type == exclusive_node))
    this->chip_exclusive = true;
  } // END partially_place_task()



/*
 * free_cpu_index()
 *
 * Marks processing unit at index as available
 * @param index - the os index of the processing unit to mark as unused
 */

void Chip::free_cpu_index(

  int index)

  {
  bool core_is_now_free = false;
  for (unsigned int i = 0; i < this->cores.size(); i++)
    {
    if (this->cores[i].free == true)
      continue;

    if (this->cores[i].free_pu_index(index, core_is_now_free) == true)
      {
      if (core_is_now_free == true)
        {
        this->availableCores++;
        }

      return;
      }
    }

  } // END free_cpu_index()



/*
 * free_task()
 *
 * Frees all of the cores that are in use for the job that matches jobid
 * @param jobid - the id of the job in question
 * @return true if this chip is completely free
 */

bool Chip::free_task(

  const char *jobid)

  {
  int  to_remove = -1;
  bool totally_free = false;

  for (unsigned int i = 0; i < this->allocations.size(); i++)
    {
    if (!strcmp(this->allocations[i].jobid, jobid))
      {
      to_remove = i;
      this->availableThreads += this->allocations[i].threads;
      this->available_memory += this->allocations[i].memory;

      // Now mark the individual cores as available
      for (unsigned int j = 0; j < this->allocations[i].cpu_indices.size(); j++)
        free_cpu_index(this->allocations[i].cpu_indices[j]);

      free_accelerators(this->allocations[i]);
      
      break;
      }
    }

  if (to_remove != -1)
    this->allocations.erase(this->allocations.begin() + to_remove);

  if ((this->availableThreads == this->totalThreads) &&
      (this->availableCores == this->totalCores))
    {
    this->chip_exclusive = false;
    totally_free = true;
    }

  return(totally_free);
  } // END free_task()



bool Chip::store_pci_device_appropriately(

  PCI_Device &device,
  bool        force)

  {
  bool stored = false;

  if (force == true)
    {
    this->devices.push_back(device);
    stored = true;
    }
  else
    {
    std::string device_cpuset(device.get_cpuset());

    if (cpusets_overlap(device_cpuset) == true)
      {
      this->devices.push_back(device);
      stored = true;
      }
    }

  if (stored)
    {
    // Increase our count of these devices
    if (device.get_type() == MIC_TYPE)
      {
      this->total_mics++;
      this->available_mics++;
      }
    else
      {
      this->total_gpus++;
      this->available_gpus++;
      }
    }

  return(stored);
  }



bool Chip::cpusets_overlap(

  const std::string &other) const

  {
  std::vector<int> one;
  std::vector<int> two;
  bool             overlap = false;

  translate_range_string_to_vector(this->chip_cpuset_string, one);
  translate_range_string_to_vector(other.c_str(), two);

  for (unsigned int i = 0; i < one.size() && overlap == false; i++)
    {
    for (unsigned int j = 0; j < two.size(); j++)
      {
      if (one[i] == two[j])
        {
        overlap = true;
        break;
        }
      }
    }

  return(overlap);
  }

#endif /* PENABLE_LINUX_CGROUPS */  
