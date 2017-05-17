#include <pbs_config.h>
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

/*
 * get_non_nvml_device()
 *
 * hwloc's method to identify a GPU failed, so use ours. Theirs fails a lot.
 *
 * @param topology - the topology that the device should fall under
 * @param device - the nvml device handle
 * @param identified - a list of the previously identified devices to make sure
 *                     we aren't storing the same one multiple times.
 * @return an hwloc obj pointing to the GPU
 */

hwloc_obj_t Machine::get_non_nvml_device(
    
  hwloc_topology_t       topology,
  nvmlDevice_t           device,
  std::set<hwloc_obj_t> &identified)

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
    return(NULL);

  osdev = NULL;
  while ((osdev = hwloc_get_next_osdev(topology, osdev)) != NULL) 
    {
    hwloc_obj_t pcidev = osdev->parent;
    if ((strncmp(osdev->name, "card", 4)) &&
        (strncmp(osdev->name, "controlD", 8)))
      continue;

    if ((pcidev != NULL) &&
        (pcidev->type == HWLOC_OBJ_PCI_DEVICE) &&
        (pcidev->attr->pcidev.domain == pci.domain) &&
        (pcidev->attr->pcidev.bus == pci.bus) &&
        (pcidev->attr->pcidev.dev == pci.device) &&
        (pcidev->attr->pcidev.func == 0))
      return(osdev);
    }

  // We haven't found it yet, but there are GPUs and hwloc knows it. Just match on type.
  osdev = NULL;
  while ((osdev = hwloc_get_next_osdev(topology, osdev)) != NULL)
    {
    if ((osdev->attr->osdev.type == HWLOC_OBJ_OSDEV_GPU) &&
        (identified.find(osdev) == identified.end()))
      return(osdev);
    }

  return(NULL);
  } // END get_non_nvml_device()



int Machine::initializeNVIDIADevices(
    
  hwloc_obj_t      machine_obj,
  hwloc_topology_t topology)

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

  std::vector<unsigned int> unfound_ids;
  unsigned int              device_count = 0;
  unsigned int              found_devices = 0;

  /* Get the device count. */
  rc = nvmlDeviceGetCount(&device_count);
  if (rc == NVML_SUCCESS)
    {
    nvmlDevice_t gpu;
    std::set<hwloc_obj_t> identified;

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
        // This was not an nvml device. We'll look for a different GPU - many aren't recognized,
        // including k80s.
        gpu_obj = this->get_non_nvml_device(topology, gpu, identified);
        if (gpu_obj == NULL)
          {
          unfound_ids.push_back(idx);
          continue;
          }
        }

      identified.insert(gpu_obj);
      found_devices++;
        
      /* The ancestor was not a numa chip. Is it the machine? */
      ancestor_obj = hwloc_get_ancestor_obj_by_type(topology, HWLOC_OBJ_MACHINE, gpu_obj);
      if (ancestor_obj != NULL)
        {
        char buf[MAXLINE];
        rc = nvmlDeviceGetName(gpu, buf, sizeof(buf));

        PCI_Device new_device;
  
        new_device.initializePCIDevice(gpu_obj, idx, topology);

        store_device_on_appropriate_chip(new_device, false);

        // hwloc sees the K80 as a single device. We want to display two to stay in sync
        // with NVML, so store an extra for each K80
        if (!strncmp(buf, "Tesla K80", 9))
          {
          new_device.setId(new_device.get_id() + 1);
          store_device_on_appropriate_chip(new_device, false);
          found_devices++;
          idx++;
          }
        }
      }

    if (found_devices != device_count)
      {
      sprintf(log_buffer, "NVML reports %u devices, but we only found %u. Placement will be approximated.",
        device_count, found_devices);
      log_err(-1, __func__, log_buffer);

      for (size_t i = 0; i < unfound_ids.size(); i++)
        {
        PCI_Device new_device;
        new_device.initializePCIDevice(NULL, unfound_ids[i], topology);
        store_device_on_appropriate_chip(new_device, true);
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
void PCI_Device::initializeGpu(

  int              idx,
  hwloc_topology_t topology)

  {
  int rc;
  nvmlDevice_t  gpu_device;
  
  id = idx;
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
    if (nearest_cpuset != NULL)
      {
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
    }

  this->type = GPU;

  }
#endif

#endif
