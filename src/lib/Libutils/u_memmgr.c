/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
*
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
*
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
*
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
*
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
*
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
*
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
*
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
*
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
*
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
*
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
*
* 7. DISCLAIMER OF WARRANTY
*
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
*
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/





#include <u_memmgr.h>
#include <pbs_error.h>
#include <stdlib.h>
#include <string.h>

/* The following three functions are for unit testing purposes
 * The are overridden in the test code to generate full coverage of all
 * variations of the functions.
 * Please do not touch */
void *malloc_track(size_t __size)
  {
  return malloc(__size);
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
  int rc = TRUE;
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
      mm->current_pos = (char *)mm->the_mem;
      mm->prev_mgr = NULL;
      mm->next_mgr = NULL;
      mm->current_mgr = mm;
      *mgr = mm;
      }
    }
  if (rc != TRUE)
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
  int rc = TRUE;
  memmgr *new_mgr = NULL;
  if ((rc = memmgr_init(&new_mgr, size)) == TRUE)
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
  int rc = FALSE;
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
      rc = TRUE;
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
        if ((rc = memmgr_extend(mgr, 0)) == TRUE)
          {
          mm = (*mgr)->current_mgr;
          }
        }
      }
    }
  if (rc == TRUE)
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
  memmgr *new_mgr = NULL;
  if ((new_size > 0) && (mm == NULL))
    {
    if ((mgr != NULL) && (*mgr != NULL))
      {
      res_mem = memmgr_calloc(mgr, 1, new_size);
      }
    }
  else if (new_size > 0)
    {
    memcpy(&size, ptr - sizeof(int), sizeof(int));
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
        if ((mm->the_mem = realloc(mm->the_mem, new_alloc)) != NULL)
          {
          if (mm->the_mem != NULL)
            {
            res_mem = mm->the_mem + sizeof(int);
            mm->alloc_size =  new_alloc;
            mm->remaining = 0;
            mm->current_pos = NULL;
            mm->the_mem[new_alloc] = '\0';
            memcpy(mm->the_mem, &new_size, sizeof(int));
            }
          }
        }
      /* new_size > existing, less than total and only one ref */
      else
        {
        memset(mm->the_mem + sizeof(int) + size, 0, new_size - size);
        mm->the_mem[new_size] = '\0';
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
  int rc = FALSE;
  int size = 0;
  char *size_ptr = NULL;
  memmgr *mm = memmgr_find(mgr, ptr);
  memmgr *new_head = NULL;
  if (mm != NULL)
    {
    size_ptr = ptr - sizeof(int);
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
    rc = TRUE;
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
  if (rc == TRUE)
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
