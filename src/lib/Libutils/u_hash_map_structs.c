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
 * Structure based hashmap wrapper
 */
#include <pbs_config.h>
#include <u_hash_map_structs.h>
#include <stdlib.h>
#include <stdio.h>
#include <pbs_ifl.h>
#include <pbs_job.h>

/* Adds item to the hashmap indicated by **head
 * If *head is NULL, and new hashmap is created
 *  The existing map will always be checked and any existing reference
 *  will be replace (with the associated memory freed)
 * Also, If a NULL is passed in as a value, it will be set to ""
 * Boolean return value
 */
int hash_add_item(
  job_data_container *head,            /* M - hashmap */
  const char        *name,                 /* key for the item */
  const char        *value,                /* value for the item */
  int                var_type,               /* type of variable */
  int                op_type)                /* operation of variable */

  {
  job_data *item = new job_data(name,value,var_type,op_type);

  head->lock();
  int ret =  head->insert(item,name,true);
  head->unlock();
  return ret;
  } /* END hash_add_item() */



void hash_priority_add_or_exit(

  job_data_container *head,
  const char         *name,
  const char         *value,
  int                 var_type)

  {
  bool      should_add = true;
  job_data *old_item = head->find(name);

  if (old_item != NULL)
    {
    // Only call insert if we have priority over the old item
    if (var_type >= old_item->var_type)
      should_add = false;
    }

  if (should_add)
    {
    if (hash_add_item(head, name, value, var_type, SET) == FALSE)
      {
      fprintf(stderr, "Error allocating memory for hash (%s)-(%s)\n", name, value);
      exit(1);
      }
    }
  } // END hash_priority_add_or_exit()



/* A wrapper for hash to accomodate for memory allocation errors
 */
void hash_add_or_exit(
  job_data_container *head,          /* M - hashmap */
  const char        *name,               /* I - The item being added to the hashmap */
  const char        *val,                /* I - Sets the value of variable */
  int                var_type)             /* I - Sets the type of the variable */

  {
  if (hash_add_item(head, name, val, var_type, SET) == FALSE)
    {
    fprintf(stderr, "Error allocating memory for hash (%s)-(%s)\n", name, val);
    exit(1);
    }
  }


/* Remove an entry from the hashmap
 * If the first element is removed, the *head is also changed */
int hash_del_item(
    job_data_container *head,          /* M - hashmap */
    const char        *name)               /* I - entry to delete */
  {
  head->lock();
  job_data *item = head->find(name);
  head->unlock();
  if(item != NULL)
    {
    head->lock();
    head->remove(name);
    head->unlock();
    delete item;
    return TRUE;
    }
  return FALSE;
  }

/* Free's and clears the contents of a hashmap */
int hash_clear(
    job_data_container *head)          /* M - hashmap */
  {
  head->lock();
  job_data_iterator *it = head->get_iterator();
  job_data *item;

  while ((item = it->get_next_item()) != NULL)
    {
    delete item;
    }

  delete it;

  head->clear();
  head->unlock();
  return TRUE;
  }

/* Finds an element in a hashmap
 * The element returned contains any pointers from the hashmap!!!
 * Boolean return value 1 indicates success, 0 indicates failure
 * DO NOT FREE WHEN FINISHED.
 * Use the hash_clear or hash_del_item function
 */
int hash_find(
    
  job_data_container *head,           /* I - hashmap */
  const char        *name,               /* I - name to find */
  job_data           **env_var)       /* O - return value when found */

  {
  int rc = TRUE;
  *env_var = NULL;
  if (name != NULL)
    {
    head->lock();
    *env_var = head->find(name);
    head->unlock();
    }
  if (*env_var == NULL)
    rc = FALSE;
  return rc;
  } /* END hash_find() */



/* Returns the quantity of elements in the hashmap */
int hash_count(
    job_data_container *head)           /* I - hashmap */
  {
  head->lock();
  int ret = head->count();
  head->unlock();
  return ret;
  }

/* Prints all of the contents to the screen (debugging) */
int hash_print(
   
  job_data_container *head)           /* I - hashmap */

  {
  int cntr = 0;
  if (head != NULL)
    {
    head->lock();
    job_data *en;
    job_data_iterator *it = head->get_iterator();
    while((en = it->get_next_item()) != NULL)
      {
      printf("%d - [%25s]-{%s}\n", cntr, en->name.c_str(), en->value.c_str());
      cntr++;
      }
    head->unlock();

    delete it;
    }
  return cntr;
  }

/* Merge the contents of src hash into dest hash
 * If overwrite_existing is set, existing contents will be overwritten
 * This allocates memory for the new hash entries
 * returns qty of items added */
int hash_add_hash(

  job_data_container *dest,
  job_data_container *src,
  int                 overwrite_existing)

  {
  int cntr = 0;
  job_data *en;
  job_data *tmp;

  src->lock();
  job_data_iterator *it = src->get_iterator();

  while ((en = it->get_next_item()) != NULL)
    {
    if (overwrite_existing)
      {
      dest->lock();
      dest->insert(new job_data(en->name.c_str(),en->value.c_str(), en->var_type, en->op_type),en->name.c_str(),true);
      dest->unlock();
      cntr++;
      }
    else if (hash_find(dest, en->name.c_str(), &tmp) != TRUE)
      {
      hash_add_item(dest, en->name.c_str(), en->value.c_str(), en->var_type, en->op_type);
      cntr++;
      }
    }

  delete it;

  src->unlock();
  dest->unlock();

  return cntr;
  }

int hash_strlen(

  job_data_container *src)

  {
  int len = 0;
  job_data *en;
  src->lock();
  job_data_iterator *it = src->get_iterator();
  for (en=it->get_next_item(); en != NULL; en=it->get_next_item())
    {
    len += en->name.length();
    len += en->value.length();
    }

  delete it;
  src->unlock();

  return len;
  }
