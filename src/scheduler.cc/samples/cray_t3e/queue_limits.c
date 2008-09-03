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
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysmacros.h>
#include <string.h>

/* PBS header files */

#include "pbs_error.h"
#include "pbs_ifl.h"
#include "log.h"

/* Scheduler header files */

#include "toolkit.h"
#include "gblxvars.h"

/*
 * Determine if a job *can* run in this queue.  This is distinct from if
 * it *should* be run in the queue.
 *
 * A job *can* fit in a queue if its requested resources are not greater
 * than the queue's maximums.
 *
 * A job *should* be run only if its requested resources do not exceed the
 * queue's *available* resources.
 */
int
schd_job_fits_queue(Job *job, Queue *queue, char *reason)
  {
  /* char   *id = "schd_job_fits_queue"; */
  int     priority;

  /* Some of the limits are not tested for priority jobs.
   */
  priority = job->flags & JFLAGS_PRIORITY;

  /*
   * Compare the job's requested resources against the queue's limits.
   */

  if (!priority && (queue->wallt_min != UNSPECIFIED) &&
      (job->walltime < queue->wallt_min))
    {
    if (reason)
      (void)sprintf(reason,
                    "Does not meet queue '%s' walltime minimum (%s).",
                    queue->qname, schd_sec2val(queue->wallt_min));

    return (0);
    }

  if (!priority && (queue->wallt_max != UNSPECIFIED) &&
      (job->walltime > queue->wallt_max))
    {
    if (reason)
      (void)sprintf(reason,
                    "Would exceed queue '%s' walltime limit (%s).", queue->qname,
                    schd_sec2val(queue->wallt_max));

    return (0);
    }

  if (!priority && (queue->nodes_min != UNSPECIFIED) &&
      (job->nodes < queue->nodes_min))
    {
    if (reason)
      (void)sprintf(reason,
                    "Does not meet queue '%s' node minimum (%d).", queue->qname,
                    queue->nodes_min);

    return (0);
    }

  if (!priority && (queue->nodes_max != UNSPECIFIED) &&
      (job->nodes > queue->nodes_max))
    {
    if (reason)
      (void)sprintf(reason, "Would exceed queue '%s' node limit (%d).",
                    queue->qname, queue->nodes_max);

    return (0);
    }

  /*
   * The job _can_ fit in this queue.  This doesn't mean it *will* fit
   * in the queue as it currently exists, but it *would* fit if the queue
   * was completely empty.
   */
  return (1);
  }

int
schd_job_can_queue(Job *job)
  {
  QueueList *qptr;

  /* No?  Try the batch queues, if any. */

  if (schd_BatchQueues)
    {
    for (qptr = schd_BatchQueues; qptr != NULL; qptr = qptr->next)
      {
      if (schd_job_fits_queue(job, qptr->queue, NULL))
        return (1);
      }
    }

  /*
   * No queues were found that could run this job.  Reject it, since it
   * will never be able to run.
   */

  return (0);
  }

/*
 * Check queue limits to see if a job should be run out of this queue.
 */
int
schd_check_queue_limits(Queue *queue, char *reason)
  {
  /* char   *id = "schd_check_queue_limits"; */

  /* Is the queue enabled?  If not, note it and fail. */
  if (queue->flags & QFLAGS_DISABLED)
    {
    if (reason)
      sprintf(reason, "Queue %s not enabled", queue->qname);

    return (-1);
    }

  /* Is the queue started?  If not, note it and fail. */
  if (queue->flags & QFLAGS_STOPPED)
    {
    if (reason)
      sprintf(reason, "Queue %s not started", queue->qname);

    return (-1);
    }

  /* Is the queue below the run limit?  If so, note it and fail. */
  if ((queue->maxrun != UNSPECIFIED) &&
      (queue->running >= queue->maxrun))
    {
    if (reason)
      sprintf(reason, "Queue %s run limit (%d) reached", queue->qname,
              queue->maxrun);

    /*
     * Assert the MAXRUN bit, since the queue is essentially full.
     */
    queue->flags |= QFLAGS_MAXRUN;

    return (-1);
    }

  /* Has the queue nodes limit been reached?  If so, note it and fail. */
  if ((queue->nodes_max != UNSPECIFIED) &&
      (queue->nodes_assn >= queue->nodes_max))
    {
    if (reason)
      sprintf(reason, "Queue %s node limit (%d) reached", queue->qname,
              queue->nodes_max);

    /*
     * Assert the QFLAGS_FULL bit, since a queue resource has been
     * reached.
     */
    queue->flags |= QFLAGS_FULL;

    return (-1);
    }

  /*
   * There are queued jobs and no queue limits have been reached.  Set
   * the user run limit and return okay.
   */
  return (0);
  }
