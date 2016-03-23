#ifndef __MOM_MEMORY_H__
#define __MOM_MEMORY_H__

#include "license_pbs.h"

#include "pbs_job.h"

/* was in mom_mach.c */
 typedef struct proc_mem
  {
  unsigned long long mem_total;
  unsigned long long mem_used;
  unsigned long long mem_free;
  unsigned long long swap_total;
  unsigned long long swap_used;
  unsigned long long swap_free;
  }proc_mem_t;

proc_mem_t *get_proc_mem_from_path(const char *path);
long long get_memory_requested_in_kb(job &pjob);
int       get_cpu_count_requested_on_this_node(job &pjob);

#endif /* __MOM_MEMORY_H__ */
