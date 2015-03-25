#include <stdlib.h>
#include <stdio.h>

#include "req.hpp"

const char *place_node = "node";
const char *place_socket = "socket";
const char *place_numa = "numachip";
const char *place_core = "core";

const char *use_cores = "usecores";

req::req() {}

unsigned long req::getMemory() const

  {
  return(1024);
  }

int req::getExecutionSlots() const

  {
  return(2);
  }

std::string req::getThreadUsageString() const

  {
  return(use_cores);
  }
