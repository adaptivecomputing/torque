#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <hwloc.h>

#include "mom_memory.h"
#include "resource.h"
#include "pbs_nodes.h"
#include "pbs_job.h"
#include "pbs_config.h"
#include "node_internals.hpp"
#include "allocation.hpp"

int LOGLEVEL;

char log_buffer[4096];
#ifdef NUMA_SUPPORT
int       num_node_boards;
nodeboard node_boards[MAX_NODE_BOARDS]; 
int       numa_index;
#endif

int              memory_pressure_threshold = 0; /* 0: off, >0: check and kill */
short            memory_pressure_duration  = 0; /* 0: off, >0: check and kill */
int              MOMConfigUseSMT           = 1; /* 0: off, 1: on */
hwloc_topology_t topology;
int              read_cpuset_rc;
const char      *cpus_template_string = "0-5";
const char      *mems_template_string = "0";

char             global_cpus_string[MAXPATHLEN];
char             global_mems_string[MAXPATHLEN];

int hwloc_bitmap_parselist(const char *buf, hwloc_bitmap_t map);
int hwloc_bitmap_displaylist(char *buf, size_t buflen, hwloc_bitmap_t map);
node_internals   internal_layout;

bool no_memory;

extern bool check_event;
extern void event_data(const char *d);

void log_err(int l, const char *func_name, const char *msg) {}
void log_ext(int a, const char *b, const char *c, int d) {}
void log_event(int a, int b, const char *c, const char *d)
  {
  if(check_event) event_data(d);
  }

job *mom_find_job(const char *jobid) 
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

#ifdef PENABLE_LINUX26_CPUSETS

#if 0
int init_cpusets(void)
  {
  return 0;
  }

int read_cpuset(
  const char     *name,  /* I */
  hwloc_bitmap_t  cpus,  /* O */
  hwloc_bitmap_t  mems)  /* O */

  {
  /* if we have a root cpuset, return template cpus and mems */
  if (strcmp(name, "/dev/cpuset") == 0 || strcmp(name, "/") == 0)
    {
    if (cpus != NULL)
      /* parse cpus_template_string */
      hwloc_bitmap_parselist(cpus_template_string, cpus);

    if (mems != NULL)
      /* parse mems_template_string */
      hwloc_bitmap_parselist(mems_template_string, mems);
    }
  else
    {
    /* Zero bitmaps */
    if (cpus != NULL)
      hwloc_bitmap_zero(cpus);

    if (mems != NULL)
      hwloc_bitmap_zero(mems);
    }

  return read_cpuset_rc;
  }

int create_cpuset(

  const char           *name,  /* I */
  const hwloc_bitmap_t  cpus,  /* I */
  const hwloc_bitmap_t  mems,  /* I */
  mode_t                flags) /* I */

  {
  /* make sure cpus and mems match templates for torque cpuset */
  /*  return PBSE_NONE if match, -1 otherwise */
  if (strcmp(name, "/dev/cpuset/torque") == 0 || strcmp(name, "/torque") == 0)
    {
    /* check for cpus to be what previously set */
    if (cpus != NULL)
      {
      hwloc_bitmap_displaylist(global_cpus_string, sizeof(global_cpus_string), cpus);
      if (strcmp(global_cpus_string, cpus_template_string))
        return(-1);
      }
    else
      return(-1);

    /* check for mems to be what previously set */
    if (mems != NULL)
      {
      hwloc_bitmap_displaylist(global_mems_string, sizeof(global_mems_string), mems);
      if (strcmp(global_mems_string, mems_template_string))
        return(-1);
      }
    else
      return(-1);

    return(PBSE_NONE);
    }

  /* return for non-torque cpuset */
  return(-1);
  }

void remove_logical_processor_if_requested(
  hwloc_bitmap_t *cpus)

  {
  return;
  }
#endif

int rmdir(const char *path) throw ()
  {
  return 0;
  }

void log_record(int eventtype, int objclass, const char *objname, const char *text)
  {
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
  }


#endif

allocation::allocation() {}
allocation::allocation(const allocation &other) {}

void translate_range_string_to_vector(

  const char       *range_string,
  std::vector<int> &indices)

  {
  }
