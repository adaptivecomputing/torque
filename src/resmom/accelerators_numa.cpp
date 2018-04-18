#include <pbs_config.h>
#include <algorithm>
#include "log.h"
#include "pbs_error_db.h"
#include "pbs_error.h"

#ifdef PENABLE_LINUX_CGROUPS
#include "machine.hpp"

#ifdef MIC
#include <hwloc/intel-mic.h>
#endif

#ifdef NVIDIA_GPUS
#ifdef NVML_API
#include <hwloc/nvml.h>
#include <nvml.h>

void log_nvml_error(nvmlReturn_t rc, char* gpuid, const char* id);
#endif
#endif


#ifdef NVML_API

int Machine::initializeNVIDIADevices(
    
  hwloc_obj_t      machine_obj,
  hwloc_topology_t topology)

  {
  nvmlReturn_t rc;
  int rc_init = PBSE_NONE;
  unsigned int device_count;

  /* Initialize the NVML handle. 
   *
   * nvmlInit should be called once before invoking any other methods in the NVML library. 
   * A reference count of the number of initializations is maintained. Shutdown only occurs 
   * when the reference count reaches zero.
   *
   * This routine does not use hwloc since as of <= 1.11.7 it is known to fail to identifiy NVIDIA devices
   * on some systems.
   * */
  rc = nvmlInit();
  if (rc != NVML_SUCCESS && rc != NVML_ERROR_ALREADY_INITIALIZED)
    {
    log_nvml_error(rc, NULL, __func__);
    return(PBSE_NONE);
    }

  /* Get the device count. */
  rc = nvmlDeviceGetCount(&device_count);
  if (rc == NVML_SUCCESS)
    {
    nvmlDevice_t gpu;
    std::set<hwloc_obj_t> identified;

    /* Get the nvml device handle at each index */
    for (unsigned int idx = 0; idx < device_count; idx++)
      {
      PCI_Device new_device;

      if (new_device.initializePCIDevice(NULL, idx, topology) != PBSE_NONE)
        {
        rc_init = -1;
        break;
        }
      store_device_on_appropriate_chip(new_device, false);
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

  return(rc_init);
  }
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



#ifdef MIC
void PCI_Device::initializeMic(

  int              idx,
  hwloc_topology_t topology)

  {
  int rc;

  nearest_cpuset = hwloc_bitmap_alloc();

  rc = hwloc_intel_mic_get_device_cpuset(topology, idx, nearest_cpuset);
  if (rc != 0)
    {
    string  buf;

    buf = "could not get cpuset of ";
    buf = buf + name.c_str();
    log_err(-1, __func__, buf.c_str());
    }
  hwloc_bitmap_list_snprintf(cpuset_string, MAX_CPUSET_SIZE, nearest_cpuset);

  this->type = MIC_TYPE;
  } // END initializeMic()
#endif



#ifdef NVIDIA_GPUS
/*
 * due to an hwloc limitation, it can't be depended on to report the cpu information about an nvml device
 * so we work around that by getting the cpulist information in a different way (sysfs)
 */

void PCI_Device::initializeGpu(

  int              idx)

  {
  int rc;
  nvmlDevice_t  gpu_device;
  nvmlPciInfo_t pci;
  char cpulist_path[PATH_MAX];
  FILE *fp;
  char *p;

  id = idx;

  this->type = GPU;

  // look up nearest cpuset of device
  //   Can do this one of 3 ways:
  //     1) use hwloc (broken in 1.11--doesn't id some gpu devices)
  //     2) use nvmlDeviceGetCpuAffinity() (need to convert bitmap to string)
  //     3) look up in sysfs (no conversion needed so use this method)
  
  rc = nvmlDeviceGetHandleByIndex(idx, &gpu_device);
  if (rc != NVML_SUCCESS)
    {
    string buf;

    buf = "nvmlDeviceGetHandleByIndex failed for nvidia gpus";
    buf = buf + name.c_str();
    log_err(-1, __func__, buf.c_str());
    return;
    }

  // get the PCI info from the NVML identified device
  rc = nvmlDeviceGetPciInfo(gpu_device, &pci);
  if (rc != NVML_SUCCESS)
    {
    snprintf(log_buffer, sizeof(log_buffer), "nvmlDeviceGetPciInfo failed with %d for index %d",
      rc, idx);
    log_err(-1, __func__, log_buffer);
    return;
    }

  string busId = pci.busId;

  // if it starts with 8 zeros, cut it down to 4 zeros for sysfs
  if (busId.substr(0,8) == "00000000")
    busId = busId.substr(4);

  // convert bus id to lower case
  transform(busId.begin(), busId.end(), busId.begin(), ::tolower);
   
  // build path to cpulist for this PCI device
  snprintf(cpulist_path, sizeof(cpulist_path), "/sys/bus/pci/devices/%s/local_cpulist",
    busId.c_str());

  // open cpulist
  if ((fp = fopen(cpulist_path, "r")) == NULL)
    {
    snprintf(log_buffer, sizeof(log_buffer), "could not open %s", cpulist_path);
    log_err(-1, __func__, log_buffer);
    return;
    }

  // read cpulist
  fgets(cpuset_string, MAX_CPUSET_SIZE, fp);
  fclose(fp);

  // delete the trailing newline
  if ((p = strchr(cpuset_string, '\n')) != NULL)
    *p = '\0';
  }
#endif

#endif
