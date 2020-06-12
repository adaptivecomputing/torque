#include "pbs_config.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef PENABLE_LINUX_CGROUPS
#include <hwloc.h>
#include <machine.hpp>
#endif

#ifdef NVML_API
#include <hwloc/nvml.h>
#include <nvml.h>
#endif


#ifdef PENABLE_LINUX_CGROUPS
hwloc_bitmap_t hwloc_bitmap_alloc()
  {
  return(0);
  }
#endif


void log_err(

  int         errnum,  /* I (errno or PBSErrno) */
  const char *routine, /* I */
  const char *text)    /* I */

  {
  return;
  }

#ifdef NVML_API
void log_nvml_error(nvmlReturn_t rc, char* gpuid, const char* id)
  {
  return;
  }
#endif

void Machine::store_device_on_appropriate_chip(PCI_Device &device, bool no_info)
  {
  return;
  }

