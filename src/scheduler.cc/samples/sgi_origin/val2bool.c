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
/* $Id$ */

/*
 * This file uses strftime(3)/strptime(3), which are part of XPG4,
 * but are not specified in the POSIX 1003.1 standard.
 */
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif /* _POSIX_C_SOURCE */

#include <sys/param.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "toolkit.h"

/*
 * Convert a boolean value (i.e. "True" or "False") to an int, where
 * the integer 1 represents a true value, and 0 represents non-true.
 */
int
schd_val2bool(char *val, int *bool)
  {
  /* char   *id = "val2bool"; */
  char   lower[64];

  /* Make a copy of the input string, and convert it to all lowercase. */
  strncpy(lower, val, sizeof(lower) - 1);
  schd_lowercase(lower);

  /* Test for a variety of "yes" values. */

  if ((strcmp(lower, "true") == 0) ||
      (strcmp(lower, "on") == 0) ||
      (strcmp(lower, "enabled") == 0) ||
      (strcmp(lower, "enable") == 0) ||
      (strcmp(lower, "yes") == 0) ||
      (strcmp(lower, "y") == 0) ||
      (strcmp(lower, "1") == 0))
    {
    *bool = 1;
    return (0);
    }

  /* Test for a variety of "no" values. */
  if ((strcmp(lower, "false") == 0) ||
      (strcmp(lower, "off") == 0) ||
      (strcmp(lower, "no") == 0) ||
      (strcmp(lower, "disabled") == 0) ||
      (strcmp(lower, "disable") == 0) ||
      (strcmp(lower, "n") == 0) ||
      (strcmp(lower, "0") == 0))
    {
    *bool = 0;
    return (0);
    }

  /* No free() necessary - lower is automatic array. */

  return (1);  /* Did not parse successfully. */
  }

char *
schd_bool2val(int bool)
  {
  if (bool != 0)
    return "True";

  return "False";
  }


#define DATEFMT "%m/%d/%Y@%H:%M:%S" /* MM/DD/YYYY@HH:MM:SS */

/*
 * Convert a string of the form MM/DD/YYYY@HH:MM:SS to a time_t.  Uses
 * strptime(3) function for parsing.
 */
int
schd_val2datetime(char *string, time_t *when)
  {
  /* char   *id = "get_datetime"; */
  char   *remain;

  struct tm tm;
  time_t  then;

  /* Call the "inverse" of strftime(3) to parse the string. */
  remain = strptime(string, DATEFMT, &tm);

  /* Check for successful parsing of the date string in DATEFMT. */

  if (remain == NULL)
    return -1;

  /* Make sure the date@time was the only thing in the string. */
  if (*remain != '\0' && !isspace(*remain))
    return -1;

  /*
  * The struct tm now contains the right information.  Turn it into a
  * valid time_t.
  */
  then = mktime(&tm);

  if (then == (time_t) - 1)
    return -1;

  /* Copy the time value and return success. */
  *when = then;

  return 0;
  }

/*
 * Change a string into either a boolean (values 0 or 1), or a time_t if
 * the string gives a time in the format "MM/DD/YYYY@HH:MM:SS".
 */
int
schd_val2booltime(char *val, time_t *t)
  {
  int    ival;
  time_t tval;

  /* Try to parse into a boolean first. */

  if (schd_val2bool(val, &ival) == 0)
    {
    *t = (time_t) ival;
    return 0;
    }

  /* Not a boolean -- try for a date@time string. */
  if (schd_val2datetime(val, &tval) == 0)
    {
    *t = tval;
    return 0;
    }

  /* Not a valid boolean or date@time, so reject it. */
  return 1;
  }

/*
 * Change an integer value into string containing either "True" or "False",
 * or (if the value > 1) a string containing the equivalent 'date@time' for
 * that value.
 * The time_t is overloaded in the case of being a boolean.
 */
char *
schd_booltime2val(time_t bool)
  {
  static char val[64];
  char   datetime[48];

  if ((bool == (time_t) 0) || (bool == (time_t) 1))
    return (schd_bool2val(bool));

  (void)strftime(datetime, sizeof(datetime) - 1, DATEFMT, localtime(&bool));

  if (time(NULL) >= bool)
    (void)sprintf(val, "%s (since %s)", schd_bool2val(1), datetime);
  else
    (void)sprintf(val, "%s (until %s)", schd_bool2val(0), datetime);

  return val;
  }
