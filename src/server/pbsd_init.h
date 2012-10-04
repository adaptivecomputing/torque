#ifndef _PBSD_INIT_H
#define _PBSD_INIT_H
#include "license_pbs.h" /* See here for the software license */

/*
 * dynamic array, with utility functions for easy appending
 */

typedef struct darray_t
  {
  int Length;
  void **Data;
  int AppendIndex;
  } darray_t;

int DArrayInit(darray_t *Array, int InitialSize);

int DArrayFree(darray_t *Array);

int DArrayAppend(darray_t *Array, void *Item);

/* static int SortPrioAscend(const void *A, const void *B); */

void update_default_np();

void add_server_names_to_acl_hosts(void);

int pbsd_init(int type);

int recov_svr_attr(int type);

#endif /* _PBSD_INIT_H */
