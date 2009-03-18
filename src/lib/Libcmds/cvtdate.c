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
 * cvtdate - convert POSIX touch date/time to seconds since epoch time
 *
 * datestr is a date/time string in the form: [[[[CC]YY]MM]DD]hhmm[.SS]
 * as defined by POSIX.
 *  CC = centry, ie 19 or 20
 *  YY = year, if CC is not provided and YY is < 69, then
 *       CC is assumed to be 20, else 19.
 *  MM = Month, [1,12], if YY is not provided and MM is less than
 *       the current month, YY is next year, else it is the
 *       current year.
 *  DD = Day of month, [1,31], if MM is not provided and DD is less
 *       than the current day, MM is next month, else it is the
 *       next month.
 *  hh = hour, [00, 23], if DD is not provided and hh is less than
 *       the current hour, DD is tomorrow, else it is today.
 *  mm = minute, [00, 59]
 *  SS = seconds, [00, 59]
 *
 * Returns: time_t number of seconds since epoch (Coordinated Univ. Time)
 *   -1 if error.
 */

#include <pbs_config.h>   /* the master config generated by configure */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

time_t
cvtdate(datestr)
char   *datestr;
  {
  char    buf[3];
  time_t    clock;
  int    i;

  struct tm *ptm;
  struct tm  tmpPtm;
  char      *pc;

  struct tm  tm;
  int    year = 0;
  int    month = -1;
  int    day = 0;


  if ((pc = strchr(datestr, (int)'.')))
    {
    *pc++ = '\0';

    if ((strlen(pc) != 2) ||
        (isdigit((int)*pc) == 0) ||
        (isdigit((int)*(pc + 1)) == 0))
      return (-1);

    tm.tm_sec = atoi(pc);

    if (tm.tm_sec > 59)
      return (-1);
    }
  else
    tm.tm_sec = 0;

  for (pc = datestr; *pc; ++pc)
    if (isdigit((int)*pc) == 0)
      return (-1);

  buf[2] = '\0';

  clock = time((time_t *)0);

	ptm   = localtime_r(&clock,&tmpPtm);

  tm.tm_year = ptm->tm_year; /* default year to current */

  tm.tm_mon  = ptm->tm_mon; /* default month to current */

  tm.tm_mday = ptm->tm_mday; /* default day to current */

  switch (strlen(datestr))
    {

    case 12:  /* CCYYMMDDhhmm */
      buf[0] = datestr[0];
      buf[1] = datestr[1];
      year   = atoi(buf) * 100;
      datestr += 2;

      /* no break, fall into next case */

    case 10:  /* YYMMDDhhmm */
      buf[0] = datestr[0];
      buf[1] = datestr[1];
      i      = atoi(buf);

      if (year == 0)
        if (i > 68)
          year = 1900 + i;
        else
          year = 2000 + i;

      else
        year += i;

      tm.tm_year = year - 1900;

      datestr += 2;

      /* no break, fall into next case */

    case 8:  /* MMDDhhmm */
      buf[0] = datestr[0];

      buf[1] = datestr[1];

      i = atoi(buf);

      if (i < 1 || i > 12)
        return (-1);

      if (year == 0)
        if (i <= ptm->tm_mon)
          tm.tm_year++;

      month = i - 1;

      tm.tm_mon = month;

      datestr += 2;

      /* no break, fall into next case */

    case 6:  /* DDhhmm */
      buf[0] = datestr[0];

      buf[1] = datestr[1];

      day = atoi(buf);

      if (day < 1 || day > 31)
        return (-1);

      if (month == -1)
        if (day < ptm->tm_mday)
          tm.tm_mon++;

      tm.tm_mday = day;

      datestr += 2;

      /* no break, fall into next case */

    case 4:  /* hhmm */
      buf[0] = datestr[0];

      buf[1] = datestr[1];

      tm.tm_hour = atoi(buf);

      if (tm.tm_hour > 23)
        return (-1);

      tm.tm_min = atoi(&datestr[2]); /* mm -  minute portion */

      if (tm.tm_min > 59)
        return (-1);

      if (day == 0)   /* day not specified */
        if ((tm.tm_hour < ptm->tm_hour) ||
            ((tm.tm_hour == ptm->tm_hour) &&
             (tm.tm_min <= ptm->tm_min)))
          tm.tm_mday++; /* time for tomorrow */

      break;

    default:
      return (-1);
    }

  tm.tm_isdst = -1;

  return (mktime(&tm));
  }
