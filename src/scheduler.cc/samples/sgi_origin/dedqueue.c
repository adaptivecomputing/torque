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
 * FILE: dedqueue.c
 * Contains routines needed to process dedicated time and/or scheduled
 * down time.
 */
#include <sys/types.h>
#include <errno.h>
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

static int schedule_dedicated(Queue *dedq);

int
schd_handle_dedicated_time(Queue *dedq)
  {
  char   *id = "schd_handle_dedicated_time";
  Job    *jobs;
  Outage *outages;
  char   *comment, *shorthost;
  char   *ptr;
  char    buffer[256];
  int     ran;

  if (!(schd_ENFORCE_DEDTIME && schd_TimeNow >= schd_ENFORCE_DEDTIME))
    {
    comment = schd_booltime2val(schd_ENFORCE_DEDTIME);
    strcpy(buffer, schd_JobMsg[NO_DED_TIME]);

    if (ptr = strchr(comment, '('))
      {
      strcat(buffer, " ");
      strcat(buffer, ptr);
      }

    comment = buffer;

    DBPRT(("%s: dedicated time is not enforced.\n", id));
    }
  else
    {
    outages = schd_host_outage(dedq->exechost, 0);
    /* Get the short hostname for the outages string. */
    shorthost = schd_shorthost(dedq->exechost);

    if (outages != NULL)
      {

      /* Are we currently in dedicated time? */
      if ((schd_TimeNow >= outages->beg_time) &&
          (schd_TimeNow < outages->end_time))
        {
        (void)sprintf(log_buffer,
                      "currently dedtime for %s (%s/%s - %s/%s)",
                      shorthost ? shorthost : outages->exechost,
                      outages->beg_datestr, outages->beg_timestr,
                      outages->end_datestr, outages->end_timestr);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   log_buffer);
        DBPRT(("%s: %s\n", id, log_buffer));

        DBPRT(("%s: schedule_dedicated(%s@%s)\n", id,
               dedq->qname, dedq->exechost));
        ran = schedule_dedicated(dedq);

        return (ran);
        }
      else
        {
        /* Dedtime is being enforced, and a dedtime is upcoming. */


        (void)sprintf(log_buffer,
                      "upcoming dedtime for %s (%s/%s - %s/%s)",
                      shorthost ? shorthost : outages->exechost,
                      outages->beg_datestr, outages->beg_timestr,
                      outages->end_datestr, outages->end_timestr);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   log_buffer);
        DBPRT(("%s: %s\n", id, log_buffer));

        sprintf(buffer, "%s (%s %s %s)", schd_JobMsg[WAIT_FOR_DEDTIME],
                shorthost ? shorthost : outages->exechost,
                outages->beg_datestr, outages->beg_timestr);

        if (shorthost)
          free(shorthost);

        comment = buffer;

        DBPRT(("%s: %s\n", id, comment));
        }
      }
    else
      {
      /* Dedtime is being enforced, but no dedtime is upcoming. */
      sprintf(buffer, "%s (none scheduled for %s)",
              schd_JobMsg[WAIT_FOR_DEDTIME], shorthost);
      comment = buffer;
      DBPRT(("%s: waiting for dedicated time.\n", id));
      }
    }

  /* Set the comment in each of the dedicated time jobs
   * noting why they are waiting.
   * Don't care if it's NULL -- if so, no jobs are waiting.
   */
  for (jobs = dedq->jobs; jobs != NULL; jobs = jobs->next)
    {
    schd_comment_job(jobs, comment, JOB_COMMENT_OPTIONAL);
    }

  return (0);
  }

/*
 * Dedicated-time job scheduling algorithm.
 *
 */
static int
schedule_dedicated(Queue *dedq)
  {
  char   *id = "schedule_dedicated";
  Job    *job;
  char    reason[256];
  int     queue_full, ran = 0;

  /*
   * Determine if the dedicated queue is full.  This isn't a fatal error -
   * if it is, just comment each job that this is the case.  'reason' will
   * still be set in the check below.
   */
  queue_full = schd_check_queue_limits(dedq, reason);

  if (dedq->rsrcs == NULL)
    dedq->rsrcs = schd_get_resources(dedq->exechost);

  if (dedq->rsrcs == NULL)
    {
    DBPRT(("%s: schd_get_resources(%s) failed!\n", id, dedq->exechost));
    return (-1);
    }

  /*
   * Choose the first N jobs that "fit" into the queue's resource limits.
   * prevents the call to actually run the job, but allows the scheduler
   * to set the comment field to the correct string for each pending job.
   */

  DBPRT(("%s: Considering jobs:\n", id));

  for (job = dedq->jobs; job != NULL; job = job->next)
    {

    /* We are only interested in jobs in state "Q" == Queued */
    if (job->state != 'Q')
      continue;

    /*
     * If queue_limits() says that no jobs may be run on the queue, set
     * the comment field of each job.  'reason' will still be set from
     * the call to 'check_queue_limits()' above.
     */
    if (queue_full)
      {
      DBPRT(("queue_full -- rejected [%s]\n", reason));
      schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);
      continue;
      }

    if (!schd_job_fits_queue(job, dedq, reason))
      {
      DBPRT(("job %s does not fit %s : %s\n", job->jobid, dedq->qname,
             reason));
      schd_reject_job(job, reason);
      continue;
      }

    /*
     * Check that the resources requested by this job will not exceed
     * the system's available resources.
     */
    if (schd_resource_limits(job, dedq->rsrcs, reason))
      {
      DBPRT(("rejected by resource_limits() [%s]\n", reason));
      schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);
      continue;
      }

    DBPRT(("dedicated_can_run('%s', '%s'):\n", job->jobid, dedq->qname));

    /* Zero 'when' argument indicates that we are asking about "now". */

    if (!schd_dedicated_can_run(job, dedq, 0, reason))
      {
      DBPRT(("No (%s).\n", reason));
      schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);
      continue;
      }

    /*
     * Check that the resources requested by this job will not
     * exceed the queue's available resources, and that it will
     * fit within the dedicated time.
     */
    if (schd_user_limits(job, dedq, reason))
      {
      DBPRT(("rejected by user_limits() [%s]\n", reason));
      schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);
      continue;
      }


    /*
     * Job is okay.
     *
     * Run the job on the queue, and set its comment to indicate when
     * it began running.  Then add its expected resource usage to the
     * queue's limits and resources.
     */

    if (schd_run_job_on(job, dedq, dedq->exechost, SET_JOB_COMMENT))
      {
      (void)sprintf(log_buffer, "Unable to run job '%s' on queue '%s'.",
                    job->jobid, dedq->qname);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      return (-1);
      }

    /* Account for the job's impact on the queue.  */
    schd_charge_job(job, dedq, dedq->rsrcs);

    ran++; /* Count number of jobs running on the queue. */
    }

  DBPRT(("%s: ran %d jobs on queue %s@%s.\n", id, ran, dedq->qname,

         dedq->exechost));
  return (ran);
  }
