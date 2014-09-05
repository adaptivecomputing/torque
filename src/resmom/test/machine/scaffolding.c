#include <stdlib.h>
#include <stdio.h>
#include "machine.hpp"
#include "log.h"
#include "pbs_error.h"

int hardware_style;

void log_err(int errnum, const char *routine, const char *text)
  {
  }

Socket::Socket()
  {
  }

Socket::~Socket()
  {
  }

int Socket::initializeAMDSocket(hwloc_obj_t socket_obj, hwloc_topology_t topology)
  {
  return(PBSE_NONE);
  }

int Socket::initializeNonNUMASocket(hwloc_obj_t obj, hwloc_topology_t topology)
  {
  return(PBSE_NONE);
  }

int Socket::initializeIntelSocket(hwloc_obj_t obj, hwloc_topology_t topology)
  {
  return(PBSE_NONE);
  }

Core::~Core()
  {
  }

Chip::~Chip()
  {
  }

PCI_Device::~PCI_Device()
  {
  }


