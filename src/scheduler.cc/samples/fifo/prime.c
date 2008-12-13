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
#include <string.h>
#include <ctype.h>
#include "pbs_ifl.h"
#include "log.h"
#include "constant.h"
#include "globals.h"
#include "prime.h"
#include "misc.h"


/*
 *
 * is_prime_time - will return the status of primetime
 *
 * returns true if prime time false if non_prime
 *
 * NOTE: Holidays are considered non-prime
 *
 */
enum prime_time
is_prime_time(void)
  {
  /* last_day is used to hold the day during the previous cycle.  It is used so
   * holidays only need to be checked once a day instead of every cycle
   */
  static int last_day = -1;
  enum prime_time ret = PRIME;  /* return code */

  struct tm  *tmptr;   /* current time in a struct tm */

  tmptr = localtime(&(cstat.current_time));

  /* check for holiday: Holiday == non_prime */

  if (conf.holiday_year != 0)  /* year == 0: no prime-time */
    {
    if (tmptr -> tm_year > conf.holiday_year)
      sched_log(PBSEVENT_ADMIN, PBS_EVENTCLASS_FILE, "", "The holday file is out of date, please update it.");
    else if (tmptr -> tm_yday > last_day)
      {
      last_day = tmptr -> tm_yday;
      /* tm_yday starts at 0, and julien date starts at 1 */

      if (is_holiday(tmptr -> tm_yday + 1))
        ret = NON_PRIME;
      }

    /* if ret still equals PRIME then it is not a holidy, we need to check
     * and see if we are in non-prime or prime
     */
    if (ret == PRIME)
      {
      if (tmptr -> tm_wday == 0)
        ret = check_prime(SUNDAY, tmptr);
      else if (tmptr -> tm_wday == 6)
        ret = check_prime(SATURDAY, tmptr);
      else
        ret = check_prime(WEEKDAY, tmptr);
      }
    }

  return ret;
  }

/*
 *
 * check_prime - check if it is prime time for a particular day
 *
 * returns PRIME if it is in primetime or NON_PRIME if not
 */

enum prime_time check_prime(enum days d, struct tm *t)
  {
  enum prime_time prime;  /* return code */

  /* Case 1: all primetime today */

  if (conf.prime[d][PRIME].all)
    prime = PRIME;

  /* Case 2: all nonprime time today */
  else if (conf.prime[d][NON_PRIME].all)
    prime = NON_PRIME;

  /* case 3: no primetime today */
  else if (conf.prime[d][PRIME].none)
    prime = NON_PRIME;

  /* case 4: no nonprime time today */
  else if (conf.prime[d][NON_PRIME].none)
    prime = PRIME;

  /* primetime does not cross the day boundry
   * i.e. 4:00AM - 6:00 PM
   */
  if ((int) conf.prime[d][NON_PRIME].hour - (int) conf.prime[d][PRIME].hour >= 0)
    {
    /* Case 5: primetime doesn't cross day boundry and
     * the current hour is between the start hour of primetime and the start
     * hour of non-primetime
     */
    if (t -> tm_hour > conf.prime[d][PRIME].hour &&
        t -> tm_hour < conf.prime[d][NON_PRIME].hour)
      prime  = PRIME;
    else if (t -> tm_hour == conf.prime[d][PRIME].hour)
      {
      /* prime time is less then 1 hour */
      if (conf.prime[d][PRIME].hour == conf.prime[d][NON_PRIME].hour)
        {
        /* case 6: the current hour is equal to the start of primetime hour
         * and primetime is less then one hour in length.  Current minute is
         * between the start of primetime and the start of nonprimetime
         */
        if (t -> tm_min >= conf.prime[d][PRIME].min &&
            t -> tm_min < conf.prime[d][NON_PRIME].min)
          prime = PRIME;
        else /* Case 7: !case 6 */
          prime = NON_PRIME;
        }

      /* Case 8: The current hour is equal to the start of primetime hour.
       * The current minute is after the start of primetime minute
       */
      else if (t -> tm_min > conf.prime[d][PRIME].min)
        prime = PRIME;
      else /* Case 9: !case 8 */
        prime = NON_PRIME;
      }
    else
      /* case 10: primetime doesn't cross a day boundry, and the current hour
       * is not between the start of primetime and the start of nonprimetime
       */
      prime = NON_PRIME;
    }
  else /* prime time crosses day boundry i.e. 10:00PM - 2:00AM  */
    {
    /* Case 11: primetime crosses a day boundry and the current hour is after
     * the start of primetime i.e. 11:00PM or less start of nonprime time
     * i.e. 1AM
     */
    if (t -> tm_hour > conf.prime[d][PRIME].hour ||
        t -> tm_hour < conf.prime[d][NON_PRIME].hour)
      prime = PRIME;
    else /* Case 12: !case 11 */
      prime = NON_PRIME;
    }

  return prime;
  }

/*
 *
 * is_holiday - returns true if today is a holiday
 *
 *  jdate - amount of days since the beginning of the year
 *   starting with Jan 1 == 1.
 *
 * returns true if today is a holiday, false if not
 *
 */
int is_holiday(int jdate)
  {
  int i;

  for (i = 0; i < conf.num_holidays && conf.holidays[i] != jdate; i++)
    ;

  if (i == conf.num_holidays)
    return 0;

  return 1;
  }

/*
 *
 * parse_holidays - parse the holidays file.  It should be in UNICOS 8
 *    format.
 *
 *   fname - name of holidays file
 *
 * returns success/failure
 *
 */
int parse_holidays(char *fname)
  {
  FILE *fp;  /* file pointer to holidays file */
  char buf[256]; /* buffer to read lines of the file into */
  char *config_name; /* the first word of the line */
  char *tok;  /* used with strtok() to parse the rest of the line */
  char *endp;  /* used with strtol() */
  int num;  /* used to convert string -> integer */
  char error = 0; /* boolean: is there an error ? */
  int linenum = 0; /* the current line number */
  int hol_index = 0; /* index into the holidays array in global var conf */

  if ((fp = fopen(fname, "r")) == NULL)
    {
    perror("Warning: can not open holidays file, assuming 24hr primetime");
    return 0;
    }

  while (fgets(buf, 256, fp) != NULL)
    {
    linenum++;

    if (buf[strlen(buf)-1] == '\n')
      buf[strlen(buf)-1] = '\0';

    if (!skip_line(buf))
      {
      config_name = strtok(buf, " \t");


      /* if there is a 4 digit number, it describes the year and the
       * prime/non-prime times
       *
       *  FORMAT EXAMPLE
       *
       *      YEAR      PRIME START NONPRIME START
       *      1998 0400  1530
       */

      if (strlen(config_name) == 4 && isdigit((int) config_name[0]) &&
          isdigit((int) config_name[1]) && isdigit((int) config_name[2]) &&
          isdigit((int) config_name[3]))
        {
        tok = strtok(NULL, " \t");

        if (load_day(WEEKDAY, PRIME, tok) < 0)
          error = 1;

        if (!error)
          {
          tok = strtok(NULL, " \t");

          if (load_day(WEEKDAY, NON_PRIME, tok) < 0)
            error = 1;
          }
        }

      /* this tells us if we have the correct file format.  Its ignored since
       * lots of error messages will be printed if the file format is wrong
       * and if the file format is correct but just not this, we really
       * shouldn't complain
       */
      if (!strcmp(config_name, "HOLIDAYFILE_VERSION1"))
        ;

      /* the current year - if the file is old, we want to know to log errors
       * later about it
       *
       * FORMAT EXAMPLE
       *
       * YEAR 1998
       */
      else if (!strcmp(config_name, "YEAR"))
        {
        tok = strtok(NULL, " \t");

        if (tok == NULL)
          error = 1;
        else
          {
          num = strtol(tok, &endp, 10);

          if (*endp != '\0')
            error = 1;
          else
            conf.holiday_year = num;
          }
        }

      /* primetime hours for saturday
       * first number is primetime start, second is nonprime start
       *
       * FORMAT EXAMPLE
       *
       *  saturday  0400 1700
       */
      else if (!strcmp(config_name, "saturday"))
        {
        tok = strtok(NULL, " \t");

        if (load_day(SATURDAY, PRIME, tok) < 0)
          error = 1;

        if (!error)
          {
          tok = strtok(NULL, " \t");

          if (load_day(SATURDAY, NON_PRIME, tok) < 0)
            error = 1;
          }
        }

      /* primetime hours for sunday  - same format as saturday */
      else if (!strcmp(config_name, "sunday"))
        {
        tok = strtok(NULL, " \t");

        if (load_day(SUNDAY, PRIME, tok) < 0)
          error = 1;

        if (!error)
          {
          tok = strtok(NULL, " \t");

          if (load_day(SUNDAY, NON_PRIME, tok) < 0)
            error = 1;
          }
        }

      /* primetime for weekday - same format as saturday */
      else if (!strcmp(config_name, "weekday"))
        {
        tok = strtok(NULL, " \t");

        if (load_day(WEEKDAY, PRIME, tok) < 0)
          error = 1;

        if (!error)
          {
          tok = strtok(NULL, " \t");

          if (load_day(WEEKDAY, NON_PRIME, tok) < 0)
            error = 1;
          }
        }

      /*
       * holidays
       * We only care about the julien date of the holiday.  It is enough
       * information to find out if it is a holiday or not
       *
       * FORMAT EXAMPLE
       *
       *  julien date Calendar date holiday name
       *    1  Jan 1  New Year's Day
       */
      else
        {
        num = strtol(config_name, &endp, 10);
        conf.holidays[hol_index] = num;
        hol_index++;
        }

      if (error)
        fprintf(stderr, "Error on line %d, line started with: %s\n", linenum, config_name);

      }

    error = 0;
    }

  conf.num_holidays = hol_index + 1;

  fclose(fp);
  return 0;
  }


/*
 *
 * load_day - fill in the prime time part of the config structure
 *
 *   d  - enum days: can be WEEKDAY, SATURDAY, or SUNDAY
 *   pr - enum prime_time: can be PRIME or NON_PRIME
 *
 * returns 0 on success -1 on error
 *
 */
int load_day(enum days d, enum prime_time pr, char *tok)
  {
  int num;  /* used to convert string -> integer */
  int hours;  /* used to convert 4 digit HHMM into hours */
  int mins;  /* used to convert 4 digit HHMM into minutes */
  char *endp;  /* used wtih strtol() */

  if (tok != NULL)
    {
    if (!strcmp(tok, "all") || !strcmp(tok, "ALL"))
      {
      conf.prime[d][pr].all = TRUE;
      conf.prime[d][pr].hour = UNSPECIFIED;
      conf.prime[d][pr].min = UNSPECIFIED;
      }
    else if (!strcmp(tok, "none") || !strcmp(tok, "NONE"))
      {
      conf.prime[d][pr].none = TRUE;
      conf.prime[d][pr].hour = UNSPECIFIED;
      conf.prime[d][pr].min = UNSPECIFIED;
      }
    else
      {
      num = strtol(tok, &endp, 10);

      if (*endp == '\0')
        {
        /* num is a 4 digit number of the time HHMM */
        mins = num % 100;
        hours = num / 100;
        conf.prime[d][pr].hour = hours;
        conf.prime[d][pr].min = mins;
        }
      else
        return -1;
      }
    }
  else
    return -1;

  return 0;
  }

/*
 *
 * init_prime_time - do any initializations that need to happen at the
 *     start of prime time
 *
 * returns nothing
 *
 */
void
init_prime_time(void)
  {
  cstat.is_prime = 1;
  cstat.round_robin = conf.prime_rr;
  cstat.by_queue = conf.prime_bq;
  cstat.strict_fifo = conf.prime_sf;
  cstat.sort_by = conf.prime_sort;
  cstat.fair_share = conf.prime_fs;
  cstat.load_balancing = conf.prime_lb;
  cstat.help_starving_jobs = conf.prime_hsv;
  cstat.sort_queues = conf.prime_sq;
  cstat.load_balancing_rr = conf.prime_lbrr;
  }

/*
 *
 * init_non_prime_time - do any initializations that need to happen at
 *         the beginning of non prime time
 *
 * returns nothing
 *
 */
void
init_non_prime_time(void)
  {
  cstat.is_prime = 0;
  cstat.round_robin = conf.non_prime_rr;
  cstat.by_queue = conf.non_prime_bq;
  cstat.strict_fifo = conf.non_prime_sf;
  cstat.sort_by = conf.non_prime_sort;
  cstat.fair_share = conf.non_prime_fs;
  cstat.load_balancing = conf.non_prime_lb;
  cstat.help_starving_jobs = conf.non_prime_hsv;
  cstat.sort_queues = conf.non_prime_sq;
  cstat.load_balancing_rr = conf.non_prime_lbrr;
  }
