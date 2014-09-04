#include <iostream>
#include <string>
#include <vector>
#include "machine.hpp"
#include <errno.h>
#include <hwloc.h>
#include "pbs_config.h"

#ifdef PENABLE_LINUX26_CPUSETS

#ifdef MIC
#include <hwloc/intel-mic.h>
#endif

#ifdef NVIDIA_GPU
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
    int rc;

    id = device_obj->logical_index;
    name = device_obj->name;
    info_name = device_obj->infos->name;
    info_value = device_obj->infos->value;


#ifdef MIC
    nearest_cpuset = hwloc_bitmap_alloc();
    if (nearest_cpuset == NULL)
      return(-1);

    rc = hwloc_intel_mic_get_device_cpuset(topology, idx, nearest_cpuset);
    if (rc != 0)
      {
      string  buf;

      buf << "Could not get cpuset of " << name.c_str();
      log_err(-1, __func__, "Could not get cpuset of %s", buf);
      }
    hwloc_bitmap_list_snprintf(cpuset_string, MAX_CPUSET_SIZE, nearest_cpuset);
#endif

    return(PBSE_NONE);
    }
 

#endif /* PENABLE_LINUX26_CPUSETS */   
