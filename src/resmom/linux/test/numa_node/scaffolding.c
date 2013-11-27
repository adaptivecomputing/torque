#include <stdlib.h>
#include <stdio.h> 
  
#include "numa_node.hpp"
#include "mom_memory.h"


proc_mem_t *get_proc_mem_from_path(const char *path)
  {
  proc_mem_t *pm = (proc_mem_t *)calloc(1, sizeof(proc_mem_t));

  pm->mem_total = 1024 * 1024 * 5;

  return(pm);
  }

bool is_physical_core(unsigned int os_index)
  {
  return(FALSE);
  }
