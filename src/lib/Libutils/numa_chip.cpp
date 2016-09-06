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


Chip::Chip() : id(0), totalCores(0), totalThreads(0), availableCores(0), availableThreads(0),
               total_gpus(0), available_gpus(0), total_mics(0), available_mics(0),
               chip_exclusive(false), memory(0), available_memory(0), cores(), devices(),
               allocations()

  {
  memset(chip_cpuset_string, 0, sizeof(chip_cpuset_string));
  memset(chip_nodeset_string, 0, sizeof(chip_nodeset_string));
  }
    
Chip::Chip(
    
  const Chip &other) : id(other.id), totalCores(other.totalCores), totalThreads(other.totalThreads),
                       availableCores(other.availableCores), availableThreads(other.availableThreads),
                       total_gpus(other.total_gpus), available_gpus(other.available_gpus),
                       total_mics(other.total_mics), available_mics(other.available_mics),
                       chip_exclusive(other.chip_exclusive), memory(other.memory),
                       available_memory(other.available_memory), cores(other.cores),
                       devices(other.devices), allocations(other.allocations)

  {
  // Don't copy the hwloc_const_* types as they aren't needed

  memcpy(chip_cpuset_string, other.chip_cpuset_string, sizeof(chip_cpuset_string));
  memcpy(chip_nodeset_string, other.chip_nodeset_string, sizeof(chip_nodeset_string));
  }

Chip &Chip::operator =(
    
  const Chip &other)

  {
  this->id = other.id;
  this->totalCores = other.totalCores;
  this->totalThreads = other.totalThreads;
  this->availableCores = other.availableCores;
  this->availableThreads = other.availableThreads;
  this->total_gpus = other.total_gpus;
  this->available_gpus = other.available_gpus;
  this->total_mics = other.total_mics;
  this->available_mics = other.available_mics;
  this->chip_exclusive = other.chip_exclusive;
  this->memory = other.memory;
  this->available_memory = other.available_memory;
  this->cores = other.cores;
  this->devices = other.devices;
  this->allocations = other.allocations;
  
  memcpy(chip_cpuset_string, other.chip_cpuset_string, sizeof(chip_cpuset_string));
  memcpy(chip_nodeset_string, other.chip_nodeset_string, sizeof(chip_nodeset_string));

  return(*this);
  }



Chip::Chip(

  int  execution_slots,
  int &es_remainder,
  int &per_numa_remainder) : id(0),total_gpus(0), available_gpus(0), total_mics(0), available_mics(0), 
                             chip_exclusive(false), memory(0), available_memory(0), cores(), devices(),
                             allocations()

  {
  if (es_remainder > 0)
    {
    execution_slots++;
    es_remainder--;
    }

  if (per_numa_remainder > 0)
    {
    execution_slots++;
    per_numa_remainder--;
    }

  this->totalCores = execution_slots;
  this->totalThreads = execution_slots;
  this->availableCores = this->totalCores;
  this->availableThreads = this->totalThreads;

  for (int i = 0; i < execution_slots; i++)
    {
    Core c;
    c.add_processing_unit(CORE, i);

    this->cores.push_back(c);
    }
  } // END constructor for Cray



/*
 * parse_values_from_json_string()
 */

void Chip::parse_values_from_json_string(

  const std::string        &json_layout,
  std::string              &cores,
  std::string              &threads,
  std::string              &gpus,
  std::string              &mics,
  std::vector<std::string> &valid_ids)

  {
  char        *work_str = strdup(json_layout.c_str());
  char        *ptr = strstr(work_str, "os_index\":");
  char        *val = work_str;

  if (ptr != NULL)
    {
    val = ptr + strlen("os_index\":");
    this->id = strtol(val, &val, 10);
    }

  if ((ptr = strstr(val, "cores\":")) != NULL)
    {
    val = ptr + strlen("cores\":") + 1; // add 1 for the open quote
    capture_until_close_character(&val, cores, '"');
    }

  if ((ptr = strstr(val, "threads\":")) != NULL)
    {
    val = ptr + strlen("threads\":") + 1; // add 1 for the open quote
    capture_until_close_character(&val, threads, '"');
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
    capture_until_close_character(&val, gpus, '"');
    }

  if ((ptr = strstr(val, "mics\":")) != NULL)
    {
    val = ptr + strlen("mics\":") + 1;
    capture_until_close_character(&val, mics, '"');
    }

  initialize_allocations(val, valid_ids);

  free(work_str);
  } // END parse_values_from_json_string()



/*
 * initialize_cores_from_strings()
 */

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



/*
 * initialize_allocation()
 *
 * Initializes an allocation based on the specification:
 *
 * "allocation" : {
 *   "jobid" : "<jobid>",
 *   "cpus" : "<cpu range string>",
 *   "mem" : <memory in kb>,
 *   "exclusive" : <exclusive type>,
 *   "cores_only" : <1 or 0>,
 *   "gpus" : "<gpu range string>",
 *   "mics" : "<mic range string>"
 *   }
 *
 * gpus and mics are both optional
 */

void Chip::initialize_allocation(

  char                     *allocation_str,
  std::vector<std::string> &valid_ids)

  {
  allocation   a;
  char        *ptr = strstr(allocation_str, "jobid\":");
  char        *val = allocation_str;
  std::string  tmp_val;

  if (ptr != NULL)
    {
    val = ptr + 8; // move past "jobid\":\""
    capture_until_close_character(&val, tmp_val, '"');
    a.jobid = tmp_val;
    }

  // check if the id is valid
  bool id_valid = false;
  for (size_t i = 0; i < valid_ids.size(); i++)
    {
    if (valid_ids[i] == a.jobid)
      {
      id_valid = true;
      break;
      }
    }

  // Only keep this allocation if the id is valid
  if (id_valid == true)
    {
    ptr = strstr(val, "cpus\":");
    if (ptr != NULL)
      {
      val = ptr + 7; // move past "cpus\":\""
      capture_until_close_character(&val, tmp_val, '"');
      translate_range_string_to_vector(tmp_val.c_str(), a.cpu_indices);
      }

    ptr = strstr(val, "mem\":");
    if (ptr != NULL)
      {
      val = ptr + 5; // move past "mem\":"
      a.memory = strtol(val, &val, 10);
      }

    ptr = strstr(val, "exclusive\":");
    if (ptr != NULL)
      {
      val = ptr + 11; // move past "exclusive\":"
      a.place_type = strtol(val, &val, 10);
      }

    ptr = strstr(val, "cores_only\":");
    if (ptr != NULL)
      {
      val = ptr + 12; // move past "cores_only\":"
      a.cores_only = (bool)strtol(val, &val, 10);
      }

    ptr = strstr(val, "gpus\":");
    if (ptr != NULL)
      {
      val = ptr + 7; // move past "gpus\":\"
      capture_until_close_character(&val, tmp_val, '"');
      translate_range_string_to_vector(tmp_val.c_str(), a.gpu_indices);
      }

    ptr = strstr(val, "mics\":");
    if (ptr != NULL)
      {
      val = ptr + 7; // move past "mics\":\"
      capture_until_close_character(&val, tmp_val, '"');
      translate_range_string_to_vector(tmp_val.c_str(), a.mic_indices);
      }

    a.mem_indices.push_back(this->id);

    this->allocations.push_back(a);
    }
  } // END initialize_allocation()



/*
 * initialize_allocations()
 *
 */ 

void Chip::initialize_allocations(

  char                     *allocations,
  std::vector<std::string> &valid_ids)

  {
  static const char *allocation_start = "allocation\":{";
  static const int   allocation_start_len = strlen(allocation_start);

  if ((allocations == NULL) ||
      (*allocations == '\0'))
    return;

  char *current = strstr(allocations, allocation_start);
  char *next;

  while (current != NULL)
    {
    current += allocation_start_len;
    next = strstr(current, allocation_start);
    if (next != NULL)
      {
      // Make sure there's a termination to the current string
      *next = '\0';
      }

    initialize_allocation(current, valid_ids);

    current = next;
    }

  } // END initialize_allocations()



/*
 * reserce_allocation_resources()
 *
 * @param a - the allocation that needs to be 
 */

void Chip::reserve_allocation_resources(

  allocation &a)

  {
  // reserve each cpu
  for (unsigned int j = 0; j < a.cpu_indices.size(); j++)
    {
    for (unsigned int c = 0; c < this->cores.size(); c++)
      {
      if (this->cores[c].reserve_processing_unit(a.cpu_indices[j]) == true)
        {
        if (a.cores_only == true)
          {
          this->availableCores--;
          this->availableThreads -= this->cores[c].totalThreads;
          a.threads += this->cores[c].totalThreads;
          a.cores++;
          a.cpus++;
          }
        else
          {
          this->availableThreads--;
          a.threads++;
          }
        
        break;
        }
      }
    }

  this->available_memory -= a.memory;

  for (unsigned int j = 0; j < a.gpu_indices.size(); j++)
    {
    for (unsigned int d = 0; d < this->devices.size(); d++)
      {
      if ((GPU == this->devices[d].get_type()) &&
          (this->devices[d].get_id() == a.gpu_indices[j]))
        {
        this->devices[d].set_state(true);
        this->available_gpus--;
        break;
        }
      }
    }

  for (unsigned int j = 0; j < a.mic_indices.size(); j++)
    {
    for (unsigned int d = 0; d < this->devices.size(); d++)
      {
      if ((MIC_TYPE == this->devices[d].get_type()) &&
          (this->devices[d].get_id() == a.mic_indices[j]))
        {
        this->devices[d].set_state(true);
        this->available_mics--;
        break;
        }
      }
    }
  
  if ((a.place_type == exclusive_socket) ||
      (a.place_type == exclusive_node) ||
      (a.place_type == exclusive_chip))
    this->chip_exclusive = true;
  } // END reserve_allocation_resources()



/*
 * adjust_open_resources()
 *
 */

void Chip::adjust_open_resources()

  {
  for (unsigned int i = 0; i < this->allocations.size(); i++)
    {
    this->reserve_allocation_resources(this->allocations[i]);
    } // END for each allocation

  } // END adjust_open_resources()



/*
 * initialize_accelerators_from_strings()
 */

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
 * "numanode" : {
 *   "os_index" : <index>,
 *   "cores" : <core range string>,
 *   "threads" : <thread range string>,
 *   "mem" : <memory in kb>,
 *   "gpus" : <gpu range string>,
 *   "mics" : <mic range string>,
 *   "allocation" : {
 *     "jobid" : "<jobid>",
 *     "cpus" : "<cpu range string>",
 *     "mem" : <memory in kb>,
 *     "exclusive" : <exclusive type>
 *     }
 *   }
 *
 * mics and gpus are optional and only present if they actually exist on the node
 * allocation is optional and there can be multiple of them
 */

Chip::Chip(
   
  const std::string        &json_layout,
  std::vector<std::string> &valid_ids) : id(0), totalCores(0), totalThreads(0), availableCores(0),
                                         availableThreads(0), total_gpus(0), available_gpus(0),
                                         total_mics(0), available_mics(0), chip_exclusive(false),
                                         memory(0), available_memory(0), cores(), devices(),
                                         allocations()

  {
  memset(chip_cpuset_string, 0, MAX_CPUSET_SIZE);
  memset(chip_nodeset_string, 0, MAX_NODESET_SIZE);

  if (json_layout.size() == 0)
    return;

  std::string cores;
  std::string threads;
  std::string gpus;
  std::string mics;

  parse_values_from_json_string(json_layout, cores, threads, gpus, mics, valid_ids);

  initialize_cores_from_strings(cores, threads);
  
  initialize_accelerators_from_strings(gpus, mics);

  adjust_open_resources();
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
  if (this->chip_exclusive == true)
    return(0);
  else
    return(this->available_mics);
  }

int Chip::get_available_gpus() const
  {
  if (this->chip_exclusive == true)
    return(0);
  else
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
  return(!this->chip_exclusive);
  }

bool Chip::is_completely_free() const
  {
  return(this->allocations.size() == 0);
  }



int Chip::initializePCIDevices(hwloc_obj_t chip_obj, hwloc_topology_t topology)
  {
  /* See if there are any accelerators */

#ifdef MIC
  this->initializeMICDevices(chip_obj, topology);
#endif

  return(PBSE_NONE);
  }



void Chip::displayAllocationsAsJson(

  stringstream &out) const

  {
  for (unsigned int i = 0; i < this->allocations.size(); i++)
    {
    const allocation  &a = this->allocations[i];
    std::string  cpus;
    std::string  gpus;
    std::string  mics;

    translate_vector_to_range_string(cpus, a.cpu_indices);
    translate_vector_to_range_string(gpus, a.gpu_indices);
    translate_vector_to_range_string(mics, a.mic_indices);

    out << ",\"allocation\":{\"jobid\":\"" << a.jobid;
    out << "\",\"cpus\":\"" << cpus << "\",\"mem\":" << a.memory;
    out << ",\"exclusive\":" << a.place_type;
    if (a.cores_only == true)
      out << ",\"cores_only\":1";
    else
      out << ",\"cores_only\":0";
    if (gpus.size() != 0)
      out << ",\"gpus\":\"" << gpus << "\"";
    if (mics.size() != 0)
      out << ",\"mics\":\"" << mics << "\"";
    out << "}";
    }
  } // END displayAllocationsAsJson()



void Chip::displayAsJson(

  stringstream &out,
  bool          include_jobs) const

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
    // Index 0 is the core id, the others are threads
    core_indices.push_back(this->cores[i].indices[0]);

    for (unsigned int j = 1; j < this->cores[i].indices.size(); j++)
      thread_indices.push_back(this->cores[i].indices[j]);
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
  out << "\"numanode\":{\"os_index\":" << this->id << ",\"cores\":\"" << core_range;
  out << "\",\"threads\":\"" << thread_range << "\",\"mem\":" << this->memory;

  if (gpu_range.size() != 0)
    out << ",\"gpus\":\"" << gpu_range << "\"";

  if (mic_range.size() != 0)
    out << ",\"mics\":\"" << mic_range << "\"";

  if (include_jobs)
    this->displayAllocationsAsJson(out);

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



void Chip::aggregate_allocations(

  std::vector<allocation> &master_list)

  {
  for (unsigned int i = 0; i < this->allocations.size(); i++)
    {
    bool match = false;

    for (unsigned int j = 0; j < master_list.size(); j++)
      {
      if (this->allocations[i].jobid == master_list[j].jobid)
        {
        master_list[j].add_allocation(this->allocations[i]);
        match = true;
        break;
        }
      }

    if (match == false)
      master_list.push_back(this->allocations[i]);
    }
  }



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
  c.indices.push_back(id + 16);
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
 * aggregate_allocation()
 *
 * Adds a to my list of allocations if one isn't already present for its jobid, or adds a 
 * to the existing allocation if one is present
 */

void Chip::aggregate_allocation(

  allocation &a)

  {
  bool found = false;

  for (unsigned int i = 0; i < this->allocations.size(); i++)
    {
    if (this->allocations[i].jobid == a.jobid)
      {
      this->allocations[i].add_allocation(a);
      found = true;
      break;
      }
    }

  if (found == false)
    this->allocations.push_back(a);
  } // END aggregate_allocation()



/*
 * free_core_count()
 *
 * Returns the number of cores that are completely free on this numa node
 *
 */
int Chip::free_core_count() const

  {
  int free_count = 0;
  for (size_t i = 0; i < this->cores.size(); i++)
    if (this->cores[i].is_free())
      free_count++;

  return(free_count);
  } // END free_core_count()



/*
 * how_many_tasks_fit()
 *
 * Determines how many tasks from req r fit on this chip
 * @param r - the req we're examining
 * @return the number of tasks that fit. This can be 0
 */

float Chip::how_many_tasks_fit(

  const req &r,
  int        place_type) const

  {
  float cpu_tasks;
  float gpu_tasks;
  float mic_tasks;
  float mem_tasks = 0;

  // Consider exclusive socket and node the same as exclusive chip for our purposes
  if ((place_type == exclusive_socket) ||
      (place_type == exclusive_node))
    place_type = exclusive_chip;

  if ((this->chip_exclusive == false) &&
      ((place_type != exclusive_chip) ||
       (this->chipIsAvailable()) == true))
    {
    // Need to handle place={core|thread}[=x]
    float max_cpus = r.getExecutionSlots();
    if (r.getPlaceCores() > 0)
      max_cpus = r.getPlaceCores();
    else if (r.getPlaceThreads() > 0)
      max_cpus = r.getPlaceThreads();

    if (r.getThreadUsageString() == use_cores)
      cpu_tasks = this->free_core_count() / max_cpus;
    else if (place_type == exclusive_legacy) // This is a -l resource request
      cpu_tasks = this->free_core_count() / max_cpus;
    else
      cpu_tasks = this->availableThreads / max_cpus;

    long long memory = r.getMemory();

    // Memory isn't required for submission
    if (memory != 0)
      {
      mem_tasks = this->available_memory * 1.0 / memory;

      // return the lower of the two values
      if (mem_tasks > cpu_tasks)
        mem_tasks = cpu_tasks;
      }
    else
      mem_tasks = cpu_tasks;

    float gpus = r.getGpus();
    if (gpus > 0)
      {
      gpu_tasks = this->available_gpus / gpus;
      if (mem_tasks > gpu_tasks)
        mem_tasks = gpu_tasks;
      }

    float mics = r.getMics();
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
 * getOpenThreadVector
 *
 * get a vector of thread indices for placing threads
 * from a numanode. Return true if all threads have been placed and
 * false if it is not.
 *
 * @param slots  - A vector of integers containing the indices
 *                 of the thread candidates for allocation.
 * @param execution_slots_per_task - The number of threads needed.
 */

bool Chip::getOpenThreadVector(

  std::vector<int> &slots,
  int               execution_slots_per_task)

  {
  unsigned int j = 0;
  int i = execution_slots_per_task;
  bool fits = false;

  /* this makes it so users can request gpus and mics 
     from numanodes which are not where the cores or threads
     are allocated */
  if (execution_slots_per_task == 0)
    return(true);
  slots.clear();
  i = execution_slots_per_task;
  j = 0;
  /* Can't get contiguous threads. Just get them where you can find them */
  // Get the thread indices we will use
  do
    {
    for (unsigned int x = 0; x < this->cores[j].indices.size(); x++)
      {
      int thread_index;
      if (this->cores[j].is_index_busy[x] == true)
        continue;

      thread_index = this->cores[j].indices[x];

      slots.push_back(thread_index);
      i--;
      if ((i == 0) || ((x + 1) == this->cores[j].indices.size()))
        {
        /* We fit if all of the execution slots have been filled
           or it we have used all the chip */
        fits = true;
        break;
        }
      }
    j++;

    }while((i != 0) && (j < this->cores.size()));
  
  return(fits);
  }
/*
 * getContiguousThreadVector
 *
 * get a vector of thread indices for placing threads
 * from a numanode. Return true if it is contiguous.
 * false if it is not.
 *
 * @param slots  - A vector of integers containing the indices
 *                 of the thread candidates for allocation.
 */

bool Chip::getContiguousThreadVector(

  std::vector<int> &slots,
  int               execution_slots_per_task)

  {
  unsigned int j = 0;
  int i = execution_slots_per_task;
  bool fits = false;

  /* this makes it so users can request gpus and mics 
     from numanodes which are not where the cores or threads
     are allocated */
  if (execution_slots_per_task == 0)
    return(true);

  /* First try to get contiguous threads */
  do
    {
    for (unsigned int x = 0; x < this->cores[j].indices.size(); x++)
      {
      int thread_index;

      /* if this thread is busy and we have already started creating a list,
           clear the list and start over; otherwise, continue to the next thread */
      if (this->cores[j].is_index_busy[x] == true)
        {
        if (slots.size() > 0)
          {
          i = execution_slots_per_task;
          slots.clear();
          }
        else
          continue;
        }

      thread_index = this->cores[j].indices[x];

      slots.push_back(thread_index);
      i--;
      if (i == 0)
        {
        // We fit if all of the execution slots have been filled
        fits = true;
        break;
        }
      }
    j++;
    
    }while((i != 0) && (j < this->cores.size()));

  if (fits == false)
    {
    slots.clear();
    i = execution_slots_per_task;
    j = 0;
    /* Can't get contiguous threads. Just get them where you can find them */
    // Get the thread indices we will use
    do
      {
      for (unsigned int x = 0; x < this->cores[j].indices.size(); x++)
        {
        int thread_index;
        if (this->cores[j].is_index_busy[x] == true)
          continue;

        thread_index = this->cores[j].indices[x];

        slots.push_back(thread_index);
        i--;
        if ((i == 0) || ((x + 1) == this->cores[j].indices.size()))
          {
          /* We fit if all of the execution slots have been filled
             or it we have used all the chip */
          fits = true;
          break;
          }
        }
      j++;

      }while((i != 0) && (j < this->cores.size()));
    }
  return(fits);
  }


/*
 * getContiguousCoreVector
 *
 * get a vector of core indices for placing cores
 * from a numanode. Return true if it is contiguous.
 * false if it is not.
 *
 * @param slots  - A vector of integers containing the indices
 *                 of the core candidates for allocation.
 */

bool Chip::getContiguousCoreVector(

  std::vector<int> &slots,
  int               execution_slots_per_task)

  {
  unsigned int j = 0;
  int i = execution_slots_per_task;
  bool fits = false;

  /* this makes it so users can request gpus and mics 
     from numanodes which are not where the cores or threads
     are allocated */
  if (execution_slots_per_task == 0)
    return(true);

  /* First try to get contiguous cores */
  do
    {
    if (this->cores[j].is_free() == true)
      {
      slots.push_back(j);
      i--;
      j++;
      if ((i ==0) || (j == this->cores.size()))
        {
        /* We fit if all of the execution slots have been filled
           or it we have used all the chip */
        fits = true;
        }
      }
    else
      {
      i = execution_slots_per_task;
      j++;
      slots.clear();
      }
    }while((i != 0) && (j < this->cores.size()));

  if (fits == false)
    {
    /* Can't get contiguous cores. Just get them where you can find them */
    // Get the core indices we will use
    j = 0;
    for (int i = 0; i < execution_slots_per_task; i++)
      {
      while (j < this->cores.size())
        {
        if (this->cores[j].is_free() == true)
          {
          slots.push_back(j);
          j++;
          break;
          }
        else
          j++;
        }
      }
    }
  return(fits);
  }


/*
 * place_tasks_execution_slots()
 *
 * places the task, knowing that we must use only cores
 *
 * @param execution_slots_per_task - for place=core=x. Number of lprocs to be bound to cpuset
 * @param cores_to_rsv - the number of cores to reserve for this task
 * @param chip_alloc - the allocation for this chip
 */

void Chip::place_tasks_execution_slots(

  int         execution_slots_per_task,
  int         to_rsv,
  allocation &chip_alloc,
  int         type)

  {
  std::vector<int> slots;
  float            step = 1.0;
  float            pin_index = 0.0;
  int              total_needed = execution_slots_per_task;

  if (to_rsv > execution_slots_per_task)
    {
    /* with place=core|thread=x we must reserve more cores|threads than we pin to the cpuset.
       When the reserved cpu pool spans chips, we need to keep track of the
       current index of the entire pool by offsetting the pin index and current index of
       this chip with how many cores|threads have already been pinned/reserved on other chips */
    if (execution_slots_per_task > 0)
      step = to_rsv / (float)execution_slots_per_task;
    else
      step = to_rsv;

    total_needed = to_rsv;
    }
 
  if (type == CORE)
    this->getContiguousCoreVector(slots, total_needed);
  else
    this->getContiguousThreadVector(slots, total_needed);
  
  for (std::vector<int>::iterator it = slots.begin(); it != slots.end(); it++)
    {
    if ((it - slots.begin() == floor(pin_index + 0.5)) &&
        (execution_slots_per_task > 0))
      {
      if (type == CORE)
        this->reserve_core(*it, chip_alloc);
      else
        this->reserve_place_thread(*it, chip_alloc);
        
      pin_index += step;
      }
    else
      {
      if (type == CORE)
        this->reserve_chip_core(*it, chip_alloc);
      else
        this->reserve_chip_place_thread(*it, chip_alloc);
      }
    }

  return;
  } // END place_tasks_execution_slots()


/*
 * place_task_for_legacy_threads()
 *
 * places the task, knowing that we can use threads
 *
 * @param execution_slots_per_task - the number of threads to bind for this task's cpuset
 * @param threads_to_rsv - the number of threads to reserve for this task
 * @param master - the allocation that has already been made
 * @param a - the allocation we're marking these used for
 */

void Chip::place_task_for_legacy_threads(

  int         execution_slots_per_task,
  int         threads_to_rsv,
  allocation &master,
  allocation &a)

  {
  std::vector<int> slots;
  float step = 1.0;
  float pin_index = 0.0;
  int num_threads = execution_slots_per_task;
  int total_rsvd_threads = 0;

  if (master.place_type == exclusive_legacy)
    this->getContiguousCoreVector(slots, num_threads);
  else
    this->getOpenThreadVector(slots, num_threads);
  
  for (std::vector<int>::iterator it = slots.begin(); it != slots.end(); it++)
    {
    if (it - slots.begin() == floor(pin_index + 0.5) - total_rsvd_threads)
      {
      if (master.place_type == exclusive_legacy)
        {
        int os_index = this->cores[*it].get_id();
        this->reserve_place_thread(os_index, a);
        }
      else
        this->reserve_place_thread(*it, a);
      pin_index += step;
      }
    }

  return;
  } // END place_task_for_legacy_threads()



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

  const req &r,
  int        place_type) const

  {
  bool           fits = false;
  int            max_cpus = r.getExecutionSlots();
  hwloc_uint64_t mem_per_task = r.getMemory();
  int            gpus_per_task = r.getGpus();
  int            mics_per_task = r.getMics();
  bool           cores_only = (r.getThreadUsageString() == use_cores);

  if (r.getPlaceCores() > 0)
    max_cpus = r.getPlaceCores();
  else if (r.getPlaceThreads() > 0)
    max_cpus = r.getPlaceThreads();

  if ((this->available_memory >= mem_per_task) &&
      (this->available_gpus >= gpus_per_task) &&
      (this->available_mics >= mics_per_task))
    {
    if (cores_only == true)
      {
      if (this->free_core_count() >= max_cpus)
        fits = true;
      }
    else if (place_type == exclusive_legacy)
      {
      if (this->free_core_count() >= max_cpus)
        fits = true;
      }
    else
      {
      if (this->availableThreads >= max_cpus)
        fits = true;
      }
    }

  return(fits);
  } // END task_will_fit()


/*
 * reserve_chip_core()
 *
 * Does everything reserver_core does except it does not
 * put the core_index in the cpu_indices. This is to
 * accommodate place=core=x.
 *
 * @param core_index  -  index number of core to be marked as busy
 * @param a           -  allocation object where record is kept.
 */
bool Chip::reserve_chip_core(

  int core_index, 
  allocation &a)

  {
  if (this->cores[core_index].is_free())
    {
    int os_index = this->cores[core_index].get_id();
   
    while ( this->cores[core_index].get_open_processing_unit() != -1 )
      continue;

    this->availableCores--;
    this->availableThreads -= this->cores[core_index].totalThreads;
    a.cpu_place_indices.push_back(os_index);
    a.cores++;
    a.threads += this->cores[core_index].totalThreads;
    return(true);
    }

  return(false);

  }

/*
 * reserve_core()
 *
 * Reserves the core at index core index if possible
 * @param core_index - the index of the core we wish to reserve
 * @param a - the allocation where we should record our reservation
 * @return true if a core was reserved, false otherwise
 */

bool Chip::reserve_core(

  int         core_index,
  allocation &a)

  {
  if (this->cores[core_index].is_free())
    {
    int os_index = this->cores[core_index].get_id();
    
    while ( this->cores[core_index].get_open_processing_unit() != -1 )
      continue;

    this->availableCores--;
    this->availableThreads -= this->cores[core_index].totalThreads;
    a.cpu_indices.push_back(os_index);
    a.cpus++;
    a.cores++;
    a.threads += this->cores[core_index].totalThreads;
    return(true);
    }

  return(false);
  } // END reserve_core()



bool Chip::reserve_place_thread(

  int         thread_index,
  allocation &a)

  {

  for (unsigned int i = 0; i < this->cores.size(); i++)
    {
     if (this->cores[i].reserve_processing_unit(thread_index) == true)
       {
       a.threads++;
       a.cpus++;
       a.cpu_indices.push_back(thread_index);
       this->availableThreads--;
       return(true);
       }
    }
  return(false);
  }


bool Chip::reserve_chip_place_thread(

  int         thread_index,
  allocation &a)

  {
  for (unsigned int i = 0; i < this->cores.size(); i++)
    {
    if (this->cores[i].reserve_processing_unit(thread_index) == true)
      {
      a.threads++;
      a.cpu_place_indices.push_back(thread_index);
      this->availableThreads--;
      return(true);
      }
    }
  return(false);
  }




/*
 * reserve_thread()
 *
 * Reserves a thread inside core with index core_index if possible
 * @param core_index - the index of the core whose thread we wish to reserve
 * @param a - the allocation where we should record our reservation
 * @return true if a thread was reserved, false otherwise.
 * DEPRECATED- reserve_place_thread() is the proper implementation for now
 */

bool Chip::reserve_thread(

  int         core_index,
  allocation &a)

  {
  int index = this->cores[core_index].get_open_processing_unit();

  if (index >= 0)
    {
    a.threads++;
    a.cpus++;
    a.cpu_indices.push_back(index);
    this->availableThreads--;
    return(true);
    }

  return(false);
  } // END reserve_thread()

/*
 * reserve_chip_thread()
 *
 * Reserves a thread inside core with index core_index if possible
 * but does not add the index to cpu_indices to be added to the cpuset.
 * @param core_index - the index of the core whose thread we wish to reserve
 * @param a - the allocation where we should record our reservation
 * @return true if a thread was reserved, false otherwise.
 * DEPRECATED- reserve_chip_place_thread() is the proper implementation for now
 */

bool Chip::reserve_chip_thread(

  int         core_index,
  allocation &a)

  {
  int index = this->cores[core_index].get_open_processing_unit();

  if (index >= 0)
    {
    a.threads++;
    a.cpu_place_indices.push_back(index);
    this->availableThreads--;
    return(true);
    }

  return(false);
  } // END reserve_chip_thread()


void Chip::calculateStepCounts(

  const int lprocs_per_task,
  const int processing_units_per_task,
  int &step, 
  int &step_remainder, 
  int &place_count, 
  int &place_count_remaining)

  {
   if (lprocs_per_task == 0)
     {
     step = 0;
     step_remainder = processing_units_per_task;
     place_count = 0;
     return;
     }

   if (lprocs_per_task == 1)
    {
    step = (processing_units_per_task/2) + 1;
    step_remainder = 0;
    }
  else
    {
    step = processing_units_per_task/lprocs_per_task; 
    step_remainder = processing_units_per_task % lprocs_per_task;
    }

  /* if step == 1 then we are placing cores in over half of the available cores 
     Some cores will have to be adjacent to each other so we need to calculate
     how many in a row to place together before we leave an empty slot */
  if (step == 1)
    {
    place_count = (processing_units_per_task/2) - step_remainder + (processing_units_per_task % 2);
    place_count_remaining = place_count;
    }

 }


/* spread_place_threads
 *
 * allocate contiguous threads and then only allocate
 * the lprocs_per_task_remaining to the cpuset.
 *
 * @param r  - The req to be filled
 * @param task_alloc - the allocation which will be filled
 * @param threads_per_task_remaining - the number of threads to allocate per task
 * @param lprocs_per_task_remaining - The number of logical processes to allocate the the cpuset
 *
 */

bool Chip::spread_place_threads(

  req         &r,
  allocation  &task_alloc,
  int         &threads_per_task_remaining,
  int         &lprocs_per_task_remaining,
  int         &gpus_remaining,
  int         &mics_remaining)

  {
  bool placed = false;
  bool fits = false;
  /* with place=core=x we all reserve more cores than we pin to the cpuset */
  /* step gives a rough estimate of how far apart the procs will be
     that get put in the cpuset */
  int step; 
  int step_remainder;
  int place_count = 0;
  int place_count_remaining = 0;
  int avail_threads_per_chip = this->getAvailableThreads();
  allocation from_this_chip(task_alloc.jobid.c_str());
  std::vector<int> slots;

  slots.clear();
  calculateStepCounts(lprocs_per_task_remaining, threads_per_task_remaining, step, step_remainder, 
                      place_count, place_count_remaining); 

  if ((this->chipIsAvailable() == false) || (avail_threads_per_chip < step))
    {
    /* If there are not enough available cores to make the spread there is
       no point in putting any of the task here */
    placed = false;
    return(placed); 
    }

  fits = this->getContiguousThreadVector(slots, threads_per_task_remaining);

  if (fits == true)
    {
    int step_count = step;

    if (lprocs_per_task_remaining == 1)
      step_count = 1;


    /* cores_placed and cores_to_fill are used because we only want to make sure we 
       fill the number of cores for this task */

    for (std::vector<int>::iterator it = slots.begin(); it != slots.end(); it++)
      {
      if ((step >= 2) && (lprocs_per_task_remaining != 0))
        {
        if (step_count == step)
          {
          this->reserve_place_thread(*it, from_this_chip);
          step_count = 1;
          threads_per_task_remaining--;
          lprocs_per_task_remaining--;
          }
        else
          {
          this->reserve_chip_place_thread(*it, from_this_chip);
          threads_per_task_remaining--;
          step_count++;
          }
        }
      else
        {
        if (place_count_remaining > 0)
          {
          this->reserve_place_thread(*it, from_this_chip);
          step_count = 1;
          threads_per_task_remaining--;
          lprocs_per_task_remaining--;
          place_count_remaining--;
          if (step_remainder == 0)
            place_count_remaining = place_count;
          }
         else
          {
          this->reserve_chip_place_thread(*it, from_this_chip);
          threads_per_task_remaining--;
          place_count_remaining = place_count;
          step_remainder--;
          }
        }
      }

    allocation remaining(r);

    remaining.set_gpus_remaining(gpus_remaining);
    remaining.set_mics_remaining(mics_remaining);

    place_accelerators(remaining, from_this_chip);
    from_this_chip.mem_indices.push_back(this->id);

    remaining.get_gpus_remaining(gpus_remaining);
    remaining.get_mics_remaining(mics_remaining);

    from_this_chip.mem_indices.push_back(this->id);
    this->aggregate_allocation(from_this_chip);
    task_alloc.add_allocation(from_this_chip);
    placed = true;
    }


  return(placed);
  }


/* spread_place_cores
 *
 * allocate contiguous core and then only allocate
 * the lprocs_per_task_remaining to the cpuset.
 *
 * @param r  - The req to be filled
 * @param task_alloc - the allocation which will be filled
 * @param cores_per_task_remaining - the number of cores to allocate per task
 * @param lprocs_per_task_remaining - The number of logical processes to allocate the the cpuset
 *
 */

bool Chip::spread_place_cores(

  req         &r,
  allocation  &task_alloc,
  int         &cores_per_task_remaining,
  int         &lprocs_per_task_remaining,
  int         &gpus_remaining,
  int         &mics_remaining)

  {
  bool placed = false;
  bool fits = false;
  /* with place=core=x we all reserve more cores than we pin to the cpuset */
  /* step gives a rough estimate of how far apart the procs will be
     that get put in the cpuset */
  int step; 
  int step_remainder;
  int place_count = 0;
  int place_count_remaining = 0;
  int avail_cores_per_chip = this->getAvailableCores();
  allocation from_this_chip(task_alloc.jobid.c_str());
  std::vector<int> slots;

  slots.clear();
  calculateStepCounts(lprocs_per_task_remaining, cores_per_task_remaining, step, step_remainder, 
                      place_count, place_count_remaining); 

  if ((this->chipIsAvailable() == false) || (avail_cores_per_chip < step))
    {
    /* If there are not enough available cores to make the spread there is
       no point in putting any of the task here */
    placed = false;
    return(placed); 
    }

  from_this_chip.cores_only = true;

  fits = this->getContiguousCoreVector(slots, cores_per_task_remaining);

  if (fits == true)
    {
    int step_count = step;

    if (lprocs_per_task_remaining == 1)
      step_count = 1;


    /* cores_placed and cores_to_fill are used because we only want to make sure we 
       fill the number of cores for this task */

    for (std::vector<int>::iterator it = slots.begin(); it != slots.end(); it++)
      {
      if (step >= 2)
        {
        if ((step_count == step) && (lprocs_per_task_remaining > 0))
          {
          this->reserve_core(*it, from_this_chip);
          step_count = 1;
          cores_per_task_remaining--;
          lprocs_per_task_remaining--;
          }
        else
          {
          this->reserve_chip_core(*it, from_this_chip);
          cores_per_task_remaining--;
          step_count++;
          }
        }
      else
        {
        if (place_count_remaining > 0)
          {
          this->reserve_core(*it, from_this_chip);
          step_count = 1;
          cores_per_task_remaining--;
          lprocs_per_task_remaining--;
          place_count_remaining--;
          if (step_remainder == 0)
            place_count_remaining = place_count;
          }
         else
          {
          this->reserve_chip_core(*it, from_this_chip);
          cores_per_task_remaining--;
          place_count_remaining = place_count;
          step_remainder--;
          }
        }
      }

    allocation remaining(r);

    remaining.set_gpus_remaining(gpus_remaining);
    remaining.set_mics_remaining(mics_remaining);

    place_accelerators(remaining, from_this_chip);
    from_this_chip.mem_indices.push_back(this->id);

    remaining.get_gpus_remaining(gpus_remaining);
    remaining.get_mics_remaining(mics_remaining);

    this->aggregate_allocation(from_this_chip);
    task_alloc.add_allocation(from_this_chip);
    placed = true;
    }


  return(placed);
  }



/*
 * spread_place()
 *
 * Places a task from req r on this chip and spread it appropriately
 *
 * @param r - the req whose task we're placing
 * @param task_alloc - the allocation for this task
 * @param execution_slots_per - the number of execution slots to reserve from this chip
 * @param execution_slots_remainder - a number of extra execution slots to grab - get one 
 * and decrement if > 0
 * @return true if the task was placed, false otherwise.
 */

bool Chip::spread_place(

  req        &r,
  allocation &task_alloc,
  int         execution_slots_per,
  int        &execution_slots_remainder)

  {
  bool task_placed = false;

  if ((this->chipIsAvailable() == true) &&
      ((execution_slots_per + execution_slots_remainder) <= this->totalThreads))
    {
    allocation from_this_chip(task_alloc.jobid.c_str());
    int        placed = 0;
    int        to_add = 0;

    if (execution_slots_remainder > 0)
      to_add = 1;

    from_this_chip.place_type = task_alloc.place_type;

    if (this->totalCores >= execution_slots_per + to_add)
      {
      if (to_add == 1)
        {
        execution_slots_per++;
        execution_slots_remainder--;
        }

      // Spread over just the cores
      float step = 1.0;
      if (execution_slots_per > 0)
       step = this->totalCores / (float)execution_slots_per;

      from_this_chip.cores_only = true;

      for (float i = 0.0; placed < execution_slots_per; i+= step)
        {
        this->reserve_core(std::floor(i + 0.5), from_this_chip);
        placed++;
        }
      }
    else
      {
      // Spread over the cores and the threads - this option doesn't really make any
      // sense but I suppose we have to code for it
      int per_core = execution_slots_per / this->totalCores;
      int remainder = execution_slots_per % this->totalCores;

      // Place one extra if we still have a remainder
      if (execution_slots_remainder > 0)
        {
        placed--;
        execution_slots_remainder--;
        }

      for (unsigned int i = 0; i < this->cores.size() && placed < execution_slots_per; i++)
        {
        for (int j = 0; j < per_core; j++)
          {
          if (this->reserve_thread(i, from_this_chip) == true)
            placed++;
          }
        }

      while (remainder > 0)
        {
        for (unsigned int i = 0; i < this->cores.size() && placed < execution_slots_per; i++)
          {
          if (this->reserve_thread(i, from_this_chip) == true)
            {
            placed++;
            remainder--;
            }
          }
        }
      }

    from_this_chip.mem_indices.push_back(this->id);

    task_placed = true;
    this->chip_exclusive = true;
    this->aggregate_allocation(from_this_chip);
    task_alloc.add_allocation(from_this_chip);
    }

  return(task_placed);
  } // spread_place()



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

  req        &r,
  allocation &master,
  int         to_place,
  const char *hostname)

  {
  allocation     chip_alloc(master.jobid.c_str());
  int            tasks_placed = 0;
  int            execution_slots_per_task = r.getExecutionSlots();
  hwloc_uint64_t mem_per_task = r.getMemory();
  int            practical_place = master.place_type;

  chip_alloc.place_type = master.place_type;
  chip_alloc.place_cpus = master.place_cpus;

  // Practically, we should treat place=node, place=socket, and
  // place=numanode as the same
  if ((practical_place == exclusive_socket) ||
      (practical_place == exclusive_node))
    practical_place = exclusive_chip;

  if ((practical_place != exclusive_chip) ||
      (this->chipIsAvailable() == true))
    {
    if (this->chip_exclusive == false)
      {
      if (r.getThreadUsageString() == use_cores)
        chip_alloc.cores_only = true;
      else
        chip_alloc.cores_only = false;

      for (; tasks_placed < to_place; tasks_placed++)
        {
        if (task_will_fit(r, master.place_type) == false)
          break;

        allocation task_alloc(master.jobid.c_str());
        task_alloc.cores_only = chip_alloc.cores_only;
        task_alloc.place_cpus = chip_alloc.place_cpus;

        this->available_memory -= mem_per_task;
        task_alloc.memory += mem_per_task;
        
        if (task_alloc.cores_only == true)
          {
          int cores_to_rsv = execution_slots_per_task;
          if(r.getPlaceCores() > 0)
            cores_to_rsv = r.getPlaceCores();

          place_tasks_execution_slots(execution_slots_per_task, cores_to_rsv, task_alloc, CORE);
          }
        else if ((chip_alloc.place_type == exclusive_legacy) || 
                 (chip_alloc.place_type == exclusive_legacy2))
          {
          int threads_to_rsv = execution_slots_per_task;
          if (r.getPlaceThreads() > 0)
            threads_to_rsv = r.getPlaceThreads();

          place_task_for_legacy_threads(execution_slots_per_task, threads_to_rsv, master, task_alloc);
          }
        else
          {
          int threads_to_rsv = execution_slots_per_task;
          if(r.getPlaceThreads() > 0)
            threads_to_rsv = r.getPlaceThreads();

          place_tasks_execution_slots(execution_slots_per_task, threads_to_rsv, task_alloc, THREAD);
          }

        allocation remaining(r);

        place_accelerators(remaining, task_alloc);
        task_alloc.mem_indices.push_back(this->id);

        task_alloc.set_host(hostname);
        r.record_allocation(task_alloc);
        chip_alloc.add_allocation(task_alloc);

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
      this->allocations.push_back(chip_alloc);
      master.add_allocation(chip_alloc);
      }
    }

  return(tasks_placed);
  } // END place_task()



/*
 * reserve_accelerator()
 *
 * Reserves a an accelerator with the specified type
 * @param type - either GPU or MIC for now
 * @return the os index of the accelerator that was reserved or -1 if none was found
 */

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
 * places as many accelerators from remaining as possible, decrements remaining and increments a
 * @param remaining - an allocation specifying what accelerators need to be reserved
 * @param a - the recording allocation where the indices should be stored
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



/*
 * free_accelerator()
 * 
 * frees the specified accelerator
 * @param index - the os index of the accelerator
 * @param type - the type of accelerator
 */

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



/*
 * free_accelerators()
 *
 * frees the accelerators reserved by allocation a
 * @param a - the allocation specifying which accelerators are reserved
 */

void Chip::free_accelerators(

  allocation &a)

  {
  for (unsigned int i = 0; i < a.gpu_indices.size(); i++)
    this->free_accelerator(a.gpu_indices[i], GPU);

  for (unsigned int i = 0; i < a.mic_indices.size(); i++)
    this->free_accelerator(a.mic_indices[i], MIC_TYPE);

  } // END free_accelerators()



/*
 * place_all_execution_slots()
 */

void Chip::place_all_execution_slots(

  req        &r,
  allocation &master)

  {
  // Currently we regenerate the list of accelerators to place for each numa node because
  // we're just going to give every accelerator to this job.
  allocation remaining(r);
  allocation chip_alloc(master.jobid.c_str());
  place_accelerators(remaining, chip_alloc);
  chip_alloc.cores_only = master.cores_only;
  this->chip_exclusive = true;

  for (int c=0; c < this->cores.size(); c++)
    {
    if (chip_alloc.cores_only == true)
      {
      reserve_core(c, chip_alloc);
      }
    else
      {
      for (int t=0; t < this->cores[c].indices.size(); t++)
        reserve_place_thread(this->cores[c].indices[t], chip_alloc);
      }
    }
  
  chip_alloc.mem_indices.push_back(this->id);
  this->allocations.push_back(chip_alloc);
  master.add_allocation(chip_alloc);
  } // END place_all_execution_slots()



/*
 * partially_place_task()
 *
 * Places whatever can be placed from the task specified by remaining onto this chip
 * @param remaining (I/O) - specifies how much of the task remains to be placed. Updated.
 * @param task_alloc (O) - the allocation for one task of this job
 */

bool Chip::partially_place_task(

  allocation &remaining,
  allocation &task_alloc)

  {
  bool        placed_something = false;
  int         place_type;
  allocation  chip_alloc(task_alloc.jobid.c_str());
  
  int max_cpus = remaining.cpus;
  if (remaining.place_cpus > 0)
    max_cpus = remaining.place_cpus;

  // handle memory
  if (remaining.memory > this->available_memory)
    {
    chip_alloc.memory = this->available_memory;
    remaining.memory -= this->available_memory;
    this->available_memory = 0;
    }
  else
    {
    this->available_memory -= remaining.memory;
    chip_alloc.memory = remaining.memory;
    remaining.memory = 0;
    }

  remaining.get_place_type(place_type);
  task_alloc.place_type = place_type;

  if (remaining.cores_only == true)
    {
    place_tasks_execution_slots(remaining.cpus, max_cpus, chip_alloc, CORE);
    chip_alloc.cores_only = true;
    }
  else if ((place_type == exclusive_legacy) || (place_type == exclusive_legacy2))
    {
    place_task_for_legacy_threads(remaining.cpus, max_cpus, task_alloc, chip_alloc);
    }
  else
    place_tasks_execution_slots(remaining.cpus, max_cpus, chip_alloc, THREAD);

  place_accelerators(remaining, chip_alloc);
  
  if ((chip_alloc.cpu_indices.size() > 0) ||
      (chip_alloc.cpu_place_indices.size() > 0) ||
      (chip_alloc.memory > 0) ||
      (chip_alloc.gpu_indices.size() > 0) ||
      (chip_alloc.mic_indices.size() > 0))
    {
    chip_alloc.mem_indices.push_back(this->id);
    remaining.cpus -= chip_alloc.cpu_indices.size();
    
    if (remaining.place_cpus > 0)
      {
      // place_cpus = total number of cpus to be placed, but cpu_place_indices 
      // only has the one that won't go into the cpuset, so we need to subtract both
      remaining.place_cpus -= chip_alloc.cpu_place_indices.size();
      remaining.place_cpus -= chip_alloc.cpu_indices.size();
      }

    this->allocations.push_back(chip_alloc);
    task_alloc.add_allocation(chip_alloc);

    placed_something = true;
    }

  // Practically, we should treat place=node, place=socket, and
  // place=numanode as the same, becase they all make the numanode exclusive
  if ((task_alloc.place_type == exclusive_socket) ||
      (task_alloc.place_type == exclusive_node))
    this->chip_exclusive = true;

  return(placed_something);
  } // END partially_place_task()



/*
 * free_cpu_index()
 *
 * Marks processing unit at index as available
 * @param index - the os index of the processing unit to mark as unused
 */

void Chip::free_cpu_index(

  int  index,
  bool increment_available_cores)

  {
  bool core_is_now_free = false;
  for (unsigned int i = 0; i < this->cores.size(); i++)
    {
    if (this->cores[i].free == true)
      continue;
      
    if (increment_available_cores == true)
      {
      // For cores_only, the passed-in index should be the core's index
      if (this->cores[i].get_id() != index)
        continue;

      // Free the entire core (including all its threads)
      for( unsigned int j = 0; j < this->cores[i].indices.size(); j++)
        {
        this->cores[i].free_pu_index(this->cores[i].indices[j], core_is_now_free);
        }

      if (core_is_now_free == true)
        {
        this->availableCores++;
        return;
        }
      }
    else
      {
      if (this->cores[i].free_pu_index(index, core_is_now_free) == true)
        return;
      }
    }

  } // END free_cpu_index()



/*
 * uncount_allocation()
 *
 * Updates the counts to reflect that the allocation at index will be removed
 *
 * @param i - the index of the allocation that will be removed
 */

void Chip::uncount_allocation(

  int i)

  {
  this->availableThreads += this->allocations[i].threads;
  this->available_memory += this->allocations[i].memory;

  // Now mark the individual cores as available
  for (unsigned int j = 0; j < this->allocations[i].cpu_indices.size(); j++)
    free_cpu_index(this->allocations[i].cpu_indices[j], this->allocations[i].cores_only);

  // do the same for place=core or place=thread indices
  for (unsigned int j = 0; j < this->allocations[i].cpu_place_indices.size(); j++)
    free_cpu_index(this->allocations[i].cpu_place_indices[j], this->allocations[i].cores_only);

  free_accelerators(this->allocations[i]);
  } // END uncount_allocation()



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
  std::vector<int>  to_remove;
  bool totally_free = false;

  for (unsigned int i = 0; i < this->allocations.size(); i++)
    {
    if (this->allocations[i].jobid == jobid)
      {
      to_remove.push_back(i);
      
      this->uncount_allocation(i);
      }
    }

  for (size_t i = 0; i < to_remove.size(); i++)
    // Subtract i because we are dynamically changing the vector as we erase, removing 1 element
    // each time
    this->allocations.erase(this->allocations.begin() + to_remove[i] - i);

  if (this->allocations.size() == 0)
    {
    this->chip_exclusive = false;
    totally_free = true;
    }

  return(totally_free);
  } // END free_task()



void Chip::remove_last_allocation(

  const char *jobid)

  {
  if (this->allocations.size() > 0)
    {
    size_t index = this->allocations.size() - 1;
    if (this->allocations[index].jobid == jobid)
      {
      this->uncount_allocation(index);

      this->allocations.erase(this->allocations.begin() + index);
      }
    }
  } // END remove_last_allocation()



/*
 * store_pci_device_appropriately()
 *
 * Stores device in this numa node if the cpuset overlaps or if forced
 * @param device - the device to be optionally stored on this numa node
 * @param force - if true, the device will automatically be considered part of this numa node
 * @return true if the device was stored on this numa node
 */

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
  } // END store_pci_device_appropriately() 



/*
 * cpusets_overlap()
 *
 * Tests if the specified cpuset string overlaps with the cpuset for this numa node
 * @param other - a range string specifying the other cpuset
 * @return true if the cpuset for this numa node overlaps with the specified cpuset
 */

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
  } // END cpusets_overlap()



/*
 * has_socket_exclusive_allocation()
 *
 * @return true if this chip has an allocation that is socket exclusive
 */

bool Chip::has_socket_exclusive_allocation() const

  {
  for (unsigned int i = 0; i < this->allocations.size(); i++)
    {
    if ((this->allocations[i].place_type == exclusive_socket) ||
        (this->allocations[i].place_type == exclusive_node))
      return(true);
    }

  return(false);
  } // END has_socket_exclusive_allocation()


#endif /* PENABLE_LINUX_CGROUPS */  
