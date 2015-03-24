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

using namespace std;

#define INTEL 1
#define AMD   2

  PCI_Device::PCI_Device()
    {
    id = -1;
    memset(cpuset_string, 0, MAX_CPUSET_SIZE);
    }

   PCI_Device::~PCI_Device()
    {
    id = -1;
    memset(cpuset_string, 0, MAX_CPUSET_SIZE);
    }

  int PCI_Device::initializePCIDevice(hwloc_obj_t device_obj, int idx, hwloc_topology_t topology)
    {

    id = device_obj->logical_index;
    name = device_obj->name;
    if (device_obj->infos != NULL)
      {
      info_name = device_obj->infos->name;
      info_value = device_obj->infos->value;
      }


#ifdef MIC
    int rc;

    nearest_cpuset = hwloc_bitmap_alloc();
    if (nearest_cpuset == NULL)
      return(PBSE_MEM_MALLOC);

    rc = hwloc_intel_mic_get_device_cpuset(topology, idx, nearest_cpuset);
    if (rc != 0)
      {
      string  buf;

      buf = "could not get cpuset of ";
      buf = buf + name.c_str();
      log_err(-1, __func__, buf.c_str());
      }
    hwloc_bitmap_list_snprintf(cpuset_string, MAX_CPUSET_SIZE, nearest_cpuset);
#endif
#ifdef NVIDIA_GPUS
    int rc;
    nvmlDevice_t  gpu_device;
    
    rc = nvmlDeviceGetHandleByIndex(idx, &gpu_device);
    if (rc != NVML_SUCCESS)
      {
      string buf;

      buf = "nvmlDeviceGetHandleByIndex failed for nvidia gpus";
      buf = buf + name.c_str();
      log_err(-1, __func__, buf.c_str());
      }
    else
      {
      nearest_cpuset = hwloc_bitmap_alloc();
      if (nearest_cpuset == NULL)
        return(PBSE_MEM_MALLOC);

      rc = hwloc_nvml_get_device_cpuset(topology, gpu_device, nearest_cpuset);
      if (rc != 0)
        {
        string  buf;

        buf = "could not get cpuset of ";
        buf = buf + name.c_str();
        log_err(-1, __func__, buf.c_str());
        }
      hwloc_bitmap_list_snprintf(cpuset_string, MAX_CPUSET_SIZE, nearest_cpuset);
      }

#endif

    return(PBSE_NONE);
    }
 
  void PCI_Device::displayAsString(

    stringstream &out) const

    {
    out << "      pci " << this->id << " " << this->name << "\n";
    } // end displayasstring()
    
  void PCI_Device::setName(
      
    const string &name)

    {
    this->name = name;
    }

  void PCI_Device::setId(
    
    int id)

    {
    this->id = id;
    }

#endif /* PENABLE_LINUX_CGROUPS */   
