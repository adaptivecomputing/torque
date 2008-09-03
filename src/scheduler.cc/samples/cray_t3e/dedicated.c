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
 *                 ***********************************
 *                 *** STANDARDS COMPLIANCE NOTICE ***
 *                 ***********************************
 *
 * This file makes use of the AT&T UNIX-based popen() and pclose() calls
 * to read the output of a user-supplied command (schd_DEDTIME_COMMAND).
 * This file will not build in a strict ANSI-only environment.  Neither
 * ANSI-C (ISO/IEC 9899-1990), nor the POSIX 1003.1 specification (ISO/IEC
 * 9945-1) specify the popen()/pclose() library functions.
 *
 * If the target machine does not have an implementation of popen() and
 * pclose(), a working implementation of the library routines should be
 * available from one of the free UN*X/BSD clones (i.e. NetBSD, FreeBSD,
 * OpenBSD, etc).
 *
 * most real machines do provide an implementation of the popen() and
 * pclose() library routines.  It may be necessary to turn off strict
 * ANSI-compliance modes to build this file.  It may also be necessary
 * to link with a compatibility library in order to use popen()/pclose().
 *
 * Note: popen()/pclose() are only necessary if dedicated time information
 * can only be retrieved from the output of a command.  Another strategy
 * might be to exec() the command, redirecting its output into a file, and
 * then use open()/read()/close() to parse the resultant flat file.
 */

#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "pbs_error.h"
#include "pbs_ifl.h"
#include "log.h"

#ifdef DBPRT
#undef DBPRT
#define DBPRT(X)
#endif /* DBPRT */

#include "toolkit.h"
#include "gblxvars.h"

struct outagelist
  {

  struct outagelist *next; /* next pointer */
  char   *exechost;  /* pointer to name of host for this element */
  Outage *outages;  /* list of outages for host */
  time_t  cached_at;  /* time the list was fetched or 0 if invalid */
  };

typedef struct outagelist OutageList;

OutageList *host_outage_list = NULL;

static int get_outages(char *exechost, Outage **outlistp);
static Outage *merge_outages(Outage *system_outages, Outage *host_outages);
static int MMDDYYYY_HHMM(char *MMDDYYYY, char *HHMM);
static int make_time_strs(Outage *outp);
static int free_outages(Outage *outages);
static int compare_outages_time(const void *e1, const void *e2);
static int resolve_outage_overlap(Outage **outlistp);
static char *print_outage(Outage *outp);

/*
 * The outage resolution algorithm uses an iterative approach to resolve
 * one outage at a time.  To prevent it looping indefinitely, set this
 * to some ludicrously large number (say, 250) of times that the list
 * should be passed through without resolving all the conflicts.  If this
 * happens, it is almost certainly a bug in the implementation.
 */
#define MAX_OUTAGE_RESOLVE_ITERS 250

/*
 * Invalidate any existing entries on the outage caches, and free the lists.
 */
void
schd_clear_outage_cache(void)
  {
  OutageList *outlp;

  for (outlp = host_outage_list; outlp != NULL; outlp = outlp->next)
    {
    free_outages(outlp->outages);
    outlp->cached_at = 0;
    outlp->outages   = NULL;
    }
  }

Outage *
schd_host_outage(char *exechost, time_t when)
  {
  char   *id = "schd_host_outage";
  OutageList *outlp, *newoutl;
  Outage *outages, *sys_out, *outp;

  if (when == 0)
    when = schd_TimeNow;
  else if (when < schd_TimeNow)
    {
    (void)sprintf(log_buffer,
                  "Asked for outages for time in the past - results may be inaccurate!");
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    }

  /*
   * Scan the list of host outage lists for an entry that matches
   * the requested hostname.  If not found, then there is no data
   * cached.  If the data is old, purge it and ask for a new list
   * of outages for this host.
   */
  for (outlp = host_outage_list; outlp != NULL; outlp = outlp->next)
    {
    if (strcmp(exechost, outlp->exechost) == 0)
      {
      break;
      }
    }

  /* If no entry was found on the list, one must be created.  */
  if (outlp == NULL)
    {
    newoutl = (OutageList *)malloc(sizeof(OutageList));

    if ((newoutl == NULL) ||
        ((newoutl->exechost = schd_strdup(exechost)) == NULL))
      {
      (void)sprintf(log_buffer,
                    "Warning!  Couldn't allocate space to track dedicated times!");
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));
      return (NULL);
      }

    /* newoutl->exechost is set, now fill in the rest of the fields. */
    newoutl->next      = NULL;

    newoutl->outages   = NULL;

    /*
     * Indicate that the list pointed to by newoutl->outages is bogus and
     * needs to be re-fetched.
     */
    newoutl->cached_at = 0;

    /*
     * If the list is empty, make this the first element.  Otherwise,
     * place the new element at the tail of the existing list.
     */
    if (host_outage_list == NULL)
      {
      host_outage_list = newoutl;
      }
    else
      {
      for (outlp = host_outage_list; outlp->next; outlp = outlp->next)
        /* Just walk to last element in list. */ ;

      outlp->next = newoutl;
      }

    outlp = newoutl;
    }

  /*
   * outlp now points to a valid host OutageList.  See if the list of
   * outages hanging off the host name is still valid.
   */
  if ((outlp->cached_at == 0) ||
      (schd_TimeNow - outlp->cached_at) > schd_DEDTIME_CACHE_SECS)
    {
    DBPRT(("%s: dedicated time list for host %s ", id, outlp->exechost));

    if (outlp->cached_at)
      {
      DBPRT(("expired %s ago\n", schd_sec2val(schd_TimeNow -
                                              (outlp->cached_at + schd_DEDTIME_CACHE_SECS))));
      }
    else
      {
      DBPRT(("uninitialized\n"));
      }

    /*
     * Try to get a new list of outages.  Don't blow away the old one yet,
     * though.  If there's some problem getting the new list, the values
     * from the old list are more likely to be valid than a NULL list.
     */

    if (get_outages(exechost, &outages))
      {
      (void)sprintf(log_buffer,
                    "WARNING!  Could not update dedtime cache for %s!", exechost);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));

      if (outlp->cached_at)
        (void)sprintf(log_buffer, "WARNING!  Cached data is %s old!",
                      schd_sec2val(schd_TimeNow - outlp->cached_at));
      else
        (void)sprintf(log_buffer, "WARNING!  Cache is uninitialized!");

      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

      DBPRT(("%s: %s\n", id, log_buffer));

      }
    else
      {

      DBPRT(("%s: got valid list for %s - update cache\n", id, exechost));

      (void)sprintf(log_buffer,
                    "updating dedtime cache for %s (valid for %s)", exechost,
                    schd_sec2val(schd_DEDTIME_CACHE_SECS));
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));

      /* Set up back pointers to the OutageList's exechost string. */

      for (outp = outages; outp != NULL; outp = outp->next)
        outp->exechost = outlp->exechost;

#ifdef DEBUG_DEDTIME
      DBPRT(("%s: EXECHOST OUTAGES FOR %s:\n", id, exechost));

      for (outp = outages; outp != NULL; outp = outp->next)
        DBPRT(("%s:    %s\n", id, print_outage(outp)));

#endif /* DEBUG_DEDTIME */

      /*
       * Now that the valid per-host data has been received, check for
       * any system outage, if schd_SYSTEM_NAME is not NULL.  Mix these
       * into the list, handling overlaps.  Note that system dedtimes
       * take precedence over the per-host dedtimes.
       */
      if (schd_SYSTEM_NAME)
        {
        DBPRT(("%s: SYSTEM_NAME is '%s' - look for outages.\n", id,
               schd_SYSTEM_NAME));

        if (get_outages(schd_SYSTEM_NAME, &sys_out) == 0)
          {
          if (sys_out != NULL)
            {

            /* Mark these outages as system outages. */
            for (outp = sys_out; outp != NULL; outp = outp->next)
              {
              outp->exechost = schd_SYSTEM_NAME;
              outp->flags |= OUTAGE_FLAGS_SYSTEM;
              }

#ifdef DEBUG_DEDTIME
            DBPRT(("%s: SYSTEM OUTAGES FOR %s:\n", id, exechost));

            for (outp = sys_out; outp != NULL; outp = outp->next)
              DBPRT(("%s:    %s\n", id, print_outage(outp)));

#endif /* DEBUG_DEDTIME */

            (void)sprintf(log_buffer,
                          "merging system %s outages with host %s outages",
                          schd_SYSTEM_NAME, exechost);

            log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                       id, log_buffer);

            DBPRT(("%s: %s\n", id, log_buffer));

            /* Merge the sys_out outages on top of any outages. */
            outages = merge_outages(sys_out, outages);

            if (outages == NULL)
              DBPRT(("%s: dedtime merge failed\n", id));

            sys_out = NULL; /* list was merged into outages list. */
            }
          }
        else
          {
          (void)sprintf(log_buffer, "WARNING!  Could not get "
                        "outages for system '%s'!", schd_SYSTEM_NAME);
          log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                     id, log_buffer);
          DBPRT(("%s: %s\n", id, log_buffer));
          }
        }

      /* Throw out the old list. */
      free_outages(outlp->outages);

      /* And point to the new list of outages. */
      outlp->outages = outages;

      outlp->cached_at = schd_TimeNow;

      }
    }

  /*
   * There is now (at least somewhat) valid data in the host cache.  Find
   * the first entry on the list that we might be within.  One could clean
   * up this list if a dedtime had come and gone, but the benefits would
   * be quite minimal, given the ratio of cache time to length of the
   * scheduled outages (usually secs vs. hours).
   *
   * A NULL list means that there is no scheduled outage - return the NULL
   * and be done with it.
   */

#ifdef DEBUG_DEDTIME
  DBPRT(("%s: FINAL OUTAGES FOR %s:\n", id, exechost));

  for (outp = outlp->outages; outp != NULL; outp = outp->next)
    DBPRT(("%s:    %s\n", id, print_outage(outp)));

#endif /* DEBUG_DEDTIME */

  for (outp = outlp->outages; outp != NULL; outp = outp->next)
    {
    /* Ignore any outage that has already come and gone. */
    if (outp->end_time > when)
      break;

    DBPRT(("%s: Outage from %s:%s to %s:%s ignored\n", id,
           outp->beg_datestr, outp->beg_timestr,
           outp->end_datestr, outp->end_timestr));
    }

  return (outp);
  }

/*
 * Dedicated time is scheduled to begin soon.  Determine if the given job
 * will complete before dedicated time starts.
 */
int
schd_dedicated_can_run(Job *job, Queue *queue, time_t when, char *reason)
  {
#ifdef DEBUG_DEDTIME
  char   *id = "schd_dedicated_can_run";
#endif /* DEBUG_DEDTIME */
  Outage *host_out;
  char   *shorthost;
  time_t job_ends;

  /*
   * If no dedicated queues are defined, or dedicated time is not being
   * enforced, then there's no dedtime to worry about.
   */

  if (!(schd_ENFORCE_DEDTIME && schd_TimeNow >= schd_ENFORCE_DEDTIME))
    return(1);

  if (schd_DedQueues == NULL)
    return(1);

  /* If no time is given, assume that the caller means "now".  */
  if (when == 0)
    when = schd_TimeNow;

  /* Get the (possibly cached) dedicated time for the queue's exechost. */
  host_out = schd_host_outage(queue->exechost, when);

  /* No dedicated time for this host. */
  if (host_out == NULL)
    return (1);

  shorthost = schd_shorthost(host_out->exechost);

  job_ends = when + job->walltime;

  /* Is the time currently between the start and end of the dedicated time? */
  if ((host_out->beg_time <= when) && (when <= host_out->end_time))
    {
    /*
     * If the job was not queued on the dedicated queue, it cannot be
     * run during dedicated time.  Note that and return "not okay."
     */
    if (job->flags & JFLAGS_DEDICATED)
      {
      /*
       * The job is on the dedicated queue.  It may not be runnable
       * if it runs over past the end of dedicated time.  Check that.
       */
      if (job_ends >= host_out->end_time)
        {
        if (reason)
          (void)sprintf(reason,
                        "Can't complete within this dedicated time (%s %s %s)",
                        shorthost ? shorthost : host_out->exechost,
                        host_out->end_datestr, host_out->end_timestr);

        return 0;
        }
      }
    else
      {
      /* Job is a regular job.  It cannot run during dedicated time. */
      if (reason)
        sprintf(reason, "Waiting for end of dedicated time (%s %s %s)",
                shorthost ? shorthost : host_out->exechost,
                host_out->end_datestr, host_out->end_timestr);

      return 0;

      }
    }
  else
    {
    /*
     * It is not currently dedicated time.  However, the job may spill
     * over into dedtime.  Check this by calculating when the job will
     * finish execution and seeing if that falls within dedicated time.
     */
    if (job_ends >= host_out->beg_time)
      {
      if (reason)
        (void)sprintf(reason,
                      "Can't complete before next dedicated time (%s %s %s)",
                      shorthost ? shorthost : host_out->exechost,
                      host_out->beg_datestr, host_out->beg_timestr);

      return 0;
      }
    }

#ifdef DEBUG_DEDTIME
  DBPRT(("%s: job %s is okay.\n", id, job->jobid));

#endif /* DEBUG_DEDTIME */
  return 1; /* Job will not interfere with dedicated time if run now. */
  }

/*
 * Get a linked list of outages for host exechost.  The caller is expected
 * to point the 'exechost' pointer for each element in this list to a piece
 * of storage containing the exechost's name.  This is crufty, but minimizes
 * the amount of extra allocation being done.
 */
static int
get_outages(char *exechost, Outage **outlistp)
  {
  char   *id = "get_outages";
  char    buffer[BUFSIZ];
  char    ded_ck_cmd[BUFSIZ];
  char   *p, *shortexec;
  char    Start_Date[DATE_LENGTH + 1]; /* + 1 for '\0' */
  char    Start_Time[TIME_LENGTH + 1]; /* + 1 for '\0' */
  char    End_Date[DATE_LENGTH + 1];  /* + 1 for '\0' */
  char    End_Time[TIME_LENGTH + 1];  /* + 1 for '\0' */
  char   *ded_system;
  FILE   *schedule_pipe;
  Outage *outs, *tail, *newout;
  int     info_valid, ded_cancelled;
  size_t  length;

  outs = tail = NULL;

  /*
   * Assume we will not get valid dedtime information from the piped command.
   */
  info_valid    = 0;
  Start_Date[0] = '\0';
  Start_Time[0] = '\0';
  End_Date[0]   = '\0';
  End_Time[0]   = '\0';

  /* Get the "short" version of the exechost. */
  shortexec = schd_shorthost(exechost);

  if (shortexec == NULL)
    return (-1);

  sprintf(ded_ck_cmd, "%s %s", schd_DEDTIME_COMMAND, shortexec);

  DBPRT(("%s: Consult '%s'\n", id, ded_ck_cmd));

  if ((schedule_pipe = popen(ded_ck_cmd, "r")) == NULL)
    {

    (void)sprintf(log_buffer, "ERROR: \"%s\": %s", ded_ck_cmd,
                  strerror(errno));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));

    free(shortexec);
    return (1);
    }

  /* Read each entry from the schedule output. */
  while (fgets(buffer, sizeof buffer, schedule_pipe) != NULL)
    {

    ded_cancelled = 0;

    /* Skip totally blank lines. */

    if ((length = strlen(buffer)) == 0)
      continue;

    /* Flag an error if this line was too long. */
    if (length >= (sizeof(buffer) - 1))
      {
      (void)sprintf(log_buffer,
                    "%s: Input line from '%s' too long.", id, ded_ck_cmd);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));
      info_valid = 0;
      break;
      }

    /* Remove trailing carriage return. */
    if (buffer[length - 1] == '\n')
      buffer[length - 1] = '\0';

    /* Check for error conditions returned by the program. */
    if (!strncmp(buffer, "ERROR", 5))
      {
      info_valid = 0;
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, buffer);
      DBPRT(("%s: %s\n", id, buffer));
      break;
      }

    if (!strcmp(buffer,
                "No scheduled downtime for the specified period."))
      {
      info_valid ++;
      break;  /* None scheduled */
      }

    /*
     * Output of 'schedule' "linewraps" with more than 8 spaces before
     * the text.  I.e. more than 8 spaces means that this is a continued
     * line.  We are only interested in the "header" lines.
     */
    if (!strncmp(buffer, "        ", 8))
      {
      continue;
      }

    if (strstr(buffer, "CANCELLED"))
      ded_cancelled ++;

    /* format: "SYSTEM MM/DD/YYYY HH:MM-HH:MM MM/DD/YYYY" */
    p = strtok(buffer, " \t");  /* p -> '<system>' */

    if (p == NULL)
      continue;

    /* Keep track of which system this line references. */
    ded_system = p;

    p = strtok(NULL, " \t");  /* p -> '<startdate>' */

    if (p == NULL)
      continue;

    strncpy(Start_Date, p, DATE_LENGTH);

    p = strtok(NULL, "-");   /* p -> '<starttime>' */

    if (p == NULL)
      continue;

    strncpy(Start_Time, p, TIME_LENGTH);

    p = strtok(NULL, " \t");  /* p -> '<endtime>' */

    if (p == NULL)
      continue;

    strncpy(End_Time, p, TIME_LENGTH);

    p = strtok(NULL, " \t");  /* p -> '<enddate>' */

    if (p == NULL)
      continue;

    strncpy(End_Date, p, DATE_LENGTH);

    /*
     * We have discovered a line that "looks okay".  Consider the output
     * being sent from the schedule command to be "acceptable".
     */
    info_valid++;

    /*
     * It may be a valid line, and still not be information about this
     * machine.  Convert the string to all lowercase before comparing
     * it against the lowercase system name.
     */
    schd_lowercase(ded_system);

    if (strcmp(ded_system, shortexec) != 0)
      {
      continue;
      }

    if (MMDDYYYY_HHMM(Start_Date, Start_Time) >=
        MMDDYYYY_HHMM(End_Date, End_Time))
      {
      DBPRT(("%s: found dedtime shorter than 1 minute!\n", id));
      DBPRT(("%s: bad dedtime: (%s %s %s - %s %s)\n", id, shortexec,
             Start_Date, Start_Time, End_Date, End_Time));
      continue;
      }

    if (ded_cancelled)
      {
      /*
       * A cancellation is a valid entry.  It also means we need to
       * update our cache, since it may be cancelling the cached
       * value.  However, we also need to see if there is any time
       * really scheduled later.
       */
      DBPRT(("%s: found cancelled dedtime for %s from %s:%s to %s:%s\n",
             id, shortexec, Start_Date, Start_Time, End_Date, End_Time));
      info_valid ++;
      continue;
      }

    /*
     * Dedicated time is scheduled for this machine.  Will it end at
     * some _future_ time ??  If so, make a new entry in the list of
     * Outages for the shortexec.
     */
    if (MMDDYYYY_HHMM(End_Date, End_Time) > schd_TimeNow)
      {
      info_valid ++;

      if ((newout = (Outage *)malloc(sizeof(Outage))) == NULL)
        {
        (void)sprintf(log_buffer, "malloc(Outage) failed\n");
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   log_buffer);
        DBPRT(("%s: %s\n", id, log_buffer));

        info_valid = 0;
        break;
        }

      memset((void *)newout, 0, sizeof(Outage));

      /* Hook the new element on the end of the list. */

      if (tail)
        tail->next = newout;
      else
        outs = newout;

      tail = newout;

      newout->next = NULL;

      /* Load the outage information into the new struct. */

      newout->exechost = NULL; /* Caller must set. */

      strncpy(newout->beg_datestr, Start_Date, DATE_LENGTH);

      strncpy(newout->beg_timestr, Start_Time, TIME_LENGTH);

      strncpy(newout->end_datestr, End_Date, DATE_LENGTH);

      strncpy(newout->end_timestr, End_Time, TIME_LENGTH);

      newout->beg_time = MMDDYYYY_HHMM(Start_Date, Start_Time);

      newout->end_time = MMDDYYYY_HHMM(End_Date, End_Time);
      }
    }

  pclose(schedule_pipe);

  free(shortexec);

  if (info_valid)
    {

    *outlistp = outs;
    return 0;

    }
  else
    {
    DBPRT(("%s: no valid dedtime info found (%s)\n", id, ded_ck_cmd));

    if (outs)
      free_outages(outs);

    *outlistp = NULL;

    return 1;
    }
  }

static Outage *
merge_outages(Outage *system_outages, Outage *host_outages)
  {
  char     *id = "merge_outages";
  Outage  **outp_array, *outp, *newlist;
  int       num_outages, i, found;

  /* If there are no system outages, just return the host list.  */

  if (system_outages == NULL)
    return (host_outages);

  /* If there are no host outages, just return the system list.  */
  if (host_outages == NULL)
    return (system_outages);

  /* Count the total number of outages in both lists. */
  num_outages = 0;

  for (outp = system_outages; outp != NULL; outp = outp->next)
    ++ num_outages;

  for (outp = host_outages; outp != NULL; outp = outp->next)
    ++ num_outages;

  /* Now allocate space for a bunch of pointers to the outages. */
  outp_array = (Outage **)calloc(num_outages, sizeof(Outage *));

  if (outp_array == NULL)
    {
    (void)sprintf(log_buffer, "calloc() failed (%d * %d)\n", num_outages,
                  (int)sizeof(Outage *));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));

    /* Free the passed-in lists. */
    free_outages(system_outages);
    free_outages(host_outages);

    /* And return an error condition. */
    return (NULL);
    }

  /*
   * Walk the outage lists again, assigning each element in the array to
   * point to one outage.  Sort the resultant array in ascending order
   * of the start time of the outage pointed to by each element in the
   * array.
   */
  i = 0;

  for (outp = system_outages; outp != NULL; outp = outp->next)
    outp_array[i++] = outp;

  for (outp = host_outages; outp != NULL; outp = outp->next)
    outp_array[i++] = outp;

  qsort(outp_array, num_outages, sizeof(Outage *), compare_outages_time);

  /*
   * The array is now a set of pointers to outages.  Re-link the outages in
   * the order given in the array.
   */
  newlist = outp = outp_array[0];

  for (i = 1; i < num_outages; i++)
    {
    outp->next = outp_array[i];
    outp = outp->next;
    }

  outp->next = NULL;

  /* Free the array of pointers since it is no longer needed. */
  free(outp_array);

  /*
   * Now the 'newlist' list contains the members of both lists, arranged in
   * time from earliest to latest.  All that remains is to resolve any
   * overlapping cases.  Do this one at a time, until no more can be found.
   * This should significantly simplify the code, and since there should
   * only be a handful of outages at any time, it won't be prohibitively
   * expensive.
   */
  DBPRT(("%s: MERGED/SORTED OUTAGES LIST:\n", id));

  for (outp = newlist; outp != NULL; outp = outp->next)
    DBPRT(("%s:    %s\n", id, print_outage(outp)));

  for (i = 0; i < MAX_OUTAGE_RESOLVE_ITERS; i++)
    {
    found = resolve_outage_overlap(&newlist);

    if (found < 0)
      {
      (void)sprintf(log_buffer, "couldn't resolve overlaps!");
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));
      /* Free the merged contents of the two lists. */
      free_outages(newlist);
      return (NULL);
      }

    if (found == 0)
      {
      /* No overlaps were found and resolved, so nothing left to do. */
      break;
      }

    /* At least one overlap was found and fixed.  Try to find more. */
    continue;
    }

  if (i == MAX_OUTAGE_RESOLVE_ITERS)
    {
    (void)sprintf(log_buffer,
                  "WARNING!!! Couldn't resolve all overlaps in %d passes!", i);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    }

  /*
   * 'newlist' should now contain a list of outages, either for the system
   * or for this individual host, ordered in time, and with no overlaps.
   */
  return (newlist);
  }

static int
resolve_outage_overlap(Outage **outlistp)
  {
  char    *id = "resolve_outage_overlap";
  Outage  *new, *discard;
  Outage  *outp, *next, *this, *host, *sys;
  time_t   sys_beg, sys_end, host_beg, host_end;
  int      found = 0;

  /*
   * Walk the list of outages, looking for overlaps and resolving them.
   * Once an overlap has been resolved, return 1 to indicate that there
   * was an overlap found.  In this case, this function should be called
   * again.  If no overlaps are found, return 0 to indicate that fact.
   * Return -1 on error.
   *
   * Note that system outages should take priority over per-host outages.
   *
   * The outages list *must* be sorted into ascending order of start-time
   * for the outages.
   *
   * The five possible overlap scenarios are ('*' indicates system outage
   * time, '-' indicates host outages, blank space is non-outage time) :
   *
   * Time      ***     ***     -----    -----             -----
   *  |       -***-    ***     -----     ***
   *  |       -***-   -----     ***      ***               ***
   *  |        ***    -----     ***     -----              ***
   *  |
   *  V   Case: 1       2        3        4                 5 (no overlap)
   */

  /*
   * Discarded outages go on this list, so they can be easily reclaimed
   * if another outage needs to be allocated.
   */
  discard = NULL;

  /*
   * Start with the original list head pointer.  This will be replaced
   * with the new head at the end of the for loop.
   */
  new  = *outlistp;

  for (this = new; this->next != NULL; this = next)
    {
    next = this->next;

    /*
     * If this outage ends before the next one starts, then there is no
     * overlap between these two elements.
     */

    if (this->end_time < next->beg_time)
      continue;

    /* Found an overlap. */
    found = 1;

    /*
     * Check the types of the two outages.  If they are the same, extend
     * the first entry to stretch through the whole time, note the fact.
     * Otherwise, figure out which is the host outage, and which is the
     * system outage, and assign pointers respectively.
     */
    if ((this->flags & OUTAGE_FLAGS_SYSTEM) ==
        (next->flags & OUTAGE_FLAGS_SYSTEM))
      {
      DBPRT(("%s: Two outages of same type (%s) overlap.\n", id,
             (this->flags & OUTAGE_FLAGS_SYSTEM) ? "system" : "host"));
      DBPRT(("%s: Outages are (%s)\n", id, print_outage(this)));
      DBPRT(("%s:    (%s)\n", id, print_outage(next)));

      /*
       * Expand both outages to cover the same time.  This will
       * cause it to be dealt with by case 1 below.
       */
      this->beg_time = MIN(this->beg_time, next->beg_time);
      this->end_time = MAX(this->end_time, next->end_time);
      next->beg_time = this->beg_time;
      next->end_time = this->end_time;

      /* Create new time/date strings for the different time_t's. */
      make_time_strs(this);
      make_time_strs(next);

      DBPRT(("%s: Overlap extended to (%s)\n", id, print_outage(this)));

      /* And assign them "relative priorities". */
      sys  = this;
      host = next;
      }
    else
      {
      /* Figure out which is the system and which is the host outage. */

      if (this->flags & OUTAGE_FLAGS_SYSTEM)
        {
        sys  = this;
        host = next;
        }
      else
        {
        host = this;
        sys  = next;
        }
      }

    DBPRT(("%s: system outage (%s) overlaps\n", id, print_outage(sys)));

    DBPRT(("%s:   host outage (%s)\n", id, print_outage(host)));

    sys_beg  = sys->beg_time;
    sys_end  = sys->end_time;
    host_beg = host->beg_time;
    host_end = host->end_time;

    /*
     * Quick sanity check for reasonable outage times.
     */

    if ((sys_end - sys_beg) < 1)
      {
      (void)sprintf(log_buffer,
                    "system outage (%s) less than 1 second!\n", print_outage(sys));
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));

      /* Note the problem and return an error. */
      found = -1;
      break;
      }

    if ((host_end - host_beg) < 1)
      {
      (void)sprintf(log_buffer,
                    "host outage (%s) less than 1 second!\n", print_outage(host));
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));

      /* Note the problem and return an error. */
      found = -1;
      break;
      }

    /* Case 1: 'sys' starts before, and ends after, 'host'.
     * This is a complete overlap (the easy case).  This is either a
     * duplicated host outage, or a system outage overlaps the host
     * outage.  In either case, choose one to discard and it's done.
     */
    if ((sys_beg <= host_beg) && (sys_end >= host_end))
      {
      DBPRT(("%s: case 1: sys outage overlaps all of host outage.\n",
             id));
      DBPRT(("%s:     Discarding host outage (%s)\n", id,
             print_outage(host)));

      /*
       * Discard the per-host outage onto the discard pile.  It
       * may be re-used if a new element is required.  If not, it
       * will be freed at the end of the function.  If the host
       * outage is the first in the list, bump the new list pointer
       * forward, otherwise find the previous element and point over
       * the host element to the next one.
       */

      if (host != new)
        {
        for (outp = new; outp != NULL; outp = outp->next)
          /* Walk list looking for previous pointer. */
          if (outp->next == host)
            break;

        if (outp == NULL)
          {
          DBPRT(("%s: host outage not found!\n", id));
          found = -1;
          break;
          }

        DBPRT(("%s: host outage not head of list.  Skipping from %s\n",

               id, print_outage(outp)));
        outp->next = host->next;
        }
      else
        {
        new = host->next;
        DBPRT(("%s: host outage is head of list.  new head %s\n",
               id, print_outage(new)));
        }

      host->next = discard;

      discard = host;
      DBPRT(("%s: Place host on discard pile %s\n", id,
             print_outage(discard)));

      /* This conflict is now resolved.  Break out of loop. */
      break;
      }

    /* Case 2: 'system' starts before, and ends before, 'host' outage.
     * Adjust the host outage to start just after the system outage ends.
     * The conflict is then resolved.
     */
    if ((sys_beg <= host_beg) && (sys_end < host_end))
      {
      DBPRT(("%s: case 2: sys outage overlaps start of host outage.\n",
             id));

      host->beg_time = sys_end + 1;

      /* Sanity check.  "This can't happen." */

      if (host->end_time <= host->beg_time)
        {
        DBPRT(("%s: host outage %s < 1 second long!", id,
               print_outage(host)));
        found = -1;
        break;
        }

      /* Create new time/date strings for the changed time_t's. */
      make_time_strs(host);

      DBPRT(("%s: host outage bumped to (%s)\n", id, print_outage(host)));

      /* That's all that's required to resolve the conflict. */
      break;
      }

    /* Case 3: 'system' outage starts after, and ends after, 'host' outage.
     * Adjust the host outage to end just before the system outage starts.
     * The conflict is then resolved.
     */
    if ((sys_beg > host_beg) && (sys_end >= host_end))
      {
      DBPRT(("%s: case 3: sys outage overlaps end of host outage.\n",
             id));

      host->end_time = sys_beg - 1;

      /* Sanity check.  "This can't happen." */

      if (host->end_time <= host->beg_time)
        {
        DBPRT(("%s: host outage %s < 1 second long!", id,
               print_outage(host)));
        found = -1;
        break;
        }

      /* Create new time/date strings for the changed time_t's. */
      make_time_strs(host);

      DBPRT(("%s: host outage bumped to (%s)\n", id, print_outage(host)));

      /* That's all that's required to resolve the conflict. */
      break;
      }

    /* Case 4: 'system' outage starts after, and ends before 'host' outage.
     * System outage splits the host outage into two pieces.  Shorten the
     * first piece like case #3 above, and create a new Outage for the
     * remaining chunk.
     */
    if ((sys_beg > host_beg) && (sys_end < host_end))
      {
      DBPRT(("%s: case 4: sys outage overlaps middle of host outage.\n",
             id));

      /* Shorten the original host entry to first chunk of the outage. */
      host->end_time = sys_beg - 1;

      /* Create a new host chunk to follow the system one on the list. */

      if (discard)
        {
        outp = discard;
        discard = discard->next;
        }
      else
        {
        if ((outp = (Outage *)malloc(sizeof(Outage))) == NULL)
          {
          (void)sprintf(log_buffer, "malloc(%d) failed!",
                        (int)sizeof(Outage));
          log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                     log_buffer);
          DBPRT(("%s: %s\n", id, log_buffer));
          found = -1;
          break;
          }
        }

      /* Initialize the new element. */
      memset((void *)outp, 0, sizeof(Outage));

      /* Point to the host's exechost, and fill in the time fields. */
      outp->exechost = host->exechost;

      outp->beg_time = sys_end + 1;

      outp->end_time = host_end;

      /* Now recompute the date and time strings for each outage. */
      make_time_strs(host); /* The original "first" chunk */

      make_time_strs(outp); /* The newly-created 2nd chunk */

      DBPRT(("%s: Split host outage into :\n", id));

      DBPRT(("%s:       host outage (%s)\n", id, print_outage(host)));

      DBPRT(("%s:        sys outage (%s)\n", id, print_outage(sys)));

      DBPRT(("%s:   new host outage (%s)\n", id, print_outage(outp)));

      /* And link the new host outage in after the system outage. */
      outp->next = sys->next;

      sys->next  = outp;

      /* And this case is resolved. */
      break;
      }

    /* Unexpected case!  Return an error! */
    (void)sprintf(log_buffer, "unexpected outage overlap!");

    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    DBPRT(("%s: %s\n", id, log_buffer));

    found = -1;

    break;
    }

  /*
   * Replace the pointer to the head of the list with the head of the new
   * outage list.  The new list may not have the same head as the old one
   * (if the original head was discarded).
   */
  *outlistp = new;

  /* Clean up the discard list, if necessary. */
  if (discard)
    free_outages(discard);

  discard = NULL;

  return (found);
  }

/*
 * Return a pointer to a static buffer containing a string describing the
 * times of the outage.
 */
static char *
print_outage(Outage *outp)
  {
  static char buffer[64];

  sprintf(buffer, "%s %s:%02.2d - %s %s:%02.2d [%s]",
          outp->beg_datestr, outp->beg_timestr, (int)(outp->beg_time % 60),
          outp->end_datestr, outp->end_timestr, (int)(outp->end_time % 60),
          outp->exechost);

  return (buffer);
  }

static int
compare_outages_time(const void *e1, const void *e2)
  {
  Outage  *outage1 = *(Outage **)e1;
  Outage  *outage2 = *(Outage **)e2;
  time_t   diff;

  /* Sort first based on starting time, from soonest to latest. */
  diff = outage1->beg_time - outage2->beg_time;

  if (diff)
    return (diff);

  /* Break ties by sorting by end time.  */
  diff = outage1->end_time - outage2->end_time;

  return (diff);
  }

static int
make_time_strs(Outage *outp)
  {

  struct tm *tm_ptr;

  if ((tm_ptr = localtime(&(outp->beg_time))) == NULL)
    return (1);

  sprintf(outp->beg_datestr, "%2.2d/%2.2d/%4.4d",
          tm_ptr->tm_mon + 1, tm_ptr->tm_mday, tm_ptr->tm_year + 1900);

  sprintf(outp->beg_timestr, "%2.2d:%2.2d",
          tm_ptr->tm_hour, tm_ptr->tm_min);

  if ((tm_ptr = localtime(&(outp->end_time))) == NULL)
    return (1);

  sprintf(outp->end_datestr, "%2.2d/%2.2d/%4.4d",
          tm_ptr->tm_mon + 1, tm_ptr->tm_mday, tm_ptr->tm_year + 1900);

  sprintf(outp->end_timestr, "%2.2d:%2.2d",
          tm_ptr->tm_hour, tm_ptr->tm_min);

  return (0);
  }

/*
 * Free each element of a list of outages.  Note that the 'exechost' field
 * for the outage points back to the OutageList->host, and isn't allocated
 * storage.
 */
static int
free_outages(Outage *outages)
  {
  int n;
  Outage *ptr, *next;

  for (n = 0, ptr = outages; ptr != NULL; ptr = next)
    {
    next = ptr->next;
    free(ptr);
    n++;
    }

  return (n);
  }

/* Parse "MM/DD/YYYY HH:MM" style dates into seconds since the Epoch. */
static int
MMDDYYYY_HHMM(char *MMDDYYYY, char *HHMM)
  {
  /* char   *id = "MMDDYYYY_HHMM"; */

  struct tm junk;
  char    copy[MAX_TXT + 1];

  strcpy(copy, MMDDYYYY); /* Copy it so we can modify the string. */
  copy[2] = copy[5] = '\0'; /* 'MM/DD/YYYY\0' ==> 'MM\0DD\0YYYY\0' */

  junk.tm_mon  = atoi(copy);
  junk.tm_mday = atoi(copy + 3);
  junk.tm_year = atoi(copy + 6);

  junk.tm_mon  -= 1;  /* mktime() counts from 0, not 1 like us */
  junk.tm_year -= 1900; /* mktime() counts from 1900 */

  strcpy(copy, HHMM);  /* Copy it so we can modify the string. */
  copy[2] = '\0';  /* 'HH:MM\0' ==> 'HH\0MM\0' */
  junk.tm_hour = atoi(copy);
  junk.tm_min = atoi(copy + 3);
  junk.tm_sec = 0;

  junk.tm_isdst = -1;  /* Choose correct DST value from timezone */

  return (int)mktime(&junk);
  }

