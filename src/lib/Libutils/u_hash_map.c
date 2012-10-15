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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hash_map.h"
#include "pbs_error.h"
#include "log.h"



/* get_hash_map
 *
 * returns an initialized, empty hash map
 *
 * @param size - the initial size of the hash map or -1 for default
 * @returns an initialized hash_map
 */

hash_map *get_hash_map(

  int size_param)

  {
  hash_map *hm = calloc(1, sizeof(hash_map));
  int       size;

  if (size_param == -1)
    size = INITIAL_HASH_MAP_SIZE;
  else
    size = size_param;

  hm->hm_ra = initialize_resizable_array(size);
  hm->hm_ht = create_hash(size);

  hm->hm_mutex = calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(hm->hm_mutex, NULL);

  return(hm);
  } /* END get_hash_map() */




/*
 * free_hash_map
 * frees a hash map
 *
 * @param hm - the hash map to be freed
 */

void free_hash_map(

  hash_map *hm)

  {
  pthread_mutex_t *tmp = hm->hm_mutex;

  pthread_mutex_lock(tmp);

  free_hash(hm->hm_ht);
  free_resizable_array(hm->hm_ra);
  free(hm);
 
  pthread_mutex_unlock(tmp);
  free(tmp);
  } /* END free_hash_map() */




/*
 * add_to_hash_map
 * adds an object with a specified key to the hash map
 *
 * @param hm - the hash_map to add to
 * @param obj - the object to the inserted into the hash map
 * @param key - the key to hash with
 * @returns PBSE_NONE on success or ENOMEM on failure
 */

int add_to_hash_map(
    
  hash_map *hm,
  void     *obj,
  char     *key)

  {
  int index;
  int rc = PBSE_NONE;

  pthread_mutex_lock(hm->hm_mutex);

  if (get_value_hash(hm->hm_ht, key) >= 0)
    {
    rc = ALREADY_IN_HASH_MAP;
    }
  else
    {
    if ((index = insert_thing(hm->hm_ra, obj)) == -1)
      {
      rc = ENOMEM;
      log_err(rc, __func__, "Memory failure");
      }
    else
      add_hash(hm->hm_ht, index, key);
    }

  pthread_mutex_unlock(hm->hm_mutex);

  return(rc);
  } /* END add_to_hash_map() */




/*
 * remove_from_hash_map
 * removes the object that corresponds to key from the hash map
 *
 * @param hm - the hash map to remove from
 * @param key - the key of the object to be removed
 * @return PBSE_NONE on success or KEY_NOT_FOUND on error
 */

int remove_from_hash_map(

  hash_map *hm,
  char     *key)

  {
  int index;
  int rc = PBSE_NONE;

  pthread_mutex_lock(hm->hm_mutex);

  if ((index = get_value_hash(hm->hm_ht, key)) < 0)
    rc = KEY_NOT_FOUND;
  else
    {
    remove_thing_from_index(hm->hm_ra, index);
    remove_hash(hm->hm_ht, key);
    }

  pthread_mutex_unlock(hm->hm_mutex);

  return(rc);
  } /* END remove_from_hash_map() */




void *get_remove_from_hash_map(

  hash_map *hm,
  char     *key)

  {
  void *obj = NULL;
  int   index;

  pthread_mutex_lock(hm->hm_mutex);

  if ((index = get_value_hash(hm->hm_ht, key)) >= 0)
    {
    obj = hm->hm_ra->slots[index].item;
    remove_thing_from_index(hm->hm_ra, index);
    remove_hash(hm->hm_ht, key);
    }

  pthread_mutex_unlock(hm->hm_mutex);

  return(obj);
  } /* END get_remove_from_hash_map() */




/*
 * get_from_hash_map
 * retrieves the object that corresponds to key from the hash map
 *
 * @param hm - the hash map to retrieve from
 * @param key - the key of the object to retrieve
 * @return the object if there is an object with that key in the hash map or
 * NULL otherwise
 */

void *get_from_hash_map(

  hash_map *hm,
  char     *key)

  {
  int   index;
  void *obj = NULL;

  pthread_mutex_lock(hm->hm_mutex);

  if ((index = get_value_hash(hm->hm_ht, key)) >= 0)
    obj = hm->hm_ra->slots[index].item;

  pthread_mutex_unlock(hm->hm_mutex);

  return(obj);
  } /* END get_from_hash_map() */




/*
 * next_from_hash_map
 * iterates to the next object in the hash map
 *
 * @param hm - the hash map being iterated
 * @param iter - a pointer to where we are in the hash map
 * @return the next obj in the hash map, or NULL if we are done iterating
 */

void *next_from_hash_map(

  hash_map *hm,
  int      *iter)

  {
  void *obj;

  pthread_mutex_lock(hm->hm_mutex);

  obj = next_thing(hm->hm_ra, iter);

  pthread_mutex_unlock(hm->hm_mutex);

  return(obj);
  } /* END next_from_hash_map() */




