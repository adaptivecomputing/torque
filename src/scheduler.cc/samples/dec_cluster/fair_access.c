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
 * implementing a per-queue per-user Maximum Running Jobs Limit, as well as
 * as a per-queue per-user TimeCredit (the maximum amount of remaining cputime
 * that a given user can have outstanding at any given time.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* PBS header files */

#include "pbs_error.h"
#include "pbs_ifl.h"
#include "log.h"

/* Scheduler header files */

#include "toolkit.h"
#include "gblxvars.h"
#include "msgs.h"


/* extract and store the Fair Access Directives from a line just read from
 * the scheduler's configuration file.
 */
int
arg_to_fairacl(char *arg, char *sep, FairAccessList **fairacl_ptr)
  {
  int     num = 0, max_A = 0, max_B = 0, fieldcnt, found;
  char   *id = "arg_to_fairacl";
  char   *field, aclname[30], name[30];
  AccessEntry *new_ae, *FAptr = NULL, *fptr = NULL;
  FairACL *aptr = NULL, *new_acl = NULL;

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

    if (fieldcnt == 1 && /* first field on FAIR_ACCESS line */
        (strcmp(field, "QUEUE")) &&
        (strcmp(field, "ARCH")))
      {
      sprintf(log_buffer,
              "Unrecognized FAIR_ACCESS directive: %s", field);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      return(-1);
      }
    else if (fieldcnt == 2)
      strcpy(aclname, field);

    else if (fieldcnt == 3)
      strcpy(name, field);

    else if (fieldcnt == 4)
      max_A = atoi(field);

    else if (fieldcnt == 5)
      max_B = atoi(field);
    }

  if (fieldcnt != 5)
    {
    sprintf(log_buffer,
            "Incomplete FAIR_ACCESS directive: %s", arg);
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

  /* if we found the entry, then we need to add to its acl;
   * otherwise, we first need to create a new entry struct,
   * and then add to its acl...
   */

  if (!found)
    {
    new_ae = (AccessEntry *)malloc(sizeof(AccessEntry));

    if (new_ae == NULL)
      {
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                 "malloc(newAccessEntry)");
      return(-1);
      }

    memset(new_ae, 0, sizeof(AccessEntry));

    if (FAptr == NULL)
      {
      (*fairacl_ptr)->entry = new_ae;
      fptr = (*fairacl_ptr)->entry;
      }
    else
      {
      fptr->next = new_ae;
      fptr = new_ae;
      }

    new_ae->name = schd_strdup(aclname);
    }

  aptr = fptr->list;

  if (aptr) {  /* Walk the list, looking for last element. */
    ;

    while (aptr->next != NULL)
      aptr = aptr->next;
    }

  new_acl = (FairACL *)malloc(sizeof(FairACL));

  if (new_acl == NULL)
    {
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
               "malloc(newEntry->list)");
    return(-1);
    }

  memset(new_acl, 0, sizeof(FairACL));

  if (aptr == NULL)
    {
    fptr->list = new_acl;
    aptr = fptr->list;
    }
  else
    {
    aptr->next = new_acl;
    aptr = new_acl;
    }

  /* Finially we get to fill the new acl with the info we saved earlier */
  strcpy(aptr->name, name);

  aptr->max_rjobs   = max_A;

  aptr->max_minutes = max_B;

  return (num);
  }

void schd_print_fairacl(void)
  {
  char      *id = "print_fairacl";
  FairAccessList *FALptr;
  AccessEntry    *AEptr;
  FairACL        *aptr;

  if (!schd_FairACL)
    return;

  sprintf(log_buffer, "FAIR_ACCESS: QUEUE Directives:");

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

  sprintf(log_buffer, "  %-12s %-12s %-10s %-10s",
          "QueueName", "UserName", "Max-R-Jobs", "TimeCredit");

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

  for (FALptr = schd_FairACL; FALptr != NULL; FALptr = FALptr->next)
    {
    for (AEptr = FALptr->entry; AEptr != NULL; AEptr = AEptr->next)
      {
      if (AEptr->name)
        {
        for (aptr = AEptr->list; aptr != NULL; aptr = aptr->next)
          {
          sprintf(log_buffer, "  %-12s %-12s %5.0d jobs %5.0d mins",
                  AEptr->name, aptr->name, aptr->max_rjobs,
                  aptr->max_minutes);
          log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                     log_buffer);
          }
        }
      }
    }

  return;
  }

/* Given a username and queue name, return the maximum number of
 * simutaniously running jobs for this user, or the default for
 * this queue if username is not on ACL, or the default for the
 * system, if no default for this queue.  In the second argument,
 * return the Time Credit Minutes (same options for default apply).
 */
int schd_user_accesslimits(char *user, char *qname, int *maxjobs, int *maxtime)
  {
  FairAccessList *FALptr;
  AccessEntry    *AEptr;
  FairACL        *aptr;
  int      default_jobs = 0, default_time = 0;

  for (FALptr = schd_FairACL; FALptr != NULL; FALptr = FALptr->next)
    {
    for (AEptr = FALptr->entry; AEptr != NULL;  AEptr = AEptr->next)
      {
      if (AEptr->name == NULL)
        continue;

      if (!strcmp(AEptr->name, qname))
        {
        /* requested queue, is the user listed? */
        for (aptr = AEptr->list; aptr != NULL; aptr = aptr->next)
          {
          if (!strcmp(aptr->name, user))
            {
            *maxjobs = aptr->max_rjobs;
            *maxtime = aptr->max_minutes;
            goto done_limits;
            }
          }

        /* user was not on ACL, see if there's a default */
        for (aptr = AEptr->list; aptr != NULL; aptr = aptr->next)
          {
          if (!strcmp(aptr->name, "default"))
            {
            *maxjobs = aptr->max_rjobs;
            *maxtime = aptr->max_minutes;
            goto done_limits;
            }
          }
        }

      /* If we encounter a system default, save it in case we need it */
      if (!strcmp(AEptr->name, "default"))
        {
        for (aptr = AEptr->list; aptr != NULL; aptr = aptr->next)
          {
          if (!strcmp(aptr->name, "default"))
            {
            default_jobs = aptr->max_rjobs;
            default_time = aptr->max_rjobs;
            break;
            }
          }
        }
      }
    }

  /* if we get here all we can do is return the system default, if
   * there is one. If there isn't, the the return value is 0.
   */
  *maxjobs = default_jobs;

  *maxtime = default_time;

done_limits:

  return (0);
  }

int schd_job_exceeds_fairaccess(Job *job, Queue *queue, char *reason)
  {
  int i, maxjobs, maxtime;

  schd_user_accesslimits(job->owner, queue->qname, &maxjobs, &maxtime);
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
  else
    return(0);  /* job is within limits, go ahead */

  }
