#include <stdlib.h>
#include <stdio.h>

#ifdef PENABLE_LINUX_CGROUPS
#include <hwloc.h>
#include <machine.hpp>
#endif

#ifdef NVML_API
#include <hwloc/nvml.h>
#include <nvml.h>
#endif


#ifdef PENABLE_LINUX_CGROUPS
hwloc_bitmap_t hwloc_bitmap_alloc()
  {
  return(0);
  }
#endif


void log_err(

  int         errnum,  /* I (errno or PBSErrno) */
  const char *routine, /* I */
  const char *text)    /* I */

  {
  return;
  }
