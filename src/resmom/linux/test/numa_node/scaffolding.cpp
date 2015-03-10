#include <stdlib.h>
#include <stdio.h> 
  
#include "numa_node.hpp"
#include "mom_memory.h"

int MOMConfigUseSMT;
char cpulist[1024];

proc_mem_t *get_proc_mem_from_path(const char *path)
  {
  proc_mem_t *pm = (proc_mem_t *)calloc(1, sizeof(proc_mem_t));

  pm->mem_total = 1024 * 1024 * 5;

  return(pm);
  }

void get_cpu_list(const char *jobid, char *cpuset_buf, int bufsize)
  {
  snprintf(cpuset_buf, bufsize, "%s", cpulist);
  }
  
bool is_physical_core(unsigned int index)
  {
  return(true);
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

