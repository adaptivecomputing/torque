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
 * There are two routines here.  The first, queue_limits(), evaluates the
 * limits of a queue to see if the queue can support running another job.
 * The other, user_limits(), evaluates a job against any imposed user
 * limit for the specified queue.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/param.h>
#if defined(sgi)
#include <sys/sysmacros.h>
#endif
#include <string.h>

/* Scheduler header files */
#include "toolkit.h"
#include "gblxvars.h"
#include "msgs.h"

/* PBS header files */
#include "pbs_error.h"
#include "log.h"

extern int connector;

static int user_running(Queue *queue, char *user);

/*
 * Given a user batch request and a queue name, evaluate the job against
 * limits retrieved from the queue_limits() routine.
 */
int
schd_user_limits(Job *job, Queue *queue, char *reason)
  {
  /* char   *id = "schd_user_limits"; */
  int     jobs_running;

  /*
   * If there is a user run limit, make sure the user is under their limit.
   */

  if ((queue->userrun != UNSPECIFIED) && queue->userrun)
    {
    jobs_running = user_running(queue, job->owner);

    /* See if run limit will be exceeded */

    if ((jobs_running != UNSPECIFIED) &&
        (jobs_running >= queue->userrun))
      {
      if (reason)
        sprintf(reason, "Exceeds queue %s userrun limit (%d)",
                queue->qname, queue->userrun);

      return (-1);
      }
    }

  /* If the queue has a maximum CPU limit, enforce it. */
  if ((queue->ncpus_max != UNSPECIFIED) && queue->ncpus_max)
    {

    /* Will this job exceed the queue CPU limit? */
    if ((job->ncpus + queue->ncpus_assn) > queue->ncpus_max)
      {
      if (reason)
        sprintf(reason, "Exceeds queue %s CPU limit (%d)",
                queue->qname, queue->ncpus_max);

      return (-1);
      }
    }

  /* If the queue has a maximum memory limit, enforce it. */
  if ((queue->mem_max != UNSPECIFIED) && queue->mem_max)
    {

    /* Will this job exceed the queue memory limit? */
    if ((job->memory + queue->mem_assn) > queue->mem_max)
      {
      if (reason)
        sprintf(reason, "Exceeds queue %s memory limit (%ul)",
                queue->qname, (unsigned long)queue->mem_max);

      return (-1);
      }
    }

  /* If the job meets all the criteria above, it is runnable. */
  return (0);
  }

static int
user_running(Queue *queue, char *user)
  {
  /*char   *id = "user_running";*/
  Job    *job;
  int     jobs_running;

  DBPRT(("%s(%s, %s)\n", id, user, queue->qname));

  jobs_running = 0;

  /*
   * Count the jobs in the queue's job list that are running and owned
   * by this user.
   */

  for (job = queue->jobs; job != NULL; job = job->next)
    {
    if ((job->state != 'R') && (!strcmp(job->owner, user)))
      jobs_running++;
    }

  DBPRT(("    user %s has %d job%s running in queue '%s'\n",

         user, jobs_running, jobs_running == 1 ? "" : "s", queue->qname));

  return (jobs_running);
  }

/* given the stated cpu and memory usage, *could* this job run ?
 */
int schd_job_could_run(int now_cpu, int now_mem, int now_run,
                       size_t arch_mem, int arch_cpu, Job *job, Queue *queue, char *reason)
  {
  int ret = 0;
  char *id = "job_could_run";

  /* check to see if running this job would... */

  /* ...exceed the global per-group architecture CPU/Memory limits */
  ret = job_exceeds_accesslimits(job, queue->rsrcs->arch, arch_mem, arch_cpu);

  if (ret)
    {
    if (ret == 3)
      sprintf(reason, "Exceeds arch (%s) group MEM limit",
              queue->rsrcs->arch);

    if (ret == 2)
      sprintf(reason, "Exceeds arch (%s) group CPU limit",
              queue->rsrcs->arch);

    if (ret == 1)
      sprintf(reason, "Exceeds arch (%s) group limit",
              queue->rsrcs->arch);

    return (0);
    }

  /* ...exceed the queue's max run limit?  */
  if ((queue->maxrun != UNSPECIFIED) && (now_run + 1 > queue->maxrun))
    {
    sprintf(reason, "Queue %s run limit (%d) reached",
            queue->qname, queue->maxrun);
    return (0);
    }

  /* ...exceed the queue's ncpu limit?  */
  if ((queue->ncpus_max != UNSPECIFIED) &&
      (now_cpu + job->ncpus > queue->ncpus_max))
    {
    sprintf(reason, "Queue %s CPU limit (%d) reached", queue->qname,
            queue->ncpus_max);
    return (0);
    }

  /* ...exceed the queue's memory limit?  */
  if ((queue->mem_max != UNSPECIFIED) &&
      (now_mem + job->memory > queue->mem_max))
    {
    sprintf(reason, "Queue %s Memory limit", queue->qname);
    return (0);
    }

  /* ...exceed the queue's total Ncpus?  */
  if (now_cpu + job->ncpus > queue->rsrcs->ncpus_total)
    {
    sprintf(reason, "%s (%d max)", schd_JobMsg[NO_NODES],
            queue->rsrcs->ncpus_total);
    return (0);
    }

  /* ...exceed the queue's total Memory?  */
  if (now_mem + job->memory > queue->rsrcs->mem_total)
    {
    sprintf(reason, schd_JobMsg[NO_FREE_MEM]);
    return (0);
    }

  return(1);
  }
