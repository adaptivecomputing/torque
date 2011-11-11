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


#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "pbs_error.h"
#include "dynamic_string.h"
#include "log.h" /* for MAXLINE */
#include "utils.h"




/*
 * checks whether or not the dynamic string needs to grow
 * @return number of bytes to grow, or 0 if no need to grow
 */

size_t need_to_grow(

  dynamic_string *ds,
  char           *to_check)

  {
  size_t to_add = strlen(to_check) + 1;
  size_t to_grow = 0;

  if (ds->size < ds->used + to_add)
    {
    to_grow = to_add + ds->size;
    if (to_grow < (ds->size * 4))
      to_grow = ds->size * 4;
    }

  return(to_grow);
  } /* END need_to_grow() */





int size_to_dynamic_string(
    
  dynamic_string    *ds,   /* O */
  struct size_value  szv)  /* I */

  {
  char    buffer[MAXLINE];
  size_t  len;
  size_t  to_grow;

  sprintf(buffer, "%lu", szv.atsv_num);
  len = 2 + strlen(buffer);
  if (ds->used == 0)
    len++;

  if (ds->size - ds->used < len)
    {
    char   *tmp;

    if (len > ds->size * 2)
      to_grow = len;
    else
      to_grow = ds->size * 2;

    if ((tmp = realloc(ds->str, to_grow)) == NULL)
      return(ENOMEM);

    ds->str  = tmp;
    ds->size = to_grow;
    }

  strcat(ds->str, buffer);
  
  switch (szv.atsv_shift)
    {
    case 10:

      strcat(ds->str, "kb");

      break;

    case 20:

      strcat(ds->str, "mb");

      break;

    case 30: 

      strcat(ds->str, "gb");

      break;

    case 40:

      strcat(ds->str, "tb");

      break;

    case 50:

      strcat(ds->str, "pb");

      break;
    }

  ds->used += len;

  return(PBSE_NONE);
  } /* END size_to_dynamic_string() */




/*
 * appends to a string, resizing if necessary
 *
 * @param ds - the string to append to
 * @param to_append - the string being appended
 */
int append_dynamic_string(
    
  dynamic_string *ds, /* M */
  char *to_append)    /* I */

  {
  size_t     to_grow = need_to_grow(ds,to_append);
  char      *tmp;
  int        len = strlen(to_append);

  if (ds->used == 0)
    ds->used += 1;

  if (to_grow > 0)
    {
    tmp = realloc(ds->str,to_grow);

    if (tmp == NULL)
      return(ENOMEM);

    strcat(tmp,to_append);

    ds->str  = tmp;
    ds->size = to_grow;
    }
  else
    {
    /* no need to grow, just append to the string */
    strcat(ds->str,to_append);
    }
    
  ds->used += len;

  return(PBSE_NONE);
  } /* END append_dynamic_string() */





int append_dynamic_string_xml(
    
  dynamic_string *ds,
  char           *str)

  {
  int i;
  int len = strlen(str);
  int empty_at_start = (ds->used == 0);

  if (empty_at_start == FALSE)
    ds->used -= 1;

  for (i = 0; i < len; i++)
    {
    /* Resize if needed
     * note - QUOT_ESCAPED_LEN is the most we could be adding right now */
    if (ds->size - ds->used < QUOT_ESCAPED_LEN)
      {
      char   *tmp;
      size_t  to_grow;

      if (ds->size * 2 < QUOT_ESCAPED_LEN)
        to_grow = QUOT_ESCAPED_LEN * 10;
      else
        to_grow = ds->size * 2;

      if ((tmp = realloc(ds->str, to_grow)) == NULL)
        return(ENOMEM);

      ds->str  = tmp;
      ds->size = to_grow;
      }

    switch (str[i])
      {
      case '<':

        strcat(ds->str, LT_ESCAPED);
        ds->used += LT_ESCAPED_LEN;

        break;

      case '>':

        strcat(ds->str, GT_ESCAPED);
        ds->used += GT_ESCAPED_LEN;

        break;

      case '&':

        strcat(ds->str, AMP_ESCAPED);
        ds->used += AMP_ESCAPED_LEN;

        break;

      case '"':

        strcat(ds->str, QUOT_ESCAPED);
        ds->used += QUOT_ESCAPED_LEN;

        break;

      case '\'':

        strcat(ds->str, APOS_ESCAPED);
        ds->used += APOS_ESCAPED_LEN;

        break;

      default:

        /* copy one character */
        ds->str[ds->used] = str[i];
        ds->used += 1;

        break;
      }
    }

  ds->str[ds->used] = '\0';
  ds->used += 1;

  return(PBSE_NONE);
  } /* END append_dynamic_string_xml() */



/*
 * performs a strcpy at the end of the string (used for node status strings)
 * the difference is this function places strings with their terminating 
 * characters one after another:
 * string_one'\0'string_two'\0'...
 *
 * @param ds - the dynamic string we're copying to
 * @param str - the string being copied
 */
int copy_to_end_of_dynamic_string(

  dynamic_string *ds,
  char           *to_copy)

  {
  size_t  to_grow = need_to_grow(ds,to_copy);
  char   *tmp;
  int     len = strlen(to_copy) + 1;

  if (to_grow > 0)
    {
    tmp = realloc(ds->str,to_grow);
    
    if (tmp == NULL)
      return(ENOMEM);

    strcpy(tmp + ds->used,to_copy);

    ds->str  = tmp;
    ds->size = to_grow;
    }
  else
    {
    /* no need to grow, just append to the string */
    strcpy(ds->str + ds->used,to_copy);
    }
    
  ds->used += len;

  return(PBSE_NONE);
  } /* END copy_to_end_of_dynamic_string() */




/* 
 * initializes a dynamic string and returns it, or NULL if there is no memory 
 *
 * @param initial_size - the initial size of the string, use default if -1
 * @param str - the initial string to place in the dynamic string if not NULL
 * @return - the dynamic string object or NULL if no memory
 */
dynamic_string *get_dynamic_string(
    
  int   initial_size, /* I (-1 means default) */
  char *str)          /* I (optional) */

  {
  dynamic_string *ds = malloc(sizeof(dynamic_string));

  if (ds == NULL)
    return(ds);

  if (initial_size > 0)
    {
    ds->size = initial_size;
    ds->str = malloc(initial_size);
    }
  else
    {
    ds->size = DS_INITIAL_SIZE;
    ds->str = malloc(DS_INITIAL_SIZE);
    }

  if (ds->str == NULL)
    {
    free(ds);
    return(NULL);
    }
    
  /* initialize empty str */
  ds->str[0] = '\0';
  ds->used = 0;

  /* add the string if it exists */
  if (str != NULL)
    {
    if (append_dynamic_string(ds,str) != PBSE_NONE)
      {
      free_dynamic_string(ds);
      return(NULL);
      }
    }

  return(ds);
  } /* END get_dynamic_string() */





/* 
 * frees a dynamic string 
 *
 * @param ds - the dynamic string being freed 
 */
void free_dynamic_string(
    
  dynamic_string *ds) /* M */

  {
  free(ds->str);
  free(ds);
  } /* END free_dynamic_string() */



/*
 * reset the dynamic string to empty, but do not resize
 *
 * @param ds - the dynamic string being cleared
 */
void clear_dynamic_string(

  dynamic_string *ds) /* M */

  {
  ds->used = 0;
  memset(ds->str, 0, ds->size);
  } /* END clear_dynamic_string() */




char *get_string(

  dynamic_string *ds)

  {
  return(ds->str);
  } /* END get_string() */


