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

#include "attribute.h"
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
  const char     *to_check)

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




int resize_if_needed(

  dynamic_string *ds,
  const char     *to_check)

  {
  size_t  new_size = need_to_grow(ds, to_check);
  size_t  difference;
  char   *tmp;

  if (new_size > 0)
    {
    /* need to resize */
    difference = new_size - ds->size;

    if ((tmp = realloc(ds->str, new_size)) == NULL)
      return(ENOMEM);

    ds->str = tmp;
    /* zero out the new space as well */
    memset(ds->str + ds->size, 0, difference);
    ds->size = new_size;
    }

  return(PBSE_NONE);
  } /* END resize_if_needed() */




int size_to_dynamic_string(
    
  dynamic_string    *ds,   /* O */
  struct size_value *szv)  /* I */

  {
  char buffer[MAXLINE];

  sprintf(buffer, "%lu", szv->atsv_num);
  
  switch (szv->atsv_shift)
    {
    case 10:

      strcat(buffer, "kb");

      break;

    case 20:

      strcat(buffer, "mb");

      break;

    case 30: 

      strcat(buffer, "gb");

      break;

    case 40:

      strcat(buffer, "tb");

      break;

    case 50:

      strcat(buffer, "pb");

      break;
    }

  return(append_dynamic_string(ds, buffer));
  } /* END size_to_dynamic_string() */




/*
 * appends to a string, resizing if necessary
 *
 * @param ds - the string to append to
 * @param to_append - the string being appended
 */
int append_dynamic_string(
    
  dynamic_string *ds,        /* M */
  const char     *to_append) /* I */

  {
  int len = strlen(to_append);
  int add_one = FALSE;
  int offset = ds->used;

  if (ds->used == 0)
    add_one = TRUE;
  else
    offset -= 1;

  resize_if_needed(ds, to_append);
  strcat(ds->str + offset, to_append);
    
  ds->used += len;

  if (add_one == TRUE)
    ds->used += 1;

  return(PBSE_NONE);
  } /* END append_dynamic_string() */





int append_dynamic_string_xml(
    
  dynamic_string *ds,
  const char     *str)

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
    resize_if_needed(ds, QUOT_ESCAPED);

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
  const char     *to_copy)

  {
  int     len = strlen(to_copy) + 1;

  resize_if_needed(ds, to_copy);
    
  strcpy(ds->str + ds->used,to_copy);
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
    
  int         initial_size, /* I (-1 means default) */
  const char *str)          /* I (optional) */

  {
  dynamic_string *ds = calloc(1, sizeof(dynamic_string));

  if (ds == NULL)
    return(ds);

  if (initial_size > 0)
    ds->size = initial_size;
  else
    ds->size = DS_INITIAL_SIZE;
    
  ds->str = calloc(1, ds->size);

  if (ds->str == NULL)
    {
    free(ds);
    return(NULL);
    }
    
  /* initialize empty str */
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




int delete_last_word_from_dynamic_string(

  dynamic_string *ds)

  {
  char *curr;

  if (ds->used <= 0)
    return(-1);

  /* used is always strlen(str) + 1 + 1 */
  curr = ds->str + ds->used - 1;

  if ((curr > ds->str) &&
      (*curr == '\0'))
    {
    curr--;
    ds->used -= 1;
    }

  /* walk backwards across str until you hit a null terminator */
  while ((curr >= ds->str) &&
         (*curr != '\0'))
    {
    *curr = '\0';
    curr--;
    ds->used -= 1;
    }

  return(PBSE_NONE);
  } /* END delete_last_word_from_dynamic_string() */




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




int append_char_to_dynamic_string(
    
  dynamic_string *ds,
  char            c)

  {
  char str[2];
  str[0] = c;
  str[1] = '\0';

  return(append_dynamic_string(ds, str));
  } /* END append_char_to_dynamic_string() */


