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
#include "pbs_error.h"
#include "utils.h"
#include "hash_table.h"


/* PLEASE NOTE:
 *
 * This hash table isn't designed to be an industrial-strength,
 * all-purpose hash table. Right now it is intended only to be used 
 * to hold indexes for resizable arrays 
 */


hash_table_t *create_hash(

  int size)

  {
  hash_table_t *ht = malloc(sizeof(hash_table_t));
  size_t        amount = sizeof(bucket) * size;
  int           i;

  ht->size = size;
  ht->buckets = malloc(amount);

  /* initialize the buckets */
  for (i = 0; i < size; i++)
    ht->buckets[i].value = -1;

  return(ht);
  } /* END create_hash() */





int get_hash(
    
  hash_table_t *ht,
  char         *key)

  {
  /* since key is a job name, just get the numbers from the front */
  return(atoi(key) % ht->size);
  } /* END get_hash() */





void free_buckets(

  bucket *buckets,
  int     size)

  {
  int     i;
  bucket *b;
  bucket *tmp;

  for (i = 0; i < size; i++)
    {
    b = buckets + i;

    while (b != NULL)
      {
      tmp = b;
      b = b->next;

      free(tmp);
      }
    }
  } /* END free_buckets() */





void add_to_bucket(

  bucket *buckets,
  int     index,
  char   *key,
  int     value)

  {
  if (buckets[index].value == -1)
    {
    /* empty bucket, just add it here */
    buckets[index].value = value;
    buckets[index].key   = key;
    buckets[index].next  = NULL;
    }
  else
    {
    /* make this the last bucket at the end */
    bucket *b = buckets + index;

    while (b->next != NULL)
      b = b->next;

    b->next = malloc(sizeof(bucket));
    b->next->value = value;
    b->next->key   = key;
    b->next->next  = NULL;
    }
  } /* END add_to_bucket() */





int add_hash(
    
  hash_table_t *ht,
  int           value,
  char         *key)

  {
  int index;

  /* check if we need to rehash */
  if (ht->size == ht->num)
    {
    size_t   amount = ht->size * 2 * sizeof(bucket);
    bucket  *tmp = malloc(amount);
    int      i;
    int      new_index;

    /* initialize the new buckets */
    for (i = 0; i < ht->size * 2; i++)
      tmp[i].value = -1;

    /* rehash all of the hold values */
    for (i = 0; i < ht->size; i++)
      {
      bucket *b = ht->buckets + i;
      ht->size = ht->size * 2;

      while (b != NULL)
        {
        new_index = get_hash(ht,b->key);

        add_to_bucket(tmp,new_index,b->key,b->value);

        b = b->next;
        }
      } /* END foreach (old buckets) */

    free_buckets(ht->buckets,ht->size / 2);

    ht->buckets = tmp;
    } /* END if (need to rehash) */

  index = get_hash(ht,key);

  add_to_bucket(ht->buckets,index,key,value);

  ht->num++;

  return(PBSE_NONE);
  } /* END add_hash() */





int remove_hash(
    
  hash_table_t *ht,
  char         *key)

  {
  int     rc = PBSE_NONE;
  int     index = get_hash(ht,key);
  bucket *b = ht->buckets + index;
  bucket *prev = NULL;

  while (b != NULL)
    {
    if (!strcmp(key,b->key))
      break;

    prev = b;
    b = b->next;
    }

  if (b == NULL)
    rc = KEY_NOT_FOUND;
  else
    {
    if (prev == NULL)
      {
      if (b->next == NULL)
        {
        /* make this a blank bucket */
        ht->buckets[index].value = -1;
        }
      else
        {
        /* copy the next bucket here and release it */
        ht->buckets[index].value = b->next->value;
        ht->buckets[index].key   = b->next->key;
        ht->buckets[index].next  = b->next->next;

        free(b);
        }
      }
    else
      {
      prev->next = b->next;
      free(b);
      }

    ht->num--;
    } /* END found */

  return(rc);
  } /* END remove_hash() */




/* 
 * find the bucket with key in it and returns the value
 * this is used as a quick lookup to find a job instead of a linear search.
 */
int get_value_hash(
    
  hash_table_t *ht,
  char         *key)

  {
  int     value = -1;
  bucket *b = ht->buckets + get_hash(ht,key);

  while (b != NULL)
    {
    if (!strcmp(b->key,key))
      break;

    b = b->next;
    }

  if (b != NULL)
    value = b->value;

  return(value);
  } /* END get_value_from_hash() */






void change_value_hash(

  hash_table_t *ht,
  char         *key,
  int           new_value)

  {
  int index = get_hash(ht,key);
  bucket *b = ht->buckets + index;

  while (b != NULL)
    {
    if (!strcmp(b->key,key))
      {
      b->value = new_value;
      break;
      }

    b = b->next;
    }
  } /* END change_value_hash() */




