#include "license_pbs.h" /* See here for the software license */
#include "u_memmgr.h"
#include "pbs_error.h"
#include <stdlib.h>
#include <string.h>

/* The following three functions are for unit testing purposes
 * The are overridden in the test code to generate full coverage of all
 * variations of the functions.
 * Please do not touch */
void *malloc_track(size_t __size)
  {
  return calloc(1, __size);
  }

void *calloc_track(size_t __nmemb, size_t __size)
  {
  return calloc(__nmemb, __size);
  }

void free_track(void *__ptr)
  {
  free(__ptr);
  }

int memmgr_init(memmgr **mgr, int mgr_size)
  {
  int rc = PBSE_NONE;
  memmgr *mm = NULL;
  if (mgr == NULL)
    rc = -3;
  else if ((mm = (memmgr *)malloc_track(sizeof(memmgr))) == NULL)
    rc = -1;
  else
    {
    if (mgr_size <= 0)
      mm->alloc_size = MEMMGR_DEFAULT_SIZE;
    else
      mm->alloc_size = mgr_size;
    mm->the_mem = calloc_track(1, mm->alloc_size);
    if (mm->the_mem == NULL)
      rc = -2;
    else
      {
      mm->remaining = mm->alloc_size;
      mm->ref_count = 0;
      mm->current_pos = mm->the_mem;
      mm->prev_mgr = NULL;
      mm->next_mgr = NULL;
      mm->current_mgr = mm;
      *mgr = mm;
      }
    }
  if (rc != PBSE_NONE)
    {
    if (mm)
      {
      if (mm->the_mem) free_track(mm->the_mem);
      free_track(mm);
      }
    }
  return rc;
  }

int memmgr_extend(memmgr **mgr, int size)
  {
  int rc = PBSE_NONE;
  memmgr *new_mgr = NULL;
  if ((rc = memmgr_init(&new_mgr, size)) == PBSE_NONE)
    {
    (*mgr)->current_mgr->next_mgr = new_mgr;
    new_mgr->prev_mgr = (*mgr)->current_mgr;
    (*mgr)->current_mgr = new_mgr;
    new_mgr->current_mgr = NULL;
    }
  return rc;
  }

void *memmgr_calloc(memmgr **mgr, int qty, int size)
  {
  int rc = PBSE_MEM_MALLOC;
  int qps = qty * size;
  void *new_mem = NULL;
  memmgr *mm = NULL;
  /* sizeof(int) == size of requested memory
   * size == memory requested
   * 1 == null ptr
   */
  int real_size = sizeof(int) + (qty * size) + 1;
  if ((mgr != NULL) && (*mgr != NULL) && (qps > 0))
    {
    mm = (*mgr)->current_mgr;
    if (mm->remaining > real_size)
      {
      rc = PBSE_NONE;
      }
    else
      {
      if (qty*size > MEMMGR_DEFAULT_SIZE)
        {
        mm = NULL;
        rc = memmgr_shuffle(mgr, &mm, qty*size);
        }
      else
        {
        if ((rc = memmgr_extend(mgr, 0)) == PBSE_NONE)
          {
          mm = (*mgr)->current_mgr;
          }
        }
      }
    }
  if (rc == PBSE_NONE)
    {
    memcpy(mm->current_pos, &size, sizeof(int));
    mm->current_pos += sizeof(int);
    new_mem = mm->current_pos;
    mm->current_pos[size] = '\0';
    mm->current_pos += size + 1;
    mm->remaining -= real_size;
    mm->ref_count++;
    }
  return new_mem;
  }




char *memmgr_strdup(
    
  memmgr **mgr,
  char    *value,
  int     *size)

  {
  char *rv = NULL;
  int   t_size = 0;

  if (value == NULL)
    t_size = 0;
  else
    t_size = strlen(value) + 1;

  if (t_size <= 0)
    {
    }
  else if ((rv = (char *)memmgr_calloc(mgr, 1, t_size)) != NULL)
    {
    strcpy(rv, value);

    if (size != NULL)
      *size = t_size;
    }

  return(rv);
  } /* END memmgr_strdup() */




char *memmgr_strcat(memmgr **mgr, char *val1, char *val2, int *size)
  {
  int t_size = 0;
  char *rv = NULL;
  if (val2 == NULL)
    return val1;
  else if (val1 == NULL)
    return val2;
  else
    {
    t_size = strlen(val1) + strlen(val2) + 1;
    if ((rv = (char *)memmgr_calloc(mgr, 1, t_size)) != NULL)
      {
      rv = (char *)memmgr_calloc(mgr, 1, t_size);
      strcpy(rv, val1);
      strcat(rv, val2);
      }
    }
  return rv;
  }

/* finds which memmgr a ptr belongs to, returns NULL if not valid */
memmgr *memmgr_find(memmgr **mmgr, void *ptr)
  {
  memmgr *mm = NULL;
  if ((mmgr == NULL) || (*mmgr == NULL) || (ptr == NULL))
    {
    mm = NULL;
    }
  else
    {
    mm = *mmgr;
    while (mm)
      {
      if (((unsigned char *)ptr > mm->the_mem) &&
          ((unsigned char *)ptr < (mm->the_mem + mm->alloc_size)))
          {
          break;
          }
      mm = mm->next_mgr;
      }
    }
  return mm;
  }

/*
 * Returns null if the pointer is not in an allocated range
 * or insufficient memory is available.
 * undefined
 */
void *memmgr_realloc(memmgr **mgr, void *ptr, int new_size)
  {
  void *res_mem = NULL;
  int new_alloc = sizeof(int) + new_size + 1;
  int size = 0;
  memmgr *mm = memmgr_find(mgr, ptr);
  void *tmp = NULL;
  if ((new_size > 0) && (mm == NULL))
    {
    if ((mgr != NULL) && (*mgr != NULL))
      {
      res_mem = memmgr_calloc(mgr, 1, new_size);
      }
    }
  else if (new_size > 0)
    {
    memcpy(&size, ((int *)ptr) - 1, sizeof(int));
    /* Requested realloc is smaller than the current alloc */
    if (new_size < size)
      {
      memcpy(((unsigned char *)ptr) - sizeof(int), &new_size, sizeof(int));
      res_mem = (unsigned char *)ptr;
      ((unsigned char *)ptr)[new_size] = '\0';
      /* As we will move requests for larger allocation,
       * there is no point in clearing the remaining memory */
/*      memset(ptr + new_size, 0, size - new_size); */
      /* Only one reference in block, make unused mem available */
      if (mm->ref_count == 1)
        {
        mm->current_pos = mm->the_mem + new_alloc;
        mm->remaining = mm->alloc_size - new_alloc;
        memset(mm->the_mem + new_alloc, 0, size - new_size);
        memcpy(mm->the_mem, &new_size, sizeof(int));
        }
      }
    /* Only one reference in this memory chunk. */
    else if (mm->ref_count == 1)
      {
      /* Larger than all the mem in the chuck */
      if (new_size > mm->alloc_size)
        {
        /* as it's the only thing, realloc the current block */
        if ((tmp = realloc(mm->the_mem, new_alloc)) != NULL)
          {
          mm->the_mem = tmp;
          memset(mm->the_mem + mm->alloc_size, 0, new_alloc - mm->alloc_size);
          res_mem = mm->the_mem + sizeof(int);
          mm->alloc_size =  new_alloc;
          mm->remaining = 0;
          mm->current_pos = NULL;
          memcpy(mm->the_mem, &new_size, sizeof(int));
          }
        else
          {
          free(mm->the_mem);
          mm->the_mem = NULL;
          }
        }
      /* new_size > existing, less than total and only one ref */
      else
        {
        memset(mm->the_mem + sizeof(int) + size, 0, new_size - size);
        mm->the_mem[new_size - 1] = '\0';
        mm->remaining = mm->alloc_size - new_alloc;
        mm->current_pos = mm->the_mem + new_alloc;
        res_mem = mm->the_mem + sizeof(int);
        memcpy(mm->the_mem, &new_size, sizeof(int));
        }
      }
    else
      {
      res_mem = memmgr_calloc(mgr, 1, new_size);
      memcpy(res_mem, ptr, size);
      /* Return value is not checked as the ptr was validated in
       * memmgr_find above */
      memmgr_free(&mm, ptr);
      }
    }
  return res_mem;
  }

int memmgr_free(memmgr **mgr, void *ptr)
  {
  int rc = PBSE_MEM_MALLOC;
  int size = 0;
  char *size_ptr = NULL;
  memmgr *mm = memmgr_find(mgr, ptr);
  memmgr *new_head = NULL;
  if (mm != NULL)
    {
    size_ptr = (char *)ptr - sizeof(int);
    memcpy(&size, size_ptr, sizeof(int));
    memset(size_ptr, 0, size+sizeof(int));
    mm->ref_count--;
    if (mm->ref_count == 0)
      {
      mm->current_pos = mm->the_mem;
      if (mm->current_mgr == NULL)
        {
        if (((*mgr)->current_mgr != *mgr) && ((*mgr)->current_mgr != mm))
          {
          memmgr_remove(&mm);
          }
        }
      else
        {
        if ((*mgr)->next_mgr != NULL)
          {
          new_head = (*mgr)->next_mgr;
          new_head->current_mgr = (*mgr)->current_mgr;
          }
        memmgr_remove(&mm);
        *mgr = new_head;
        }
      }
    rc = PBSE_NONE;
    }
  return rc;
  }

/* free the memmgr chain */
void memmgr_destroy(memmgr **mgr)
  {
  memmgr *mm = *mgr;
  memmgr *next_mgr = NULL; 
  while (mm != NULL)
    {
    next_mgr = mm->next_mgr;
    free_track(mm->the_mem);
    mm->the_mem = NULL;
    free_track(mm);
    mm = NULL;
    mm = next_mgr;
    }
  *mgr = NULL;
  }

int memmgr_shuffle(memmgr **mgr, memmgr **new_mgr, int size)
  {
  int rc = memmgr_init(new_mgr, size + sizeof(int) + 1);
  memmgr *mm = NULL;
  if (rc == PBSE_NONE)
    {
    mm = (*mgr)->current_mgr;
    (*new_mgr)->next_mgr = mm;
    if (mm->prev_mgr == NULL)
      {
      mm->prev_mgr = (*new_mgr);
      /* The head node got replaced with new_mgr, move the current_mgr ptr
       *  to point to the old head node */
      (*new_mgr)->current_mgr = mm;
      mm->current_mgr = NULL;
      *mgr = *new_mgr;
      }
    else
      {
      (*new_mgr)->prev_mgr = mm->prev_mgr;
      mm->prev_mgr->next_mgr = (*new_mgr);
      mm->prev_mgr = (*new_mgr);
      (*new_mgr)->current_mgr = NULL;
      }
    }
  return rc;
  }

/* removes an individual memmgr from a chain */
void memmgr_remove(memmgr **mgr)
  {
  if ((*mgr)->prev_mgr != NULL)
    (*mgr)->prev_mgr->next_mgr = (*mgr)->next_mgr;
  if ((*mgr)->next_mgr != NULL)
    (*mgr)->next_mgr->prev_mgr = (*mgr)->prev_mgr;
  free_track((*mgr)->the_mem);
  free_track(*mgr);
  *mgr = NULL;
  }
