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

const int MIC_TYPE = 0;
const int GPU = 1;

PCI_Device::PCI_Device() : name(), id(-1), info_name(), info_value(), nearest_cpuset(NULL),
                           type(-1), busy(false)
  {
  memset(cpuset_string, 0, MAX_CPUSET_SIZE);
  }

PCI_Device::PCI_Device(

  const PCI_Device &other) : name(other.name), id(other.id), info_name(other.info_name),
                             info_value(other.info_value), type(other.type), busy(other.busy)

  {
  memcpy(this->cpuset_string, other.cpuset_string, sizeof(this->cpuset_string));
  }

PCI_Device &PCI_Device::operator =(

  const PCI_Device &other)

  {
  this->name = other.name;
  this->id = other.id;
  this->info_name = other.info_name;
  this->info_value = other.info_value;

  memcpy(this->cpuset_string, other.cpuset_string, sizeof(this->cpuset_string));
  this->type = other.type;

  return(*this);
  }

PCI_Device::~PCI_Device()
  {
  id = -1;
  memset(cpuset_string, 0, MAX_CPUSET_SIZE);
  }

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
  this->initializeMic(idx, topology);
#endif

#ifdef NVIDIA_GPUS
  this->initializeGpu(idx, topology);
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

const char *PCI_Device::get_cpuset() const

  {
  return(this->cpuset_string);
  }

int PCI_Device::get_type() const

  {
  return(this->type);
  }

bool PCI_Device::is_busy() const

  {
  return(this->busy);
  }

void PCI_Device::set_state(

  bool in_use)

  {
  this->busy = in_use;
  }

void PCI_Device::set_type(

  int type)

  {
  this->type = type;
  }

int PCI_Device::get_id() const

  {
  return(this->id);
  }

#endif /* PENABLE_LINUX_CGROUPS */
