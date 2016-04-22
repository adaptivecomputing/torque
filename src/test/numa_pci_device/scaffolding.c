#include <pbs_config.h>
#include <stdlib.h>
#include <stdio.h>
#include <hwloc.h>

#include "machine.hpp"

hwloc_bitmap_t hwloc_bitmap_alloc()
  {
  return(0);
  }

#ifdef NVML_API
void PCI_Device::initializeGpu(

  int    idx,
  hwloc_topology_t topology)

  {
  return;
  }

#endif
