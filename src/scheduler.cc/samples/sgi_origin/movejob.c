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

#include <stdio.h>

/* PBS header files */
#include        "pbs_error.h"
#include        "pbs_ifl.h"
#include        "log.h"

/* Toolkit header files */
#include "toolkit.h"
#include "gblxvars.h"

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

  return (0);
  }
