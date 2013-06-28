#include "license_pbs.h"

/* was in mom_mach.c */
typedef struct proc_mem
  {
  unsigned long long mem_total;
  unsigned long long mem_used;
  unsigned long long mem_free;
  unsigned long long swap_total;
  unsigned long long swap_used;
  unsigned long long swap_free;
  } proc_mem_t;

proc_mem_t *get_proc_mem_from_path(const char *path);
