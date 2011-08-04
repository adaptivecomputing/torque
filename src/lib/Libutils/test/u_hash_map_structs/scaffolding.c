#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include "pbs_ifl.h"
#include "u_memmgr.h"

int calloc_fail;

int memmgr_init(memmgr **mgr, int mgr_size)
  {
  return TRUE;
  }
void *memmgr_calloc(memmgr **mgr, int qty, int size)
  {
  if (calloc_fail)
    return NULL;
  else
    return calloc(qty, size);
  }
int memmgr_free(memmgr **mgr, void *ptr)
  {
  free(ptr);
  }

void memmgr_destroy(memmgr **mgr)
  {
  }

