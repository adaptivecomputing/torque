#include <stdlib.h>
#include <stdio.h> 
  
#include "numa_node.hpp"
#include "mom_memory.h"


proc_mem_t *get_proc_mem_from_path(const char *path)
  {
  static proc_mem_t pm;

  memset(&pm, 0, sizeof(pm));
  pm.mem_total = 1024 * 1024 * 5;

  return(&pm);
  }

