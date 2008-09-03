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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include "misc.h"
#include "dedtime.h"
#include "globals.h"


/*
 *
 * parse_ded_file - read in dedicated times from file
 *
 *   filename - filename of dedicatd time file
 *
 * returns 0 on success non-zero on failure
 *
 * NOTE: modifies conf data structure
 *
 * FORMAT:      start         finish
 *  MM/DD/YY HH:MM MM/DD/YYYY HH:MM
 */
int parse_ded_file(char *filename)
  {
  FILE *fp;   /* file pointer for the dedtime file */
  char line[256];  /* a buffer for a line from the file */
  int error = 0;  /* boolean: is there an error? */

  struct tm tm_from, tm_to; /* tm structs used to convert to time_t */
  time_t from, to;  /* time_t values for dedtime start - end */
  int i;

  if ((fp = fopen(filename, "r")) == NULL)
    {
    sprintf(line, "Error opening file %s", filename);
    perror(line);
    return 1;
    }

  i = 0;

  memset(conf.ded_time, 0, MAX_DEDTIME_SIZE);

  while (fgets(line, 256, fp) != NULL)
    {
    if (!skip_line(line))
      {
      /* mktime() will figure out if it is dst or not if tm_isdst == -1 */
      memset(&tm_from, 0, sizeof(struct tm));
      tm_from.tm_isdst = -1;

      memset(&tm_to, 0, sizeof(struct tm));
      tm_to.tm_isdst = -1;

      if (sscanf(line, "%d/%d/%d %d:%d %d/%d/%d %d:%d", &tm_from.tm_mon, &tm_from.tm_mday, &tm_from.tm_year, &tm_from.tm_hour, &tm_from.tm_min, &tm_to.tm_mon, &tm_to.tm_mday, &tm_to.tm_year, &tm_to.tm_hour, &tm_to.tm_min) != 10)
        error = 1;
      else
        {
        /* tm_mon starts at 0, where the file will start at 1 */
        tm_from.tm_mon--;

        /* the MM/DD/YY is the wrong date format, but we will accept it anyways */
        /* if year is less then 90, assume year is > 2000 */

        if (tm_from.tm_year < 90)
          tm_from.tm_year += 100;

        /* MM/DD/YYYY is the correct date format */
        if (tm_from.tm_year > 1900)
          tm_from.tm_year -= 1900;

        from = mktime(&tm_from);

        tm_to.tm_mon--;

        if (tm_from.tm_year < 90)
          tm_from.tm_year += 100;

        if (tm_to.tm_year > 1900)
          tm_to.tm_year -= 1900;

        to = mktime(&tm_to);

        /* ignore all dedtime which has passed */
        if (!(from < cstat.current_time && to < cstat.current_time))
          {
          conf.ded_time[i].from = from;
          conf.ded_time[i].to = to;
          i++;
          }
        }

      if (error)
        {
        printf("Error: %s\n", line);
        error = 0;
        }
      }
    }

  /* sort dedtime in ascending order with all 0 elements at the end */
  qsort(conf.ded_time, MAX_DEDTIME_SIZE, sizeof(struct timegap), cmp_ded_time);

  fclose(fp);

  return 0;
  }

/*
 *
 * cmp_ded_time - compare function for qsort for the ded time array
 *
 *   Sort Keys:
 *     - zero elements to the end of the array
 *     - descending by the start time
 *
 */
int cmp_ded_time(const void *v1, const void *v2)
  {
  if (((struct timegap *)v1) -> from == 0 && ((struct timegap *)v2) -> from != 0)
    return 1;
  else if (((struct timegap *)v2) -> from == 0 && ((struct timegap *)v1) -> from != 0)
    return -1;
  else if (((struct timegap *)v1) -> from > ((struct timegap *)v2) -> from)
    return 1;
  else if (((struct timegap *)v1) -> from < ((struct timegap *)v2) -> from)
    return -1;
  else
    return 0;
  }

/*
 *
 * is_ded_time - checks if it is currently dedicated time
 *
 * returns: 1 if it is currently ded time
 *   0 if it is not ded time
 *
 */
int
is_ded_time(void)
  {

  if (cstat.current_time > conf.ded_time[0].from &&
      cstat.current_time < conf.ded_time[0].to)
    return 1;
  else
    return 0;
  }

