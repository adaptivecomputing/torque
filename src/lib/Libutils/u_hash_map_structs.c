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

/* Adds item to the hashmap indicated by **head
 * If *head is NULL, and new hashmap is created
 *  The existing map will always be checked and any existing reference
 *  will be replace (with the associated memory freed)
 * Also, If a NULL is passed in as a value, it will be set to ""
 * Boolean return value
 */
int hash_add_item(

  memmgr   **mm,                /* M - memory manager */
  job_data **head,            /* M - hashmap */
  char      *name,                 /* key for the item */
  char      *value,                /* value for the item */
  int        var_type,               /* type of variable */
  int        op_type)                /* operation of variable */

  {
  int rc = TRUE;
  int name_len = 0;
  int value_len = 0;
  job_data *je = NULL;
  job_data *item = (job_data *)memmgr_calloc(mm, 1, sizeof(job_data));
  if (item == NULL)
    rc = -1;
  else
    {
    name_len = strlen(name);
    item->name = (char *)memmgr_calloc(mm, 1, name_len+1);
    if (item->name == NULL)
      rc = -2;
    else
      {
      if (value == NULL)
        {
        value_len = 0;
        item->value = (char *)memmgr_calloc(mm, 1, 2);
        if (item == NULL)
          rc = -3;
        else
          item->value[0] = '\0';
        }
      else
        {
        value_len = strlen(value);
        item->value = (char *)memmgr_calloc(mm, 1, value_len+1);
        if (item->value == NULL)
          rc = -4;
        }
      }
    }

  if (rc == TRUE)
    {
    item->var_type = var_type;
    item->op_type = op_type;
    strcpy(item->name, name);
    if (value_len != 0)
      strcpy(item->value, value);
    /* As all memory is allocated in memmgr, and references will be removed
     * when that is freed */
    if (hash_find(*head, item->name, &je) == TRUE)
      hash_del_item(mm, head, item->name);
    HASH_ADD_KEYPTR(hh, *head, item->name, name_len, item);
    }
  else
    {
    if (item)
      {
      if (item->name)
        memmgr_free(mm, item->name);
      if (item->value)
        memmgr_free(mm, item->value);
      memmgr_free(mm, item);
      }
    rc = FALSE;
    }

  return rc;
  } /* END hash_add_item() */




/* A wrapper for hash to accomodate for memory allocation errors
 */
void hash_add_or_exit(

  memmgr   **mm,              /* M - memory manager */
  job_data **head,          /* M - hashmap */
  char      *name,               /* I - The item being added to the hashmap */
  char      *val,                /* I - Sets the value of variable */
  int        var_type)             /* I - Sets the type of the variable */
  {
  if (hash_add_item(mm, head, name, val, var_type, SET) == FALSE)
    {
    fprintf(stderr, "Error allocating memory for hash (%s)-(%s)\n", name, val);
    exit(1);
    }
  }


/* Remove an entry from the hashmap
 * If the first element is removed, the *head is also changed */
int hash_del_item(
    memmgr **mm,              /* M - memory manager */
    job_data **head,          /* M - hashmap */
    char *name)               /* I - entry to delete */
  {
  int rc = FALSE;
  char *tmp_name = NULL;
  job_data *je = NULL;
  hash_find(*head, name, &je);
  if (je != NULL)
    {
    tmp_name = je->name;
    memmgr_free(mm, je->value);
    HASH_DEL(*head, je);
    memmgr_free(mm, tmp_name);
    rc = TRUE;
    if (hash_count(*head) == 0)
      {
      *head = NULL;
      }
    memmgr_free(mm, je);
    }
  return rc;
  }

/* Free's and clears the contents of a hashmap */
int hash_clear(
    memmgr **mm,               /* M - memory manager */
    job_data **head)          /* M - hashmap */
  {
  int cntr = 0;
  char *tmp_name = NULL;
  job_data *en, *tmp;
  HASH_ITER(hh, (*head), en, tmp)
    {
    tmp_name = en->name;
    memmgr_free(mm, en->value);
    HASH_DEL(*head, en);
    memmgr_free(mm, tmp_name);
    memmgr_free(mm, en);
    cntr++;
    }
  HASH_CLEAR(hh, *head);
  *head = NULL;
  return cntr;
  }

/* Finds an element in a hashmap
 * The element returned contains any pointers from the hashmap!!!
 * Boolean return value 1 indicates success, 0 indicates failure
 * DO NOT FREE WHEN FINISHED.
 * Use the hash_clear or hash_del_item function
 */
int hash_find(
    job_data *head,           /* I - hashmap */
    char *name,               /* I - name to find */
    job_data **env_var)       /* O - return value when found */
  {
  int rc = TRUE;
  *env_var = NULL;
  if (name != NULL)
    HASH_FIND(hh, head, name, strlen(name), (*env_var));
  if (*env_var == NULL)
    rc = FALSE;
  return rc;
  }

/* Returns the quantity of elements in the hashmap */
int hash_count(
    job_data *head)           /* I - hashmap */
  {
  return HASH_COUNT(head);
  }

/* Prints all of the contents to the screen (debugging) */
int hash_print(
    job_data *head)           /* I - hashmap */
  {
  int cntr = 0;
  if (head != NULL)
    {
    job_data *en, *tmp;
    HASH_ITER(hh, head, en, tmp)
      {
      printf("%d - [%25s]-{%s}\n", cntr, en->name, en->value);
      cntr++;
      }
    }
  return cntr;
  }

/* Merge the contents of src hash into dest hash
 * If overwrite_existing is set, existing contents will be overwritten
 * This allocates memory for the new hash entries
 * returns qty of items added */
int hash_add_hash(
    memmgr **mm,
    job_data **dest,
    job_data *src,
    int overwrite_existing)
  {
  int cntr = 0;
  job_data *en, *tmp;
  for (en=src; en != NULL; en=en->hh.next)
    {
    if (overwrite_existing)
      {
      hash_add_item(mm, dest, en->name, en->value, en->var_type, en->op_type);
      cntr++;
      }
    else
      if (hash_find(*dest, en->name, &tmp) != TRUE)
        {
        hash_add_item(mm, dest, en->name, en->value, en->var_type, en->op_type);
        cntr++;
        }
    }
  return cntr;
  }

int hash_strlen(
    job_data *src)
  {
  int len = 0;
  job_data *en;
  for (en=src; en != NULL; en=en->hh.next)
    {
    len += strlen(en->name);
    if (en->value != NULL)
      {
      len += strlen(en->value);
      }
    }
  return len;
  }
