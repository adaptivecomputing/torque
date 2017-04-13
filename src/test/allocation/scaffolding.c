#include <stdlib.h>
#include <stdio.h>

#include "req.hpp"
#include "array.h"

const char *place_node = "node";
const char *place_socket = "socket";
const char *place_numa_node = "numanode";
const char *place_core = "core";
const char *place_thread = "thread";
const char *place_legacy = "legacy";
const char *place_legacy2 = "legacy2";

const char *use_cores = "usecores";

int         tc = 1;

std::string req::getPlacementType() const
  {
  return("");
  }

req::req() {}

unsigned long long req::get_memory_per_task() const

  {
  return(1024);
  }

int req::getExecutionSlots() const

  {
  return(2);
  }

int req::getTaskCount() const
  {
  return(tc);
  }

std::string req::getThreadUsageString() const

  {
  return(use_cores);
  }

int req::getMics() const

  {
  return(0);
  }

int req::get_gpus() const

  {
  return(0);
  }

int req::getPlaceCores() const
  {
  return(this->cores);
  }

int req::getPlaceThreads() const
  {
  return(this->threads);
  }

int is_whitespace(

  char c)

  {
  if ((c == ' ')  ||
      (c == '\n') ||
      (c == '\t') ||
      (c == '\r') ||
      (c == '\f'))
    return(TRUE);
  else
    return(FALSE);
  } /* END is_whitespace */
