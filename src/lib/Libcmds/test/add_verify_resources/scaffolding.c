#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "u_memmgr.h" /* memmgr */
#include "u_hash_map_structs.h" /* job_data */

void *memmgr_calloc(memmgr **mgr, int qty, int size)
  {
  fprintf(stderr, "The call to memmgr_calloc needs to be mocked!!\n");
  exit(1);
  }

void hash_add_or_exit(memmgr **mm, job_data **head, char *name, char *value, int var_type)
  {
  fprintf(stderr, "The call to hash_add_or_exit needs to be mocked!!\n");
  exit(1);
  }
