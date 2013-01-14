#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <hwloc.h>

#include "pbs_nodes.h"
#include "pbs_job.h"
#include "pbs_config.h"

int LOGLEVEL;

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

void log_err(int l, const char *func_name, const char *msg) {}
void log_ext(int a, const char *b, const char *c, int d) {}
void log_event(int a, int b, const char *c, const char *d) {}

job *mom_find_job(char *jobid) 
  {
  return(NULL);
  }
