#include "license_pbs.h" /* See here for the software license */
/*
 * Basic memory manager
 * calloc/malloc becomes very time consuming when used to allocate very small
 * amounts of memory. There is also the issue of tracking all of the individual allocations.
 * This will replace all of the individual allocations with a single point for all memory reference.
 * A free will free all memory allocated related to this specific memmgr
 */

#ifndef _U_MEMMGR_H
#define _U_MEMMGR_H
#include <string.h>
#define MEMMGR_DEFAULT_SIZE 16384
/* the_mem is the actual lock that individual allocation is taken from.
 * The pattern for taking memory is:
 * int,block
 * the int represents the length of the memory allocated
 * the block is the memory allocated for the individual alloc
 * Possible optimizations based on page size of the individual systems
 * (This would be done in the init function)
 */
typedef struct memmgr
  {
  int alloc_size;           /* Size of mem allocated for this chunck */
  unsigned char *the_mem;   /* The chunk of mem */
  int remaining;            /* Remaining bytes in mem */
  int ref_count;            /* How many refs there are to this chunk */
  unsigned char *current_pos;        /* ptr to the start of free mem */
  /* If the mgr has been extended, this points to the previous mgr */
  struct memmgr *prev_mgr;
  /* If the mgr has been extended, this points to the next mgr */
  struct memmgr *next_mgr;
  /* If the mgr has been extended, this points to the mgr currently in use
   * This is null for all mgr's but the first */
  struct memmgr *current_mgr;
  } memmgr;

void *malloc_track(size_t __size);
void *calloc_track(size_t __nmemb, size_t __size);
void free_track(void *__ptr);
int memmgr_init(memmgr **mgr, int mgr_size);
int memmgr_extend(memmgr **mgr, int size);
void *memmgr_calloc(memmgr **mgr, int qty, int size);
char *memmgr_strdup(memmgr **mgr, char *value, int *size);
char *memmgr_strcat(memmgr **mgr, char *va1, char *val2, int *size);
memmgr *memmgr_find(memmgr **mmgr, void *ptr);
void *memmgr_realloc(memmgr **mgr, void *ptr, int new_size);
int memmgr_free(memmgr **mgr, void *ptr);
void memmgr_destroy(memmgr **mgr);
int memmgr_shuffle(memmgr **mgr, memmgr **new_mgr, int size);
void memmgr_remove(memmgr **mgr);

#endif /* _U_MEMMGR_H */
