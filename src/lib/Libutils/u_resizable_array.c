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



int swap_things(

  resizable_array *ra,
  void            *thing1,
  void            *thing2)

  {
  int index1 = -1;
  int index2 = -1;
  int i;

  for (i = 0; i < ra->max; i++)
    {
    if (thing1 == ra->slots[i])
      index1 = i;
    
    if (thing2 == ra->slots[i])
      index2 = i;

    if ((index1 != -1) &&
        (index2 != -1))
      break;
    }

  if ((index1 == -1) ||
      (index2 == -1))
    return(THING_NOT_FOUND);

  ra->slots[index1] = thing2;
  ra->slots[index2] = thing1;

  return(PBSE_NONE);
  } /* END swap_things() */




int insert_thing(
    
  resizable_array *ra,
  void             *thing)

  {
  static char   *id = "insert_thing";
  void         **tmp;
  
  /* check if the array must be resized */
  if (ra->max == ra->num)
    {
    long remaining;
    long size = ra->max * 2 * sizeof(void *);

    tmp = realloc(ra->slots,size);

    if (tmp == NULL)
      {
      log_err(ENOMEM,id,"There is no memory left!! System failure\n");
      return(ENOMEM);
      }

    if (ra->next_slot == ra->max)
      ra->next_slot++;

    remaining = ra->max * sizeof(void *);
    
    memset(tmp + ra->max,0,remaining);

    tmp[ra->next_slot] = thing;

    ra->slots = tmp;

    ra->max *= 2;
    }
  else
    {
    ra->slots[ra->next_slot] = thing;
    }

  ra->num++;

  /* now update the next_slot pointer */
  while ((ra->next_slot < ra->max) && 
         (ra->slots[ra->next_slot] != NULL))
    ra->next_slot++;

  return(PBSE_NONE);
  } /* END insert_thing() */





int is_present(

  resizable_array *ra,
  void            *thing)

  {
  int i;

  for (i = 0; i < ra->max; i++)
    if (ra->slots[i] == thing)
      return(TRUE);

  return(FALSE);
  } /* END is_present() */






/* 
 * remove a thing from the array
 *
 * @param thing - the thing to remove
 * @return PBSE_NONE if the thing is removed 
 */

int  remove_thing(
    
  resizable_array *ra,
  void            *thing)

  {
  int i;

  /* find the thing */
  for (i = 0; i < ra->max; i++)
    {
    if (ra->slots[i] == thing)
      break;
    }

  if (i == ra->max)
    return(THING_NOT_FOUND);

  ra->slots[i] = NULL;

  /* reset the next_slot index if necessary */
  if (i < ra->next_slot)
    {
    ra->next_slot = i;
    }

  return(PBSE_NONE);
  } /* END remove_thing() */



/*
 * mallocs and returns a resizable array with initial size size
 *
 * @param size - the initial size of the resizable array
 * @return a pointer to the resizable array 
 */
resizable_array *initialize_resizable_array(

  int               size)

  {
  resizable_array *ra = malloc(sizeof(resizable_array));
  long             amount = sizeof(void *) * size;

  ra->max = size;
  ra->num = 0;
  ra->next_slot = 0;

  ra->slots = malloc(amount);
  memset(ra->slots,0,amount);

  return(ra);
  } /* END initialize_resizable_array() */



/*
 * returns the next available item and increments *iter
 */
void *next_thing(

  resizable_array *ra,
  int             *iter)

  {
  void *thing = NULL;
  int   i = *iter;

  /* advance to the next occupied slot */
  while ((i < ra->max) &&
         (ra->slots[i] == NULL))
    i++;

  /* make the iter one past i and assign the thing */
  if (i < ra->max)
    {
    thing = ra->slots[i];
    *iter = i + 1;
    }
  else
    *iter = i;

  return(thing);
  } /* END next_thing() */




