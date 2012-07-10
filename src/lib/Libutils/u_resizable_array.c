/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2010 Veridian Information Solutions, Inc.
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


#include <errno.h>
#include <signal.h>
#include <time.h>
#include "utils.h"
#include "resizable_array.h"
#include "pbs_error.h"
#include "../Liblog/pbs_log.h"


int swap_things(

  resizable_array *ra,
  void            *thing1,
  void            *thing2)

  {
  int index1 = -1;
  int index2 = -1;
  int i = ra->slots[ALWAYS_EMPTY_INDEX].next;

  while (i != 0)
    {
    if (thing1 == ra->slots[i].item)
      index1 = i;
    
    if (thing2 == ra->slots[i].item)
      index2 = i;

    if ((index1 != -1) &&
        (index2 != -1))
      break;

    i = ra->slots[i].next;
    }

  if ((index1 == -1) ||
      (index2 == -1))
    return(THING_NOT_FOUND);

  ra->slots[index1].item = thing2;
  ra->slots[index2].item = thing1;

  return(PBSE_NONE);
  } /* END swap_things() */




/*
 * checks if the array needs to be resized, and resizes if necessary
 *
 * @return PBSE_NONE or ENOMEM
 */
int check_and_resize(

  resizable_array *ra)

  {
  static char *id = "check_and_resize";
  slot        *tmp;
  size_t       remaining;
  size_t       size;

  if (ra->max == ra->num + 1)
    {
    /* double the size if we're out of space */
    size = (ra->max * 2) * sizeof(slot);

    if ((tmp = realloc(ra->slots,size)) == NULL)
      {
      log_err(ENOMEM,id,"No memory left to resize the array");
      return(ENOMEM);
      }

    remaining = ra->max * sizeof(slot);

    memset(tmp + ra->max, 0, remaining);

    ra->slots = tmp;

    ra->max = ra->max * 2;
    }

  return(PBSE_NONE);
  } /* END check_and_resize() */




/* 
 * updates the next slot pointer if needed \
 */
void update_next_slot(

  resizable_array *ra) /* M */

  {
  while ((ra->next_slot < ra->max) &&
         (ra->slots[ra->next_slot].item != NULL))
    ra->next_slot++;
  } /* END update_next_slot() */




/*
 * inserts an item, resizing the array if necessary
 *
 * @return the index in the array or -1 on failure
 */
int insert_thing(
    
  resizable_array *ra,
  void             *thing)

  {
  int rc;
  
  /* check if the array must be resized */
  if ((rc = check_and_resize(ra)) != PBSE_NONE)
    {
    return(-1);
    }
    
  ra->slots[ra->next_slot].item = thing;
 
  /* save the insertion point */
  rc = ra->next_slot;

  /* handle the backwards pointer, next pointer is left at zero */
  ra->slots[rc].prev = ra->last;

  /* make sure the empty slot points to the next occupied slot */
  if (ra->last == ALWAYS_EMPTY_INDEX)
    {
    ra->slots[ALWAYS_EMPTY_INDEX].next = rc;
    }

  /* update the last index */
  ra->slots[ra->last].next = rc;
  ra->last = rc;

  /* update the new item's next index */
  ra->slots[rc].next = ALWAYS_EMPTY_INDEX;

  /* increase the count */
  ra->num++;

  update_next_slot(ra);

  return(rc);
  } /* END insert_thing() */





/* 
 * inserts a thing after the thing in index
 * NOTE: index must represent a valid index
 */
int insert_thing_after(

  resizable_array *ra,
  void            *thing,
  int              index)

  {
  int rc;
  int next;
  
  /* check if the array must be resized */
  if ((rc = check_and_resize(ra)) != PBSE_NONE)
    {
    return(-1);
    }

  /* insert this element */
  ra->slots[ra->next_slot].item = thing;
 
  /* save the insertion point */
  rc = ra->next_slot;

  /* move pointers around */
  ra->slots[rc].prev = index;
  next = ra->slots[index].next;
  ra->slots[rc].next = next;
  ra->slots[index].next = rc;

  if (next != 0)
    {
    ra->slots[next].prev = rc;
    }

  /* update the last index if needed */
  if (ra->last == index)
    ra->last = rc;

  /* increase the count */
  ra->num++;

  update_next_slot(ra);

  return(rc);
  } /* END insert_thing_after() */





/* 
 * inserts a thing befire the thing in index
 * NOTE: index must represent a valid index
 */
int insert_thing_before(

  resizable_array *ra,
  void            *thing,
  int              index)

  {
  int rc;
  int prev;
  
  /* check if the array must be resized */
  if ((rc = check_and_resize(ra)) != PBSE_NONE)
    {
    return(-1);
    }

  /* insert this element */
  ra->slots[ra->next_slot].item = thing;
 
  /* save the insertion point */
  rc = ra->next_slot;

  /* move pointers around */
  prev = ra->slots[index].prev;
  ra->slots[rc].next = index;
  ra->slots[rc].prev = prev;
  ra->slots[index].prev = rc;
  ra->slots[prev].next = rc;

  /* increase the count */
  ra->num++;

  update_next_slot(ra);

  return(rc);
  } /* END insert_thing_before() */





int is_present(

  resizable_array *ra,
  void            *thing)

  {
  int i = ra->slots[ALWAYS_EMPTY_INDEX].next;

  while (i != 0)
    {
    if (ra->slots[i].item == thing)
      return(TRUE);

    i = ra->slots[i].next;
    }

  return(FALSE);
  } /* END is_present() */





/*
 * fix the next pointer for the box pointing to this index 
 *
 * @param ra - the array we're fixing
 * @param index - index of the slot we're unlinking
 */
void unlink_slot(

  resizable_array *ra,
  int              index)

  {
  int prev = ra->slots[index].prev;
  int next = ra->slots[index].next;
    
  ra->slots[index].prev = ALWAYS_EMPTY_INDEX;
  ra->slots[index].next = ALWAYS_EMPTY_INDEX;
  ra->slots[index].item = NULL;

  ra->slots[prev].next = next;

  /* update last if necessary, otherwise update prev's next index */
  if (ra->last == index)
    ra->last = prev;
  else 
    ra->slots[next].prev = prev;
  } /* END unlink_slot() */





/* 
 * remove a thing from the array
 *
 * @param thing - the thing to remove
 * @return PBSE_NONE if the thing is removed 
 */

int remove_thing(
    
  resizable_array *ra,
  void            *thing)

  {
  int i = ra->slots[ALWAYS_EMPTY_INDEX].next;
  int found = FALSE;

  /* find the thing */
  while (i != ALWAYS_EMPTY_INDEX)
    {
    if (ra->slots[i].item == thing)
      {
      found = TRUE;
      break;
      }

    i = ra->slots[i].next;
    }

  if (found == FALSE)
    return(THING_NOT_FOUND);

  unlink_slot(ra,i);

  ra->num--;

  /* reset the next_slot index if necessary */
  if (i < ra->next_slot)
    {
    ra->next_slot = i;
    }

  return(PBSE_NONE);
  } /* END remove_thing() */




void *remove_thing_memcmp(

  resizable_array *ra,
  void            *thing,
  unsigned int     size)

  {
  int   i = ra->slots[ALWAYS_EMPTY_INDEX].next;
  void *item = NULL;

  while (i != ALWAYS_EMPTY_INDEX)
    {
    /* check if equal */
    if (!memcmp(ra->slots[i].item, thing, size))
      {
      item = ra->slots[i].item;

      unlink_slot(ra, i);

      ra->num--;

      if (i < ra->next_slot)
        ra->next_slot = i;

      break;
      }

    i = ra->slots[i].next;
    }

  return(item);
  } /* END remove_thing_memcmp() */




/*
 * pop the first thing from the array
 *
 * @return the first thing in the array or NULL if empty
 */

void *pop_thing(

  resizable_array *ra)

  {
  void *thing = NULL;
  int   i = ra->slots[ALWAYS_EMPTY_INDEX].next;

  if (i != ALWAYS_EMPTY_INDEX)
    {
    /* get the thing we're returning */
    thing = ra->slots[i].item;

    /* handle the deletion and removal */
    unlink_slot(ra,i);

    ra->num--;

    /* reset the next slot index if necessary */
    if (i < ra->next_slot)
      {
      ra->next_slot = i;
      }
    }

  return(thing);
  } /* END pop_thing() */




int remove_thing_from_index(

  resizable_array *ra,
  int              index)

  {
  int rc = PBSE_NONE;

  if (ra->slots[index].item == NULL)
    rc = THING_NOT_FOUND;
  else
    {
    /* FOUND */
    unlink_slot(ra,index);

    ra->num--;

    if (index < ra->next_slot)
      ra->next_slot = index;
    }

  return(rc);
  } /* END remove_thing_from_index() */




int remove_last_thing(

  resizable_array *ra)

  {
  return(remove_thing_from_index(ra, ra->last));
  } /* END remove_last_thing() */




/*
 * callocs and returns a resizable array with initial size size
 *
 * @param size - the initial size of the resizable array
 * @return a pointer to the resizable array 
 */
resizable_array *initialize_resizable_array(

  int               size)

  {
  resizable_array *ra = calloc(1, sizeof(resizable_array));
  size_t           amount = sizeof(slot) * size;

  ra->max       = size;
  ra->num       = 0;
  ra->next_slot = 1;
  ra->last      = 0;

  ra->slots = calloc(1, amount);

  return(ra);
  } /* END initialize_resizable_array() */




/*
 * returns the next available item and increments *iter
 */
void *next_thing(

  resizable_array *ra,
  int             *iter)

  {
  void *thing;
  int   i = *iter;

  if (i == -1)
    {
    /* initialize first */
    i = ra->slots[ALWAYS_EMPTY_INDEX].next;
    }

  thing = ra->slots[i].item;
  *iter = ra->slots[i].next;

  return(thing);
  } /* END next_thing() */




/* 
 * returns the next available item from the back and decrements *iter
 */
void *next_thing_from_back(

  resizable_array *ra,
  int             *iter)

  {
  void *thing;
  int   i = *iter;

  if (i == -1)
    {
    /* initialize first */
    i = ra->last;
    }

  thing = ra->slots[i].item;
  *iter = ra->slots[i].prev;

  return(thing);
  } /* END next_thing_from_back() */




/* 
 * initialize the iterator for this array 
 */
void initialize_ra_iterator(

  resizable_array *ra,
  int             *iter)

  {
  *iter = ra->slots[ALWAYS_EMPTY_INDEX].next;
  } /* END initialize_ra_iterator() */




/*
 * searches the array for thing, finding the index
 *
 * @param ra - the array to be searched
 * @param thing - the thing we're looking for
 * @return index if present, THING_NOT_FOUND otherwise
 */
int get_index(

  resizable_array *ra,
  void            *thing)

  {
  int i = ra->slots[ALWAYS_EMPTY_INDEX].next;

  for (i = ra->slots[ALWAYS_EMPTY_INDEX].next; i != ALWAYS_EMPTY_INDEX; i = ra->slots[i].next)
    {
    if (ra->slots[i].item == thing)
      break;
    }

  if (i != ALWAYS_EMPTY_INDEX)
    return(i);
  else
    return(THING_NOT_FOUND);
  } /* END get_index() */



void free_resizable_array(

  resizable_array *ra)

  {
  free(ra->slots);
  free(ra);
  } /* END resizable_array() */



void *get_thing_from_index(

  resizable_array *ra,
  int              index)

  {
  if (index == -1)
    index = ra->slots[ALWAYS_EMPTY_INDEX].next;

  if (index >= ra->max)
    return(NULL);
  else
    return(ra->slots[index].item);
  } /* END get_thing_from_index() */




