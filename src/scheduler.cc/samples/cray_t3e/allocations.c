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
 * This file contains routines pulled over from the parallel systems'
 * PBS toolkit.
 *
 * Authors include: James Patton Jones, NAS/NASA Ames
 *                  Dr. Ed Hook, NASA LaRC
 *                  Chris Batten, NASA LaRC
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pbs_error.h"
#include "pbs_ifl.h"
#include "log.h"

#include "toolkit.h"
#include "gblxvars.h"

extern int connector;

/* Misc Other Global Variables */
int     schd_NeedToGetAllocInfo;
int     schd_NeedToGetYTDInfo;
int     schd_NumAllocation;

Alloc_Group schd_GroupTable[MAX_GROUP];

static int cmp_gname(const void *a, const void *b);
static int get_FY_used(char *gname, float *used);
static int get_allocation(char *gname, float *alloc);
static char *trim_whitespace(char *string);
/*
 * Read allocation and usage data for current fiscal year
 *
 * Allocation database is a flat file that contains lines like:
 *
 *    P= XXX:Evolution of FOO for XXX Applications = 1234 = 0.0 = 50.0 = 0
 */


void
schd_alloc_info(void)
  {
  char   *id = "schd_alloc_info";
  char   *syntax = "Syntax error in line %d of %s.  Skipping...";
  int     i;
  int     linenum, jobs, ret;
  int     count = 0;
  char    buffer[MAX_TXT * 2];
  char    gname[MAX_USER_NAME_SIZE];
  char    uid[MAX_TXT + 1];
  char    gid[MAX_TXT + 1];
  char    used[MAX_TXT + 1];
  char    alloc_line[MAX_TXT + 1];
  FILE   *fp;
  char   *p, *end;
  float   nodes_used, pct;
  float   alloc;
  int     table_modified = 0;

  if (!schd_NeedToGetAllocInfo)
    goto get_current;

  /* Initialize the allocation group table(s) */
  for (i = 0; i < MAX_GROUP; ++i)
    {
    schd_GroupTable[i].allocation = 0.0;
    strcpy(schd_GroupTable[i].gname, "");
    }

  if ((fp = fopen(schd_AllocFilename, "r")) == NULL)
    {
    (void)sprintf(log_buffer,
                  "Warning: group allocation data (%s) unreadable. Skipping...",
                  schd_AllocFilename);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }
  else
    {
    linenum = 0;

    while (fgets(buffer, sizeof(buffer), fp))
      {

      linenum ++;
      p = strtok(buffer, "="); /* p --> 'P'   */

      if ((strcmp(p, "P") != 0) || (p == NULL))
        {
        (void)sprintf(log_buffer, syntax, linenum, schd_AllocFilename);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                   id, log_buffer);
        DBPRT(("%s: %s\n", id, log_buffer));
        continue;
        }

      p = strtok(NULL, "="); /* p --> ' <description> ' */

      if (p == NULL)
        {
        (void)sprintf(log_buffer, syntax, linenum, schd_AllocFilename);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                   id, log_buffer);
        DBPRT(("%s: %s\n", id, log_buffer));
        continue;
        }

      p = strtok(NULL, "="); /* p --> '<group>'  */

      if (p == NULL)
        {
        (void)sprintf(log_buffer, syntax, linenum, schd_AllocFilename);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                   id, log_buffer);
        DBPRT(("%s: %s\n", id, log_buffer));
        continue;
        }

      p = trim_whitespace(p);

      if (isdigit((int)*p)) /* isdigit() wants an int, not char */
        sprintf(gname, "g%s", p); /* 'g7989' */
      else
        sprintf(gname, "%s", p); /* 'groupname' */

      p = strtok(NULL, "=");  /* p --> '<allocation>' */

      if (p == NULL)
        {
        (void)sprintf(log_buffer, syntax, linenum, schd_AllocFilename);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                   id, log_buffer);
        DBPRT(("%s: %s\n", id, log_buffer));
        continue;
        }

#ifdef INCLUDE_DEAD_CODE
      p = trim_whitespace(p);

      if (!strcmp(schd_SYSTEM_NAME, "babbage"))
        p = strtok(NULL, "=");

      if (!strcmp(schd_SYSTEM_NAME, "poseidon"))
        p = strtok(NULL, "=");

      if (p == NULL)
        {
        (void)sprintf(log_buffer, syntax, linenum, schd_AllocFilename);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                   id, log_buffer);
        DBPRT(("%s: %s\n", id, log_buffer));
        continue;
        }

#endif /* INCLUDE_DEAD_CODE */

      p = trim_whitespace(p);

      /* Do more robust error checking of the allocations value. */
      alloc = strtod(p, &end);

      if (*end != '\0')
        {
        (void)sprintf(log_buffer, syntax, linenum, schd_AllocFilename);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                   id, log_buffer);
        DBPRT(("%s: %s\n", id, log_buffer));
        continue;
        }

      strcpy(schd_GroupTable[count].gname, gname);

      schd_GroupTable[count].allocation = alloc;
      ++count;

      if (count >= MAX_GROUP)
        {
        (void)sprintf(log_buffer, "WARNING: Too much allocation data.  "
                      "Only using first %d lines of %s.\n",
                      count, schd_AllocFilename);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   log_buffer);
        break;
        }
      }

    fclose(fp);
    }

  schd_NumAllocation = count;

  qsort(schd_GroupTable, count, sizeof(Alloc_Group), cmp_gname);

  schd_NeedToGetAllocInfo = 0;
  schd_NeedToGetYTDInfo = 1; /* necessary */
  table_modified = 1;

get_current:

  if (!schd_NeedToGetYTDInfo)
    goto skip_ytd_info;

  /* Initialize the usage count in the table */
  for (i = 0; i < schd_NumAllocation; ++i)
    schd_GroupTable[i].total_usage = 0.0;

  if ((fp = fopen(schd_CurrentFilename, "r")) == NULL)
    {
    (void)sprintf(log_buffer,
                  "Error: %s: unable to open %s for read.", id,
                  schd_CurrentFilename);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }
  else
    {
    linenum = 0;

    while (fgets(buffer, sizeof buffer, fp))
      {

      ++linenum;

      ret = sscanf(buffer, "%s %s %d %f",
                   uid,  /* user name (string) */
                   gid,  /* group name (string) */
                   &jobs,  /* nbr of jobs */
                   &nodes_used); /* node-hours */

      if (ret != 4)   /* oops */
        {
        (void)sprintf(log_buffer, syntax, linenum, schd_CurrentFilename);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   log_buffer);
        continue;
        }

      /* XXX check ranges (esp. signed) on jobs and cpu */

      /* Find data for this group in our table */
      for (i = 0; i < schd_NumAllocation; ++i)
        if (!strcmp(schd_GroupTable[i].gname, gid))
          break;

      if (i == schd_NumAllocation)   /* Group not found in table */
        {
        (void)sprintf(log_buffer,
                      "Warning: no allocation for %s", gid);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   log_buffer);
        continue;
        }

      /* Add group's "current" usage into the table. */
      schd_GroupTable[i].total_usage += nodes_used;
      }

    fclose(fp);
    }

  schd_NeedToGetYTDInfo = 0;

  table_modified = 1;

skip_ytd_info:

  if (table_modified)
    {
    for (i = 0; i < schd_NumAllocation; i++)
      {
      alloc = schd_GroupTable[i].allocation;

      if (alloc > 0.0)
        {
        /* Calculate percentage of allocation used at this point. */
        pct = schd_GroupTable[i].total_usage;
        pct /= alloc;
        pct *= 100.0;

        sprintf(used, "%3.2f %%", pct);
        sprintf(alloc_line, "%.2f", alloc);
        }
      else
        {
        used[0] = '\0';

        if (alloc < 0.0)
          strcpy(alloc_line, "N/A");
        else
          strcpy(alloc_line, "0.00");
        }

      (void)sprintf(log_buffer,
                    "%s: %-8s Used: %16.4f Allocation: %10s  %s",
                    id, schd_GroupTable[i].gname,
                    schd_GroupTable[i].total_usage,
                    alloc_line, used);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      }
    }
  }

static int
cmp_gname(const void *a, const void *b)
  {
  Alloc_Group *alloc_group_a;
  Alloc_Group *alloc_group_b;

  alloc_group_a = (Alloc_Group *)a;
  alloc_group_b = (Alloc_Group *)b;

  return strcmp(alloc_group_a->gname, alloc_group_b->gname);
  }

/*
 * Check if group has exceeded their allocation.  If the group has no
 * allocations, refuse the job.  If they have an unlimited (negative)
 * allocation, then they have used 0% of their allocations.  Otherwise,
 * return either a '0' -- the group is under allocation, or the integer
 * percentage they've used so far (i.e. 107 for 107% usage).
 */
int
schd_is_over_alloc(char *group)
  {
  int     percent_used;
  float   alloc, used;

  /* If the group isn't found in the allocations file, reject the job. */

  if (!get_allocation(group, &alloc))
    return (100);  /* Used 100% of the non-existent allocation. */

  /* Negative allocations mean "unlimited usage".  Always okay. */
  if (alloc < 0.0)
    return (0);

  /*
   * Compute percentage of their allocation that has been used this FY.
   * If no record is found for usage, they have a zero usage.
   */
  if (!get_FY_used(group, &used))
    used = 0.0;

  percent_used = (int)((used / alloc * 100.0) + 0.5);

  if (percent_used < 100)
    return (0);  /* Are not over allocation. */
  else
    return (percent_used); /* Return percent allocation used (100+%) */
  }

/*
 * The group to which this job belongs is over their allocation.  Send
 * the owner of the job a nastygram and ask the JMS to delete the job.
 */
int
schd_reject_over_alloc(Job *job)
  {
  char   *id = "schd_reject_over_alloc";
  char    buffer[4 * MAX_TXT + 1];
  char   *unknown = "???";
  float   alloc, used;
  char   *group;
  int    rc = 0;

  char   *nastygram = "Allocation Exceeded.\n"
                      "\n"
                      "Your PBS job %s would exceed the group\n"
                      "or project allocation given for this operational year.\n"
                      "\n"
                      "This limit has been imposed on you because your group (%s)\n"
                      "has used %3.2f node-hours of its allocation of %3.2f node-hours.\n"
                      "\n"
                      "Please contact the Principal Investigator (PI) for your group\n"
                      "for additional information. The PI should contact their HPCCP\n"
                      "Resource Monitor to apply for an allocation increase.\n"
                      "\n";

  /* Get the group's allocation and usage statistics. */
  group = (job->group != NULL) ? job->group : unknown;

  /*
   * Get the current allocation for this group.  If the group does not
   * have an allocation entry, assume it's zero.
   */

  if (!get_allocation(group, &alloc))
    alloc = 0.0;

  /*
   * Get the current usage for this group.  If the group does not have a
   * current usage entry, assume it's zero -- a job may never have been
   * run by this group, if the allocation is 0.0 (not allowed).
   */
  if (!get_FY_used(group, &used))
    used = 0.0;

  /* This should not be called if the group is below its allocation. */
  if (used < alloc)
    {
    (void)sprintf(log_buffer, "group %s (job %s) below allocation ???",
                  group, job->jobid);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    return (1);
    }

  /*
   * User has used up allocation, so we get to delete this job. But we
   * need to log the deletion, and notify the user via email why the job
   * was terminated.  Don't perform the deletion if this is a remote job.
   */
  (void)sprintf(log_buffer,
                "rejecting %s because group %s over allocation (%3.1f/%3.1f)",
                job->jobid, group, used, alloc);

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

  DBPRT(("%s: %s\n", id, log_buffer));

  sprintf(buffer, nastygram, job->jobid, group, used, alloc);

  /*
   * Since the job cannot run inside allocations, delete it.  The JMS
   * will deliver the notification (the string in 'buffer') to the user.
   */
  if (schd_reject_job(job, buffer))
    {
    (void)sprintf(log_buffer, "schd_reject_job failed: %d", rc);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

  return (0);
  }

/*
 * Look for the total_usage for the requested group.  If not found, return
 * zero.  Otherwise, put the usage into *used, and return 1.
 */
static int
get_FY_used(char *gname, float *used)
  {
  int     i;

  for (i = 0; i < schd_NumAllocation; i++)
    /* If the right group is found, return the total usage. */
    if (!strcmp(schd_GroupTable[i].gname, gname))
      {
      *used = schd_GroupTable[i].total_usage;
      return (1);
      }

  return (0);  /* Requested group not found. */
  }

/*
 * Look for the allocations for the requested group.  If not found, return
 * zero.  Otherwise, put the allocation into *alloc, and return 1.
 */
static int
get_allocation(char *gname, float *alloc)
  {
  int     i = 0;

  for (i = 0; i < schd_NumAllocation; i++)
    /* If the right group is found, return the allocation. */
    if (!strcmp(schd_GroupTable[i].gname, gname))
      {
      *alloc = schd_GroupTable[i].allocation;
      return (1);
      }

  return (0);  /* Requested group not found. */
  }

/*
 * Remove leading and trailing whitespace from a string.  Returns a pointer
 * to the first non-whitespace character in the string and replaces the first
 * trailing space with a '\0'.  Returns an empty string (*p = '\0') if all
 * whitespace.
 */
static char *
trim_whitespace(char *string)
  {
  int     i;

  /* Trim off any whitespace on the end of the string. */

  for (i = (int)strlen(string) - 1; i >= 0 && isspace((int)string[i]); i--)
    string[i] = '\0';

  /* Find first non-whitespace character in the string. */
  for (i = 0; string[i] != '\0'; i++)
    if (!isspace((int)string[i]))
      break;;

  return &string[i];
  }
