#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "u_memmgr.h" /* memmgr */
#include "u_hash_map_structs.h" /* job_data */

void *memmgr_calloc(memmgr **mgr, int qty, int size)
  {
  return(calloc(qty, size));
  }


void hash_add_or_exit(memmgr **mm, job_data **head, const char *name, const char *value, int var_type)
  {
  }
