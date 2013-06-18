#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <hwloc.h>

#include "mom_memory.h"
#include "resource.h"
#include "pbs_nodes.h"
#include "pbs_job.h"
#include "pbs_config.h"

int LOGLEVEL;

char log_buffer[4096];
#ifdef NUMA_SUPPORT
int       num_node_boards;
nodeboard node_boards[MAX_NODE_BOARDS]; 
int       numa_index;
#endif

#ifdef PENABLE_LINUX26_CPUSETS
int              memory_pressure_threshold = 0; /* 0: off, >0: check and kill */
short            memory_pressure_duration  = 0; /* 0: off, >0: check and kill */
int              MOMConfigUseSMT           = 1; /* 0: off, 1: on */
hwloc_topology_t topology;
#endif

bool no_memory;

void log_err(int l, const char *func_name, const char *msg) {}
void log_ext(int a, const char *b, const char *c, int d) {}
void log_event(int a, int b, const char *c, const char *d) {}

job *mom_find_job(char *jobid) 
  {
  return(NULL);
  }

resource_def *svr_resc_def;
int           svr_resc_size = 0;

proc_mem_t *get_proc_mem_from_path(const char *path)
  {
  proc_mem_t *mem = (proc_mem_t *)calloc(1, sizeof(proc_mem_t));
  mem->mem_total = ((long long )15 * 1024) * 1024 * 1024;
  return(mem);
  }

resource *find_resc_entry(

  pbs_attribute    *pattr,  /* I */
  resource_def *rscdf)  /* I */

  {
  static resource mem;

  mem.rs_value.at_val.at_size.atsv_num = 16;
  mem.rs_value.at_val.at_size.atsv_shift = 30;

  if (no_memory == true)
    return(NULL);
  else
    return(&mem);
  }

resource_def *find_resc_def(resource_def *svr_resc, char const * name, int max_index)
  {
  return(NULL);
  }
