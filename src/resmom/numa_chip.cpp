#include <iostream>
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
#include "mom_server_lib.h" /* log_nvml_error */

using namespace std;

#define INTEL 1
#define AMD   2


  Chip::Chip()
    {
    totalThreads = 0;
    totalCores = 0;
    id = 0;
    availableThreads = 0;
    availableCores = 0;
    memset(chip_cpuset_string, 0, MAX_CPUSET_SIZE);
    memset(chip_nodeset_string, 0, MAX_NODESET_SIZE);
    chip_is_available = false;
    }

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

    this->chip_is_available = true;
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
    this->memory = chip_obj->memory.local_memory;
    this->chip_cpuset = chip_obj->allowed_cpuset;
    this->chip_nodeset = chip_obj->allowed_nodeset;
    hwloc_bitmap_list_snprintf(this->chip_cpuset_string, MAX_CPUSET_SIZE, this->chip_cpuset);
    hwloc_bitmap_list_snprintf(this->chip_nodeset_string, MAX_CPUSET_SIZE, this->chip_nodeset);
    this->chip_is_available = true;
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

  int Chip::getTotalCores()
    {
    return(this->totalCores);
    }

  int Chip::getTotalThreads()
    {
    return(this->totalThreads);
    }

  int Chip::getAvailableCores()
    {
    return(this->availableCores);
    }

  int Chip::getAvailableThreads()
    {
    return(this->availableThreads);
    }

  int Chip::getMemoryInBytes()
    {
    return(this->memory);
    }

  bool Chip::chipIsAvailable()
    {
    return(this->chip_is_available);
    }

#ifdef NVIDIA_GPUS
  #ifdef NVML_API
  int Chip::initializeNVIDIADevices(hwloc_obj_t chip_obj, hwloc_topology_t topology)
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
          continue;
    
        if (chip_obj == NULL) 
          { // this came from the Non NUMA
          PCI_Device new_device;
    
          new_device.initializePCIDevice(gpu_obj, idx, topology);
          this->devices.push_back(new_device);
          }
        else
          {
          /* TODO: test this block of code on a NUMA-capable server with the k40/k80 installed */
          PCI_Device new_device;
          ancestor_obj = hwloc_get_ancestor_obj_by_type(topology, HWLOC_OBJ_NODE, gpu_obj);
          if (ancestor_obj != NULL)
            {
            if (ancestor_obj->logical_index == chip_obj->logical_index)
              {
              PCI_Device new_device;
    
              new_device.initializePCIDevice(gpu_obj, idx, topology);
              this->devices.push_back(new_device);
              }
            }
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
  #endif
#endif

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

        new_device.initialzePCIDevice(mic_obj, idx, topology);
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

#ifdef NVIDIA_GPUS
  #ifdef NVML_API
    this->initializeNVIDIADevices(chip_obj, topology);
  #endif
#endif
    return(PBSE_NONE);

    }

  void Chip::displayAsString(

    stringstream &out) const

    {
    out << "    Chip " << this->id << " (" << this->memory / 1024 << "KB)\n";

    for (unsigned int i = 0; i < this->cores.size(); i++)
      this->cores[i].displayAsString(out);
    
    for (unsigned int i = 0; i < this->devices.size(); i++)
      this->devices[i].displayAsString(out);
    } // END displayAsString() 



  void Chip::setMemoryInBytes(

    long long memory)

    {
    this->memory = memory;
    }
  
  void Chip::setId(

    int id)

    {
    this->id = id;
    }

#endif /* PENABLE_LINUX26_CPUSETS */  
