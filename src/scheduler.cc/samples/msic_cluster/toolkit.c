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
 *
 * This file contains a variety of routines that should not need to be
 * modified to work with a variety of schedulers. This is an attempt
 * at encapulating common code into a single file for the various
 * tasks that a scheduler may need to perform. Major tasks, or those
 * that may be scheduler or site specific are left in separate files.
 *
 * Code for this toolkit has been written and/or contributed by several
 * people including:
 *
 *     Dr. Ed Hook, Nick Cardo, Karl Schilke, and James Patton Jones.
 *
 * Toolkit assembled and maintained by:
 *
 *     James Patton Jones, jjones@pbspro.com,  Veridian Systems
 *
 *
 * This "toolkit" provides the following routines:
 *
 * --------------------------------------------------------------------------
 * int schd_how_many(char *str, char *state)
 *
 *      Given a state_count string, return the integer value of the
 *      state requested.
 *
 * --------------------------------------------------------------------------
 * char * schd_byte2val(size_t bytes)
 *
 *      Return a pointer to a static string that is the shortest
 *      string by which the number of bytes can be accurately
 *      represented. i.e.:
 *
 *      1023  -> 1023b
 *      16384  -> 16kb
 *      1048576  -> 1mb
 *      16777216  -> 16mb
 *      16777217  -> 16777217b
 *      34359738368 -> 32gb
 *
 * --------------------------------------------------------------------------
 * char *schd_sec2val(int seconds)
 *
 *      Convert seconds into "HH:MM:SS" format. Note that the returned
 *      string is a pointer to a static buffer. The caller must copy the
 *      time string into a holding buffer before the next call to
 *      sec2val() occurs.
 *
 * --------------------------------------------------------------------------
 * size_t schd_val2byte(char *val)
 *
 *      Convert an allocation value string to its equivalent value in bytes./
 *
 * --------------------------------------------------------------------------
 * int schd_val2bool(char *val, int *bool)
 *
 *      Convert a boolean value (i.e. "True" or "False") to an int, where
 *      the integer 1 represents a true value, and 0 represents non-true.
 *
 * --------------------------------------------------------------------------
 * char * schd_bool2val(int bool)
 *
 *      Convert a boolean value (i.e. an integer of 1 or 0) to  a char string
 *      of "True" or "False".
 *
 * --------------------------------------------------------------------------
 * time_t schd_val2sec(char *val)
 *
 *      Convert a value string from time to its equivalent value in seconds.
 *
 * --------------------------------------------------------------------------
 * int schd_val2datetime(char *string, time_t *when)
 *
 *      Convert a string of the form MM/DD/YYYY@HH:MM:SS to a time_t.  Uses
 *      strptime(3) function for parsing.
 *
 * --------------------------------------------------------------------------
 * int schd_val2booltime(char *val, time_t *t)
 *
 *      Change a string into either a boolean (values 0 or 1), or a time_t
 *      if the string gives a time in the format "MM/DD/YYYY@HH:MM:SS".
 *
 * --------------------------------------------------------------------------
 * int schd_register_file(char *filename)
 * int schd_file_has_changed(char *filename, int reset_stamp)
 *
 *      Routines to check if files have been changed, deleted, created,
 *      etc. since the last invocation. Used to automatically load new
 *      configuration and statistics data.
 *
 * --------------------------------------------------------------------------
 * int schd_forget_file(char *filename)
 *
 *      "Forget" about the file named by 'filename', or all files if
 *      'filename' is a NULL pointer. Returns the number of files removed
 *      from the watch list, or -1 if the file is not found (or the list
 *      was empty).
 *
 * --------------------------------------------------------------------------
 * char * schd_getat(char *at, Batch_Status *bs, char *rs)
 *
 *      Given a batch_status structure (bs), search the attributes for
 *      the requested attribute (at) and return it's value (v). If the
 *      attribute contains a resource list, then search the resource list
 *      for the requested resource (rs). The function returns the value
 *      if found or a NULL if the search is not successful.
 *
 * --------------------------------------------------------------------------
 * void schd_timestamp(char *msg)
 *
 * --------------------------------------------------------------------------
 * int schd_free_qlist(QueueList *qlist)
 *
 *      Free a set of QueueList structures that reference (via qptr->queue)
 *      an existing set of Queue structs. The Queues themselves are not
 *      affected.
 *
 * --------------------------------------------------------------------------
 * int schd_destroy_qlist(QueueList *list)
 *
 *      This function differs from the schd_free_qlist() function in that
 *      it actually frees the Queue's themselves, while schd_free_qlist()
 *      simply destroys the list structures that point to the Queue's.
 *
 * --------------------------------------------------------------------------
 * char * schd_strdup(char *string)
 *
 *      strdup(3) is not required for POSIX compliance, so we must provide
 *      a "lookalike".
 *
 * --------------------------------------------------------------------------
 * char * schd_lowercase(char *string)
 *
 *      Convert characters of a string to lowercase.  Return a pointer to
 *      the original string.
 *
 * --------------------------------------------------------------------------
 * char * schd_shorthost(char *fqdn)
 *
 *      Return a pointer to a copy of the host part of the fqdn presented.
 *      I.e. if fqdn is "foo.bar.com", copy the host part and return a
 *      pointer to it, with the contents "foo". If it is already short,
 *      return a copy anyway.
 *
 * --------------------------------------------------------------------------
 * int schd_move_job_to(Job *thisjob, Queue *destq)
 *
 *      Move the internal representation of the given job from the list on
 *      its queue to the tail of the destination queue's list. If the
 *      destination queue is NULL, this is equivalent to deleting the job
 *      from the per-queue lists.
 *
 * --------------------------------------------------------------------------
 * int schd_prime_time(time_t when)
 *
 *      It is important to be able to accurately determine when it is prime
 *      and non-prime time. This is accomplished by reading the holidays
 *      file and creating a table of prime and non-prime times. A quick
 *      search of the table can then determine whether we are in prime or
 *      non-prime time.
 *
 *      Given a time_t, determine if it is prime or nonprime time then. If
 *      the 'when' argument is 0, find out if it is primetime *now*.
 *      Return 1 for prime time, 0 for nonprime time.
 *
 * --------------------------------------------------------------------------
 * static int day_prime_time(int dow, int now)
 *
 *      Based on time of day and day of week, is it prime time now?
 *
 * --------------------------------------------------------------------------
 * int schd_read_holidays(void)
 *
 *      Initialize the holidays[] array from the contents of the holidays
 *      file
 *
 * --------------------------------------------------------------------------
 * void init_holidays(void)
 *
 *      Initialize weekdays array.  Default is "no primetime anytime", and
 *      no holidays.  Holidays are loaded from a compile time defined file,
 *      (see toolkit.h) which defaults to "/usr/lib/acct/holidays".
 *
 * --------------------------------------------------------------------------
 * int schd_secs_til_prime(time_t when)
 *
 *      Calculate the number of seconds until the next prime time begins.
 *      If the 'when' argument is non-zero, determine how long from the
 *      time given. Otherwise, determine how long it is from now.
 *
 * --------------------------------------------------------------------------
 * int schd_secs_til_nonprime(time_t when)
 * int schd_reset_observed_pt (QueueList *qlist)
 * int schd_reject_job(Job *job, char *reason)
 *
 *      Calculate the number of seconds until the next non-prime time
 *      begins. If the 'when' argument is non-zero, determine how long
 *      from the time given. Otherwise, determine how long it is from now.
 *
 * --------------------------------------------------------------------------
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/times.h>
#include <sys/param.h>
#include <sys/stat.h>

#include "toolkit.h"
#include "gblxvars.h"

#include "pbs_error.h"
#include "log.h"

char schd_VersionString[] = "MSIC Custom PBS Cluster Scheduler (version 1.5)\n";

/*
 *      Given a state_count string, return the integer value of the
 *      state requested.
 */
int
schd_how_many(char *str, char *state)
  {
  /* char   *id = "how_many"; */
  char   *avalue, *ptr1;
  int     intval = 0;

  /* Make a duplicate of the string since strtok() is destructive. */
  avalue = schd_strdup(str);

  if (avalue == NULL)
    return (0); /* XXX - return an error (malloc) here? */

  /* Search for a token that matches the requested state */
  for (ptr1 = strtok(avalue, " "); ptr1 != NULL; ptr1 = strtok(NULL, " "))
    if (!strncmp(state, ptr1, strlen(state)))
      break;

  if (ptr1 == NULL)
    goto free_and_exit; /* XXX return an error */

  /* Locate the number after the colon */
  ptr1 = strchr(ptr1, ':');

  if (ptr1 == NULL)
    goto free_and_exit; /* XXX return an error? */

  ptr1++;

  intval = atoi(ptr1);

free_and_exit:
  free(avalue);  /* Free the memory schd_strdup() allocated. */

  return (intval);
  }


/* Power-of-two unit multipliers. */
#define KILO ((size_t)1024)
#define MEGA (KILO*1024)
#define GIGA (MEGA*1024)
#define TERA (GIGA*1024)

/*
 * byte2val(bytes)
 *
 * Return a pointer to a static string that is the shortest string by which
 * the number of bytes can be accurately represented. i.e.:
 *
 * 1023   -> 1023b
 * 16384   -> 16kb
 * 1048576   -> 1mb
 * 16777216  -> 16mb
 * 16777217  -> 16777217b
 * 34359738368  -> 32gb
 */

char *
schd_byte2val(size_t bytes)
  {
  int     log_1024  = 0; /* logarithm base 1024 of multiplier */
  size_t  mult      = 1; /* Initial multiplier */
  size_t  next_mult = 1024; /* multiplier of next-highest unit */
  static char string[32];
  char   *units[] =
    {
    "b",   /* bytes     */
    "kb",   /* kilobytes */
    "mb",   /* megabytes */
    "gb",   /* gigabytes */
    "tb",   /* terabytes */
    "pb",   /* petabytes */
    "eb"  /* exabytes  */
    };

  /*
   * Find the first multiplier by which the given byte count is not
   * evenly divisible.  If we overflow the next multiplier, we have
   * gone far enough.
   */

  while (bytes && (bytes % next_mult) == 0)
    {
    mult = next_mult;
    next_mult <<= 10;
    log_1024 ++;

    if (next_mult == 0)
      break;
    }

  /*
   * Make 'bytes' be the number of units being represented.
   */
  bytes /= mult;

  /*
   * Create a string from number of units, and the symbol for that unit.
   */
  sprintf(string, "%lu%s", (unsigned long)bytes, units[log_1024]);

  /*
   * Remember: this is a static string!
   */
  return (string);
  }

/*
 * Convert seconds into "HH:MM:SS" format.  Note that the returned string
 * is a pointer to a static buffer.  The caller must copy the time string
 * into a holding buffer before the next call to sec2val() occurs.
 */

char   *
schd_sec2val(int seconds)
  {
  /* char   *id = "sec2val"; */
  static char tval[16];

  int     hours   = 0;
  int     minutes = 0;

  /* Hours */

  if (seconds >= (60 * 60))
    hours = seconds / (60 * 60);

  seconds -= (hours * (60 * 60));

  /* Minutes */
  if (seconds >= 60)
    minutes = seconds / 60;

  /* Seconds */
  seconds -= (minutes * 60);

  (void)sprintf(tval, "%2.2d:%2.2d:%2.2d", hours, minutes, seconds);

  return (&tval[0]);
  }

/* Convert an allocation value string to its equivalent value in bytes. */

size_t
schd_val2byte(char *val)
  {
  char   *id = "schd_val2byte";
  int     b = 0;
  size_t num = 0;
  char   *p;

  if (val == NULL)
    return (0);

  if (val[0] == '?')
    {
    (void)sprintf(log_buffer, "error from getreq(physmem): %s: [%d, %d]",
                  val, pbs_errno, errno);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    return (0);
    }

  b = 0;

  num = 0;
  num = strtoul(val, &p, 0);


  /* Now 'p' should point to first non-number character. */

  /* If no units given, return the number. */

  if ((p == val) || (*p == '\0'))
    return (num); /* XXX return num if p == val? */

  /* Parse the given order of magnitude. */
  switch (*p)
    {

    case 'k':

    case 'K':
      num *= KILO;
      break;

    case 'm':

    case 'M':
      num *= MEGA;
      break;

    case 'g':

    case 'G':
      num *= GIGA;
      break;

    case 't':

    case 'T':
      num *= TERA;
      break;

    case 'b':

    case 'B':
      b++;
      break;

    default:
      return (-1);
    }

  /* XXX if (b && *p != '\0') return error. */

  /* Parse the word-size unit. */
  p++;

  if ((*p != '\0') && !b)
    {
    switch (*p)
      {

      case 'b':

      case 'B':
        break;

      case 'w':

      case 'W':
        num *= NBPW; /* Multiply by size of word. */
        break;

      default:
        return (-1);
      }
    }

  /* Return the total number of bytes represented by the value. */
  return (num);
  }

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

  if ((strcmp(lower, "true") == 0)    ||
      (strcmp(lower, "on") == 0)      ||
      (strcmp(lower, "enable") == 0)  ||
      (strcmp(lower, "enabled") == 0) ||
      (strcmp(lower, "yes") == 0)     ||
      (strcmp(lower, "yes") == 0)     ||
      (strcmp(lower, "y") == 0)       ||
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

  return (1);  /* Did not parse successfully. */
  }

char *
schd_bool2val(int bool)
  {
  if (bool != 0)
    return "true";

  return "false";
  }

/*
 *  Convert a value string from time to its equivalent value in seconds.
 */

time_t
schd_val2sec(char *val)
  {
  char   *id = "val2sec";
  char   *p1, *p2, *p3, *end, *zero = "0";
  time_t  v1, v2, v3;
  time_t  sec;
  char   *valcopy;

  p3 = NULL;

  valcopy = schd_strdup(val);

  if (valcopy == NULL)
    {
    (void)sprintf(log_buffer, "schd_strdup(val) failed.");
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    return ((time_t) - 1);
    }

  /* Split string into at most 3 tokens. */
  p1 = strtok(valcopy, ":");

  p2 = strtok(NULL, ":");

  if (p2 != NULL)
    {
    p3 = strtok(NULL, ":");
    }

  /* Only seconds specified.  Shift right 2 places into p3, and zero fill */
  if (p2 == NULL)
    {
    p3 = p1;
    p2 = zero;
    p1 = zero;
    }

  /* Minutes and seconds specified.  Shift right into p3, and zero fill */
  if (p3 == NULL)
    {
    p3 = p2;
    p2 = p1;
    p1 = zero;
    }

  v1 = (time_t)strtol(p1, &end, 10);

  if (*end != '\0')
    goto error;

  if (v1 < 0)
    goto error;

  v2 = (time_t)strtol(p2, &end, 10);

  if (*end != '\0')
    goto error;

  if (v2 < 0 || v2 > 59)
    goto error;

  v3 = (time_t)strtol(p3, &end, 10);

  if (*end != '\0')
    goto error;

  if (v3 < 0 || v3 > 59)
    goto error;

  free(valcopy);

  sec = (v1 * 3600) + (v2 * 60) + v3;

  return (sec);

error:

  (void)sprintf("Can't parse time '%s' into seconds.\n", val);

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

  DBPRT(("%s: %s\n", id, log_buffer));

  free(valcopy);

  return ((time_t) - 1);
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

/*
 * Routines to check if files have been changed, deleted, created, etc. since
 * the last invocation.  Used to automatically load new configuration and
 * statistics data.
 */


struct filestatus
  {

  struct filestatus *next;
  char    *filename;
  time_t   ctime;
  int      exists;
  };

typedef struct filestatus FileStatus;

static FileStatus *filestats = NULL;

int
schd_register_file(char *filename)
  {
  char    *id = "schd_register_file";
  FileStatus *stats, *tail, *new = NULL;

  /*
   * Look for the tail of the list.  While walking the list, check to see
   * that the filename is not already registered.
   */
  tail = NULL;

  for (stats = filestats; stats != NULL; stats = stats->next)
    {
    if (strcmp(filename, stats->filename) == 0)
      {
      sprintf(log_buffer, "%s: file %s already registered.", id,
              filename);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));
      return (-1);
      }

    tail = stats;
    }

  /* Create space for the new record. */
  new = (FileStatus *) malloc(sizeof(FileStatus));

  if (new == NULL)
    {
    sprintf(log_buffer,
            "%s: out of memory allocating FileStatus for file %s",
            id, filename);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    return (-1);
    }

  /* Clear the record out -- this clears the ctime and next pointer. */
  memset(new, 0, sizeof(FileStatus));

  /* Keep a copy of the filename around. */
  new->filename = schd_strdup(filename);

  if (new->filename == NULL)
    {
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
               "schd_strdup(filename)");
    free(new);
    return (-1);
    }

  /*
   * If this is not the first element, tack it on the end of the list.
   * Otherwise, start the list with it.
   */
  if (tail)
    tail->next = new;
  else
    filestats = new;

  (void)sprintf(log_buffer, "%s: file %s registered.", id, filename);

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

  /*
   * Load the new element with the initial values for the file.  Ignore
   * the return value - only setting up the timestamp and file existence
   * status are important.
   */
  (void)schd_file_has_changed(filename, 1);

  return (0);
  }

int
schd_file_has_changed(char *filename, int reset_stamp)
  {
  char   *id = "schd_file_has_changed";

  FileStatus *stats;

  struct stat stat_buffer;
  int     exists;

  /* Assume that the file has not changed, and that it exists. */
  exists      = 1;

  if (filename == NULL)
    {
    DBPRT(("%s: filename is null\n", id));
    return (-1);
    }

  for (stats = filestats; stats != NULL; stats = stats->next)
    {
    if (strcmp(filename, stats->filename) == 0)
      break;
    }

  if (stats == NULL)
    {
    DBPRT(("%s: filename %s not registered\n", id, filename));
    return (-1);
    }

  /* Get the file modification times from the filesystem. */
  if (stat(filename, &stat_buffer) == -1)
    {
    if (errno == ENOENT)
      {
      exists = 0;
      }
    else
      {
      (void)sprintf(log_buffer,
                    "%s: stat(%s) failed: %d", id, filename, errno);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      return (-1);
      }
    }

  /*
   * Has file has changed state?
   */
  if (exists != stats->exists)
    {
    stats->exists = exists;

    if (exists && reset_stamp)
      stats->ctime = stat_buffer.st_ctime;

    return (1);
    }

  /*
   * If the ctime is different from the previously recorded one, the
   * file has changed.  stat(2) indicates that ctime will be changed
   * on every write, truncate, etc.  Update the ctime value for the
   * next call.
   */
  if (exists && (stat_buffer.st_ctime != stats->ctime))
    {
    if (reset_stamp)
      stats->ctime = stat_buffer.st_ctime;

    return (1);
    }

  /*
   * Either file still does not exist, or it has not been changed since
   * the last call.
   */

  return (0);
  }

/*
 * "Forget" about the file named by 'filename', or all files if 'filename'
 * is a NULL pointer.  Returns the number of files removed from the watch
 * list, or -1 if the file is not found (or the list was empty).
 */
int
schd_forget_file(char *filename)
  {
  /* char   *id = "schd_forget_file"; */
  FileStatus *stats, *prev, *next;
  int  count;

  count = 0;
  prev  = NULL;

  /*
   * Remove entries in the list of file stats being watched that match
   * the supplied filename, or all entries if 'filename' is NULL.
   */

  for (stats = filestats; stats != NULL; stats = next)
    {
    next = stats->next;

    if (filename && (strcmp(filename, stats->filename) != 0))
      {
      prev = stats;
      continue;
      }

    if (stats == filestats)
      {
      filestats = next;
      }
    else
      {
      prev->next = next;
      }

    /* Free the schd_strdup()'d filename */
    free(stats->filename);

    free(stats);

    count ++;
    }

  if (count)
    return (count);
  else
    return (-1);
  }

/*
 * Given a batch_status structure (bs), search the attributes for the
 * requested attribute (at) and return it's value (v).  If the attribute
 * contains a resource list, then search the resource list for the requested
 * resource (rs).  The function returns the value if found or a NULL if
 * the search is not successful.
 */

char *
schd_getat(char *at, Batch_Status *bs, char *rs)
  {
  /* char   *id = "getat"; */
  AttrList *a;

  for (a = bs->attribs; a != NULL; a = a->next)
    {
    /* Is this the attribute we are looking for? */
    if (!strcmp(at, a->name))
      {

      /*
       * If no resource specified, OR this resource matches the
       * requested one, return a pointer to the attribute's value.
       */
      if ((rs == NULL) || (strcmp(rs, a->resource) == 0))
        return (a->value);
      }
    }

  return (NULL); /* No matching attributes. */
  }


/*
 * POSIX/ANSI does not provide a portable method for finding the system time
 * of day at sub-second resolution.  gettimeofday() is not POSIX 1003.1
 * compliant, so it cannot be used if _POSIX_SOURCE is defined.
 *
 * POSIX 1003.1 provides the times() library call, which returns the number
 * of ticks since some fixed time.  From this number of ticks, an elapsed
 * time can be calculated to sub-second granularity.  Note that the system
 * must be asked how many ticks there are per second.
 */

#ifndef HAVE_GETTIMEOFDAY
#if (!defined(_POSIX_SOURCE) && !defined(_POSIX_C_SOURCE)) || \
 (defined(sgi) && (defined(_BSD_COMPAT) || defined(_BSD_TIME)))
#define HAVE_GETTIMEOFDAY
#endif /* ! _POSIX_SOURCE */
#endif /* ! HAVE_GETTIMEOFDAY */

void
schd_timestamp(char *msg)
  {
#ifdef DEBUG
  char *id = "schd_timestamp";
  static char  timestr[32];
  time_t sec;

  struct tms tms;
  static clock_t oticks = 0;
  clock_t ticks;
  clock_t dsec, dmsec;
  static int tickspersec = 0;

#ifdef HAVE_GETTIMEOFDAY

  struct timeval now;

  if (gettimeofday(&now, NULL))
    {
    DBPRT(("%s: gettimeofday() failed.\n", id));
    return;
    }

  sec = now.tv_sec;

#else /* HAVE_GETTIMEOFDAY */

  if (time(&sec) == (time_t)(-1))
    {
    DBPRT(("%s: gettimeofday() failed.\n", id));
    return;
    }

#endif /* else HAVE_GETTIMEOFDAY */

  strcpy(timestr, ctime(&sec));

  timestr[19] = '\0';

#ifdef HAVE_GETTIMEOFDAY
  DBPRT(("%s: %-10s %s.%03ld ", id, (msg ? msg : "TIMESTAMP"),
         &timestr[11], now.tv_usec / 1000));

#else /* HAVE_GETTIMEOFDAY */
  DBPRT(("%s: %-10s %s ", id, (msg ? msg : "TIMESTAMP"), &timestr[11]));

#endif /* else HAVE_GETTIMEOFDAY */

  /* If not already known, find the number of ticks per second. */
  if (tickspersec == 0)
    {
    tickspersec = (int)sysconf(_SC_CLK_TCK);

    if (tickspersec < 0)
      {
      DBPRT(("%s: sysconf(_SC_CLK_TCK) failed.\n", id));
      return;
      }
    }

  ticks = times(&tms);

  if (ticks == (clock_t)(-1))
    {
    DBPRT(("%s: times() failed.\n", id));
    return;
    }

  if (oticks)
    {
    dsec  = (ticks - oticks) / tickspersec;
    dmsec = (((ticks - oticks) % tickspersec) * 1000) / tickspersec;

    DBPRT(("elapsed %ld.%03ld\n", dsec, dmsec));
    }

  oticks = ticks;

#endif /* DEBUG */

  return;
  }

/*
 * Free a set of QueueList structures that reference (through qptr->queue)
 * an existing set of Queue structs.  The Queues themselves are not affected.
 */
int
schd_free_qlist(QueueList *qlist)
  {
  QueueList *qptr, *next;
  int   num = 0;

  for (qptr = qlist; qptr != NULL; qptr = next)
    {
    next = qptr->next;
    free(qptr);
    num++;
    }

  return (num);
  }

/*
 * This function differs from the schd_free_qlist() function in that it
 * actually frees the Queue's themselves, while schd_free_qlist() simply
 * destroys the list structures that point to the Queue's.
 */

int
schd_destroy_qlist(QueueList *list)
  {
  int num_items = 0;
  QueueList *qptr, *qnext;

  for (qptr = list; qptr != NULL; qptr = qnext)
    {
    qnext = qptr->next;

    if (qptr->queue->qname)
      free(qptr->queue->qname); /* Free strdup()'d storage. */

    if (qptr->queue->useracl)
      schd_free_useracl(qptr->queue->useracl);

    if (qptr->queue->exechost)
      free(qptr->queue->exechost);

    free(qptr->queue);   /* Free the queue struct. */

    free(qptr);    /* And the list that pointed to it. */

    num_items++;   /* Count it. */
    }

  return (num_items);
  }

/*
 * strdup(3) is not required for POSIX compliance, so we must provide a
 * "lookalike".
 */
char *
schd_strdup(char *string)
  {
  size_t  length;
  char   *copy;

  /*
   * Allocate new space for a copy of the string contents, and a
   * trailing '\0'.
   */
  length = strlen(string) + 1;

  copy = malloc(length);

  if (copy == NULL)
    return (NULL);

  memcpy(copy, string, length);

  return(copy);
  }

/*
 * Convert characters of a string to lowercase.  Return a pointer to
 * the original string.
 */
char *
schd_lowercase(char *string)
  {
  char   *p;

  for (p = string; *p != '\0'; p++)
    {
    if (isupper(*p))
      *p = tolower(*p);
    }

  return (string);
  }

/*
 * Return a pointer to a copy of the host part of the fqdn presented.  I.e.
 * if fqdn is "foo.bar.com", copy the host part and return a pointer to it,
 * with the contents "foo".  If it is already short, return a copy anyway.
 */
char *
schd_shorthost(char *fqdn)
  {
  char   *shorthost, *dotp;
  size_t  length;

  /* Check for bogus fqdn. */

  if (*fqdn == '.')
    return (NULL);

  if ((dotp = strchr(fqdn, '.')) != NULL)
    length = (size_t)(dotp - fqdn);
  else
    length = strlen(fqdn);

  if ((shorthost = (char *)malloc(length + 1)) != NULL)
    {
    memcpy(shorthost, fqdn, length);
    shorthost[length] = '\0';
    }

  return (shorthost);
  }

/*
 * Move the internal representation of the given job from the list on its
 * queue to the tail of the destination queue's list.
 *
 * If the destination queue is NULL, this is equivalent to deleting the job
 * from the per-queue lists.
 */

int
schd_move_job_to(Job *thisjob, Queue *destq)
  {
  Job    *prev, *tail;
  Queue  *srcq;

  srcq = thisjob->queue;

  if (srcq == NULL)
    {
    DBPRT(("move_job_to(Job %s, Queue %s) before job->queue init'd\n",
           thisjob->jobid, destq ? destq->qname : "[dead]"));
    return (-1);
    }

  if (srcq->jobs == NULL)
    {
    DBPRT(("job %s says queue %s is owner, but joblist is NULL.\n",
           thisjob->jobid, srcq->qname));
    return (-1);
    }

  /*
   * If the head of the source queue's job list is not the job in question,
   * walk down the list until we find the element before the job (i.e.
   * until the element's next pointer is equal to the job's pointer).
   */
  prev = NULL;

  if (srcq->jobs != thisjob)
    {
    for (prev = srcq->jobs; prev != NULL; prev = prev->next)
      {
      if (prev->next == thisjob)
        break;
      }

    if (prev == NULL)
      {
      DBPRT(("job %s says queue %s is owner, but not on queue joblist.\n",
             thisjob->jobid, srcq->qname));
      return (-1);
      }
    }

  /*
   * Account for the moved job.  Decrement the appropriate counters on the
   * source queue, and increment them on the destination queue (if present)
   */

  switch (thisjob->state)
    {

    case 'R':
      srcq->running --;

      if (destq)
        destq->running ++;

      break;

    case 'Q':
      srcq->queued --;

      if (destq)
        destq->queued ++;

      break;

    default: /* Do nothing.  Present for completeness. */
      break;
    }

  /*
   * Remove the job from the source queue's list.  The previous pointer may
   * be NULL -- this indicates that the job is the head of the source list.
   * In that case, simply move the source queue's pointer forward and we're
   * done.  Otherwise, point the previous job's next pointer to skip over
   * this one.  Either way, the job is no longer a list, so set its next
   * pointer to NULL.
   */
  if (prev == NULL)
    srcq->jobs = srcq->jobs->next;
  else
    prev->next = thisjob->next;

  thisjob->next = NULL;

  if (destq)
    {
    /*
     * Append the job to the destination queue job list.  Like the source
     * queue, a NULL pointer in queue->jobs indicates that the list is
     * empty.  In this case, the detached job becomes the head of the
     * list.  Otherwise, find the tail of the list and hook the new job
     * onto the end of the list.
     */
    if (destq->jobs == NULL)
      {
      destq->jobs = thisjob;
      }
    else
      {
      for (tail = destq->jobs; tail->next != NULL; tail = tail->next)
        /* do nothing -- just walk the list */
        ;

      tail->next = thisjob;
      }

    /* Make the destination the owner of the job. */
    thisjob->queue = destq;

    }
  else
    {
    /*
     * Moving to a NULL queue is a job deletion.  This job is no longer
     * referenced on the source, so will be lost.  Free it's resources.
     */
    schd_free_jobs(thisjob);
    }

  return (0);
  }

/*
 * It is important to be able to accurately determine when it is prime and
 * non-prime time.  This is accomplished by reading the holidays file and
 * creating a table of prime and non-prime times.
 * A quick search of the table can then determine whether we are in prime
 * or non-prime time.
 */


#define SUNDAY   0
#define SATURDAY 1 /* XXX Saturday is 6 in timeval */
#define WEEKDAY  2
#define PRIME    0
#define NONPRIME 1

#define MAX_HOLIDAYS 256

/*
 * Range of valid primetime times is 0 - 86399 (hr * 3600 + min * 60 + sec).
 * Valid times cannot be negative.
 */
#define ALL      -1
#define NONE     -2

static int Num_holidays;
static int holidays[MAX_HOLIDAYS][2];
static int weekdays[3][2];
static int day_prime_time(int, int);
void init_holidays(void);

/*
 * Given a time_t, determine if it is prime or nonprime time then.  If
 * the 'when' argument is 0, find out if it is primetime *now*.
 * Return 1 for prime time, 0 for nonprime time
 */
int
schd_prime_time(time_t when)
  {

  struct tm *tmptr;
  int     cur_time; /* Now (decimal - 9:30AM == 930, 6:45PM == 1845) */
  int     h_idx;
  int     rcode;

#ifdef DEBUG

  if (!schd_ENFORCE_PRIME_TIME)
    {
    fprintf(stderr, "WARNING!!!  ");
    fprintf(stderr, "prime_time() called but ENFORCE_PRIME_TIME not set\n");
    fprintf(stderr, "WARNING!!!  ");
    fprintf(stderr, "This is almost certainly a bug!\n");
    return (0);
    }

#endif /* DEBUG */

  /* If no time specified, use the scheduler's "TimeNow" time_t. */
  if (when == 0)
    when = schd_TimeNow;

  tmptr = localtime(&when);

  cur_time = (tmptr->tm_hour * 3600) + (tmptr->tm_min * 60) + tmptr->tm_sec;

  /*
   * Check holidays array to determine if today is a holiday.  Holidays
   * are not primetime.  Note that days in struct timeval's are counted
   * starting at 0, not at the first like on the calendar.
   */
  for (h_idx = 0; h_idx < Num_holidays; h_idx++)
    {
    if (tmptr->tm_yday + 1 == holidays[h_idx][0])
      return (0);
    }

  /* Today isn't a holiday so is it SATURDAY, SUNDAY or WEEKDAY? */
  switch (tmptr->tm_wday)
    {

    case 0:
      rcode = day_prime_time(SUNDAY, cur_time);
      break;

    case 6:
      rcode = day_prime_time(SATURDAY, cur_time);
      break;

    default:
      rcode = day_prime_time(WEEKDAY, cur_time);
      break;
    }

#if 0
  DBPRT(("schd_prime_time: it %s primetime at %s", rcode ? "is" : "is not",
         asctime(tmptr)));

#endif /* 0 */

  return (rcode);
  }

/* Based on time of day and day of week, is it prime time now? */
static int
day_prime_time(int dow, int now)
  {
  int     prime_time;
  int     not_prime_time;

  prime_time = weekdays[dow][PRIME];
  not_prime_time = weekdays[dow][NONPRIME];

  switch (prime_time)
    {

    case ALL:
      return (1);  /* Prime time all day */

    case NONE:
      return (0);  /* No prime time today */

    default: /* XXX document this case */

      if (not_prime_time > prime_time)
        {
        if ((now >= prime_time) && (now < not_prime_time))
          return (1);
        else
          return (0);
        }
      else
        {
        if ((now >= not_prime_time) && (now < prime_time))
          return (0);
        else
          return (1);
        }
    }
  }

/* Initialize the holidays[] array from the contents of the holidays file */
int
schd_read_holidays(void)
  {
  char   *id = "schd_read_holidays";
  FILE   *fd;
  char    buf[255];
  char   *ptr, *err;
  int     h_idx = 0;
  long    day, line = 0;

  init_holidays();

  if ((fd = fopen(HOLIDAYS_FILE, "r")) == NULL)
    {
    (void)sprintf(log_buffer,
                  "Warning: holidays file (%s) is unreadable. Ignoring...",
                  HOLIDAYS_FILE);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));

    return (-1);
    }

  /* Read lines from the holidays file one by one. */
  while (fgets(buf, sizeof(buf), fd))
    {

    line ++;

    if (strlen(buf) == (sizeof(buf) - 1))
      {
      (void)sprintf(log_buffer,
                    "Line %ld of %s is too long.", line, HOLIDAYS_FILE);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));
      return (-1);
      }

    /* Trim off comments before anything else is done. */
    if ((ptr = strchr(buf, '*')) != NULL)
      * ptr = '\0';

    /* Find first whitespace-separated token in the remaining string. */
    ptr = strtok(buf, " \t");

    /* Skip blank lines (may have been only a comment). */
    if (ptr == NULL)
      continue;

    /* Skip 'YEAR ...' entries */
    if (!strncmp(ptr, "YEAR", 4))
      continue;

    /*
     * Line was not parsed otherwise -- must be a holiday.  Holidays
     * are non prime, so set current holiday list to the day number
     * of the current year on which the holiday is celebrated.
     * Note a syntax error if the number doesn't parse nicely, or it
     * is nonsensical.
     */

    day = strtol(ptr, &err, 10);

    if ((*err != '\0') || (day < 0) || (day > 365))
      {
      (void)sprintf(log_buffer,
                    "Syntax error in line %ld of %s", line, HOLIDAYS_FILE);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));
      return (-1);
      }

    /* SGI uses a day of 0 to indicate "ANY YEAR" - ignore these. */
    if (day == 0)
      continue;

    /* No loss of precision, since 1 <= day <= 365 (see above). */
    holidays[h_idx][0] = (int)day;

    holidays[h_idx][NONPRIME] = ALL;

    h_idx++;

    Num_holidays++;

    if (Num_holidays == MAX_HOLIDAYS)
      {
      (void)sprintf(log_buffer,
                    "Only %d holidays allowed - skipping any remaining in %s",
                    MAX_HOLIDAYS, HOLIDAYS_FILE);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));
      break;
      }
    }

  fclose(fd);

  return (Num_holidays);
  }

/*
 * Initialize weekdays array.  Default is "no primetime anytime", and no
 * holidays.
 *
 * Holidays are loaded from the file ''/usr/lib/acct/holidays''
 */
void
init_holidays(void)
  {
  /* Default is no primetime anytime. */

  weekdays[SATURDAY][PRIME]    = NONE;
  weekdays[SATURDAY][NONPRIME] = ALL;
  weekdays[SUNDAY][PRIME]      = NONE;
  weekdays[SUNDAY][NONPRIME]   = ALL;
  weekdays[WEEKDAY][PRIME]     = schd_PRIME_TIME_START;
  weekdays[WEEKDAY][NONPRIME]  = schd_PRIME_TIME_END;

  Num_holidays = 0; /* No holidays to start out. */
  }

#define MIDNIGHT 86400
/*
 * Calculate the number of seconds until the next prime time begins.
 * If the 'when' argument is non-zero, determine how long from the
 * time given.  Otherwise, determine how long it is from now.
 */
int
schd_secs_til_prime(time_t when)
  {

  struct tm *tmptr;
  time_t  now; /* Number of seconds elapsed since midnight */
  int     next_prime;
  int     prime_start;
  int     days;
  int     p;

  if (when == 0)
    {
    /* No time specified -- find out what time it is now. */
    time(&when);
    }

  tmptr = localtime(&when);

  /*
   * Calculate number of seconds elapsed since last midnight (00:00:00).
   */
  now = ((tmptr->tm_hour * 60 * 60) +
         (tmptr->tm_min * 60) +
         tmptr->tm_sec);

  /*
   * Is today a FRIDAY or SATURDAY?  Assume there is no primetime on
   * weekends, and calculate number of days forward from that.
   */

  switch (tmptr->tm_wday)
    {

    case 5: /* Friday */
      /*
       * If it's before the start of prime time, then the next prime time
       * is in a matter of hours.  Otherwise, prime time starts on the
       * following Monday (in 2 days).
       */

      p = weekdays[WEEKDAY][PRIME];

      if (now < p)
        days = 0;
      else
        days = 2;

      break;

    case 6: /* Saturday */
      /*
       * Primetime won't start until Monday, so we know we have at least
       * one day to wait (all on Sunday).
       */
      days = 1;

      break;

    default: /* Sunday through Thursday, expect primetime to start "soon" */
      days = 0;

      break;
    }

  /*
   * 'days' tells us how many days we need to get to a weekday (either
   * this day, or the following Monday morning).  This will be the first
   * prime time.  This will need a special case if a specific weekday
   * does not have a prime time.
   */

  prime_start = weekdays[WEEKDAY][PRIME];

  /*
   * If we are already past the start of this prime time, than the *next*
   * prime time will begin tomorrow morning.  To get the time until then,
   * take the number of seconds in a day, and subtract how far into today
   * you've gotten.  XXX is this right in all cases?
   */
  if (now >= prime_start)
    next_prime = MIDNIGHT - (now - prime_start);
  else
    next_prime = prime_start - now;

  /* Add in the time for any full days without primetime. */
  next_prime += (days * MIDNIGHT);

  return (next_prime);
  }

/*
 * Calculate the number of seconds until the next non-prime time begins.
 * If the 'when' argument is non-zero, determine how long from the
 * time given.  Otherwise, determine how long it is from now.
 */
int
schd_secs_til_nonprime(time_t when)
  {

  struct tm *tmptr;
  time_t  now; /* Number of seconds elapsed since midnight */
  int     prime_end;
  int     non_prime_start;

  if (when == 0)
    {
    /* No time specified -- find out what time it is now. */
    time(&when);
    }

  tmptr = localtime(&when);

  /*
   * Calculate number of seconds elapsed since last midnight (00:00:00).
   */
  now = ((tmptr->tm_hour * 60 * 60) +
         (tmptr->tm_min * 60) +
         tmptr->tm_sec);

  /* Find how many seconds from midnight until the next nonprime time. */
  non_prime_start = weekdays[WEEKDAY][NONPRIME];
  prime_end       = non_prime_start - now;

  return (prime_end);
  }

int
schd_reset_observed_pt(QueueList *qlist)
  {
  QueueList *qptr;
  int   nqueues;

  nqueues = 0;

  for (qptr = qlist; qptr != NULL; qptr = qptr->next)
    {
    qptr->queue->observe_pt =
      (schd_ENFORCE_PRIME_TIME && schd_TimeNow >= schd_ENFORCE_PRIME_TIME);
    ++ nqueues;
    }

  return (nqueues);
  }


/*
 * The size of the buffer to allocate to hold the message that will be
 * sent to the user.  This is dynamically allocated (once) since it may
 * need to be a very large buffer, depending on the complexity of the
 * message.
 */
#define MSG_BUFFER_SIZE 4000

extern int connector;

int
schd_reject_job(Job *job, char *reason)
  {
  char   *id = "schd_reject_job";
  static char *message = NULL;
  int     rc = 0;

  if (message == NULL)
    {
    if ((message = (char *)malloc(MSG_BUFFER_SIZE)) == NULL)
      {
      (void)sprintf(log_buffer, "cannot malloc %d bytes\n",
                    MSG_BUFFER_SIZE);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      return (-1);
      }
    }

  (void)sprintf(message,
                "\n"
                "PBS job '%s' was rejected by all execution queues.\n"
                "\n"
                "The reason given for this action was :\n"
                "\n"
                "       %s\n"
                "\n"
                "Please correct the problem and resubmit your job, or contact the PBS\n"
                "administrator for assistance.\n"
                "\n"
                "Thank you.\n"
                "\n",
                job->jobid, reason);

  /*
   * Ask PBS to delete the job from the queue, which should deliver the
   * message to the user.
   */
  rc = pbs_deljob(connector, job->jobid, message);

  if (rc)
    {
    (void)sprintf(log_buffer, "pbs_deljob failed: error %d", rc);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    return 1;
    }

  /*
   * Delete this job from the queue's list (move to a NULL queue)
   */
  DBPRT(("JOB %s DELETED!!!\n", job->jobid));

  DBPRT(("Message: %s\n", reason));

  schd_move_job_to(job, NULL);

  return 0;
  }

void
schd_check_primetime(void)
  {
  char *id = "check_primetime";

  /*
   * See if the holidays file has changed. If it's re-read successfully,
   * update the last changed timestamp.  Otherwise, keep it around and
   * keep trying to re-read it until someone fixes the problem. "This
   * shouldn't happen."
   */

  if (schd_file_has_changed(HOLIDAYS_FILE, 0) > 0)
    {
    (void)sprintf(log_buffer,
                  "Attempting to update holidays/primetime from %s.",
                  HOLIDAYS_FILE);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s\n", log_buffer));

    if (schd_read_holidays() < 0)
      {
      (void)sprintf(log_buffer, "Failed to read holidays file.");
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                 log_buffer);
      DBPRT(("%s\n", log_buffer));
      }
    else
      {
      /* Reset the "last changed time", since it was re-read okay. */
      (void)schd_file_has_changed(HOLIDAYS_FILE, 1);
      }
    }

  /*
   * If this is the first run during non-primetime, set all the execution
   * queues' observed primetime back to 'on'.  If it's primetime now, set
   * the "last run in primetime" global.
   */
  if (schd_prime_time(0))
    {
    last_run_in_pt = 1;

    }
  else if (last_run_in_pt)
    {
    DBPRT(("%s: First non-pt run, reset queue observed times.\n", id));

    if (schd_BatchQueues)
      schd_reset_observed_pt(schd_BatchQueues);

    /* Last run was not in prime time. */
    last_run_in_pt = 0;
    }
  }
