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
 * This file contains routines for support Fair Access to system, including
 * implementing a per-queue Shares or percentages of system resources, and
 * Maximum Running Jobs Limit
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Scheduler header files */
#include "toolkit.h"
#include "gblxvars.h"
#include "msgs.h"

/* PBS header files */
#include "pbs_error.h"
#include "log.h"


/* extract and store the Fair Access Directives from a line just read from
 * the scheduler's configuration file.
 */
int
arg_to_fairshare(char *arg, char *sep, FairAccessList **fairacl_ptr)
  {
  int     num = 0, max_A = 0, max_B = 0, fieldcnt, found;
  char   *id = "arg_to_fairshare";
  char   *field, aclname[30];
  AccessEntry *new, *FAptr = NULL, *fptr = NULL;

  /*
   * Multiple lines may be used to add entries to the FairACL list. Find
   * the tail of the passed-in list (if there is one), and assign the
   * FAptr to the tail element.  Later, the new element will be hung off
   * FAptr's next field (or FAptr will be set to it.)
   */

  if (*fairacl_ptr == NULL)
    {
    *fairacl_ptr = (FairAccessList *)malloc(sizeof(FairAccessList));
    (*fairacl_ptr)->next = NULL;
    (*fairacl_ptr)->entry = NULL;
    }

  FAptr = (*fairacl_ptr)->entry;

  /* first we process the configuration line passed in to use, saving
   * the important bits for later; at this point we don't know if we
   * have a new queue entry, or simply an addition to an existing
   * queue's FairAccess list.
   */
  fieldcnt = 0;

  for (field = strtok(arg, sep); field != NULL; field = strtok(NULL, sep))
    {
    fieldcnt++;

    if (fieldcnt == 1 && /* first field on FAIR_SHARE line */
        (strcmp(field, "QUEUE")))
      {
      sprintf(log_buffer,
              "Unrecognized FAIR_SHARE directive: %s", field);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      return(-1);
      }
    else if (fieldcnt == 2)  /* Queue name */
      strcpy(aclname, field);

    else if (fieldcnt == 3)  /* Queue max shares (%) */
      max_A = atoi(field);

    else if (fieldcnt == 4)  /* Queue max running jobs (% CPUs) */
      max_B = atoi(field);
    }

  if (fieldcnt != 4)
    {
    sprintf(log_buffer,
            "Incomplete FAIR_SHARE directive: %s", arg);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    return(-1);
    }

  if (max_A < 0 || max_A > 100)
    {
    sprintf(log_buffer,
            "FAIR_SHARE share percentage (%d) should be between 1 and 100",
            max_A);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    return(-1);
    }

  if (max_B < 0 || max_B > 100)
    {
    sprintf(log_buffer,
            "FAIR_SHARE running job percentage (%d) should be between 1 and 100",
            max_B);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    return(-1);
    }

  found = 0;

  if (FAptr)
    {
    /* search for an existing entry for this queue  */
    for (fptr = FAptr; fptr != NULL; fptr = fptr->next)
      {
      if (!strcmp(fptr->name, aclname))
        {
        found = 1;
        break; /* need to add a new ACL on the entry */
        }
      }

    if (!found)
      {
      for (fptr = FAptr; fptr->next != NULL; fptr = fptr->next)
        /* Walk the list, looking for last element. */;
      }
    }

  /* if we found the entry, then we need add the new info to it;
   * otherwise, we first need to create a new entry struct, and
   * then add to it...
   */

  if (!found)
    {
    new = (AccessEntry *)malloc(sizeof(AccessEntry));

    if (new == NULL)
      {
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                 "malloc(newAccessEntry)");
      return(-1);
      }

    memset(new, 0, sizeof(AccessEntry));

    if (FAptr == NULL)
      {
      (*fairacl_ptr)->entry = new;
      fptr = (*fairacl_ptr)->entry;
      }
    else
      {
      fptr->next = new;
      fptr = new;
      }

    new->name = schd_strdup(aclname);
    }

  /* Finially we get to fill it in with the info we saved earlier */
  new->past_ndays   = 0;

  new->past_percent = 0.0;

  new->max_percent  = (double)max_A;

  new->max_running  = max_B;

  new->today_max    = 0.0;

  new->today_usage  = 0.0;

  new->default_mem  = (size_t)0;

  return (num);
  }

void schd_print_fairshare(void)
  {
  char      *id = "print_fairacl";
  FairAccessList *FALptr;
  AccessEntry    *AEptr;

  if (!schd_FairACL)
    return;

  sprintf(log_buffer, "FAIR_SHARE: QUEUE Directives:");

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

  sprintf(log_buffer, "  %-12s %-14s", "QueueName", "MaxPercentage");

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

  for (FALptr = schd_FairACL; FALptr != NULL; FALptr = FALptr->next)
    {
    for (AEptr = FALptr->entry; AEptr != NULL; AEptr = AEptr->next)
      {
      if (AEptr->name)
        {
        sprintf(log_buffer, "  %-12s %5.0d",
                AEptr->name, AEptr->max_percent);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   log_buffer);
        }
      }
    }

  return;
  }

/* Given a queue name, return the maximum number of simutaniously running
 * jobs for this queue. In the second argument, return the Max Shares allowed
 * (percentage of system resources).
 */
int schd_accesslimits(char *qname, int *maxjobs, int *maxtime)
  {
  FairAccessList *FALptr;
  AccessEntry    *AEptr;
  int      default_jobs = 0, default_time = 0;

  for (FALptr = schd_FairACL; FALptr != NULL; FALptr = FALptr->next)
    {
    for (AEptr = FALptr->entry; AEptr != NULL;  AEptr = AEptr->next)
      {
      if (AEptr->name == NULL)
        continue;

      if (!strcmp(AEptr->name, qname))
        {
        *maxjobs = AEptr->max_running * schd_MAX_NCPUS / 100;
        return(0);
        }
      }
    }

  /* if we get here all we can do is return the value 0.  */
  *maxjobs = default_jobs;

  *maxtime = default_time;

  return (0);
  }

int schd_job_exceeds_fairshare(Job *job, Queue *queue, char *reason)
  {

#if 0
  /* for now, just return OK, since these are soft limits */

  int i, maxjobs, maxtime;

  schd_accesslimits(queue->qname, &maxjobs, &maxtime);
  maxtime *= 60;  /* convert minutes to seconds */

  /* Now, walk the list of running jobs and record each user's count. */

  for (i = 0; i < nUsers; ++i)
    {
    if (!strcmp(job->owner, Users[i].name))
      {
      break;
      }
    }

  if (job->walltime + Users[i].remaining_time > maxtime)
    {
    sprintf(reason, "Would exceed aggregrate runtime limit (%d mins)",
            maxtime / 60);
    return(1);  /* would cause user to exceed limits */

    }
  else if (Users[i].running_jobs == maxjobs)
    {
    sprintf(reason, "Maximum running-jobs limit (%d) reached", maxjobs);
    return(1);  /* would cause user to exceed limits */
    }

#endif

  return(0);  /* job is within limits, go ahead */
  }

/* Return the percentage of system resources (shares) used by AE
 * so far today.
 */
double percent_shares_today(AccessEntry *AE)
  {
  if (AE->today_max < 1)
    return (0.0);

  return (AE->today_usage / AE->today_max * 100.0);
  }

/* Return the percentage of system resources (shares) used by AE
 * historically (as far back as we have been keeping track).
 */
double percent_shares_past(AccessEntry *AE)
  {
  return ((AE->past_percent * AE->past_ndays + percent_shares_today(AE)) /
          (AE->past_ndays + 1));
  }

void update_share_usage(AccessEntry *AE)
  {
  if (AE->past_percent < 1)
    {
    AE->past_percent = percent_shares_today(AE);
    AE->past_ndays   = 1;
    }
  else
    {
    AE->past_percent = percent_shares_past(AE);
    AE->past_ndays++;
    }

  AE->today_usage = 0.0;

  AE->today_max   = 0.0;
  }
