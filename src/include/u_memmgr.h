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
  char *current_pos;        /* ptr to the start of free mem */
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
memmgr *memmgr_find(memmgr **mmgr, void *ptr);
void *memmgr_realloc(memmgr **mgr, void *ptr, int new_size);
int memmgr_free(memmgr **mgr, void *ptr);
void memmgr_destroy(memmgr **mgr);
int memmgr_shuffle(memmgr **mgr, memmgr **new_mgr, int size);
void memmgr_remove(memmgr **mgr);

#endif /* _U_MEMMGR_H */
