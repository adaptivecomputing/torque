#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include "machine.hpp"
#include "log.h"
#include "pbs_error.h"

int hardware_style;

void log_err(int errnum, const char *routine, const char *text)
  {
  }

#include "../../numa_pci_device.cpp"
#include "../../numa_socket.cpp"
#include "../../numa_chip.cpp"
#include "../../numa_core.cpp"
