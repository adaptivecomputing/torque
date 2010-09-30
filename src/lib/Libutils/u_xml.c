
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
#include "utils.h"



/**
 * gets the parent and the child from an xml string 
 * also updates end to be the first character after
 * the parent's closing tag
 * string must have .*?<parent>.*?</parent>.*?
 *
 * @param start - the start of the string to examine
 * @param parent - where to store the parent's name (without '<>')
 * @param parent_size - the max size of parent
 * @param child - where to store the child data, trims whitespace
 * @param child_size - the max size of child
 * @param end - pointer to the first character after the closing tag
 */
int get_parent_and_child(

  char  *start,       /* I */
  char **parent,      /* O */
  char **child,       /* O */
  char **end)         /* O */

  {
  char *ptr = strchr(start,'<');
  char  closingTag[MAXLINE];
  int   parent_len = 0;
  char *child_start;
  char *child_end;

  char *id = "get_parent_and_child";

  if (ptr == NULL)
    {
    log_err(-1,id,"No parent tag found\n");

    return(-1);
    }

  /*advance past the '<' */
  ptr++;
  *parent = ptr;

  /* copy the parent name and create the closing tag */
  sprintf(closingTag,"</");
  while (*ptr != '>')
    {
    closingTag[parent_len+2] = *ptr;
    parent_len++;
    ptr++;
    }

  *ptr = '\0';

  /* NULL terminate the string */
  closingTag[parent_len+2] = '\0';

  /* advance past the '>' */
  ptr++;

  /* mark where the child begins */
  child_start = ptr;

  child_end = strstr(ptr,closingTag);

  if (child_end == NULL)
    {
    log_err(-1,id,"Cannot find closing tag\n");

    return(-1);
    }

  /* move behind the '<' */
  *end = child_end;
  /* NULL terminate the string */
  *child_end = '\0';
  /* move behind it to check for whitespace */
  child_end--;

  /* trim whitespace on start/end of child */
  while (is_whitespace(*child_start))
    child_start++;
  while (is_whitespace(*child_end))
    {
    /* terminate the string earlier */
    *child_end = '\0';
    child_end--;
    }

  *child = child_start;

  /* last, advance the end pointer to the end of the closing tag */
  while (**end != '>')
    (*end)++;

  /* move past the '>' */
  (*end)++;

  return(0);
  } /* END get_parent_and_child() */


