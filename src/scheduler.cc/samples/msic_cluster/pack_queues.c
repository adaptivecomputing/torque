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

extern int connector;
int schd_cpus_left(Job *job, Resources *rsrcs);

/*
 * Given a list of jobs, ordered from most-eligible to least-eligible to
 * run, attempt to place as many of them as possible into the queues listed
 * in qlist.  Jobs that will not currently fit are ignored (i.e. the entire
 * list is processed, running each job in order that fits).
 *
 * Jobs are run by run_job_on() as they are found on the list.  Resources
 * must be supplied because the jobs are tested against them in
 * 'schd_resources_avail()', and they are updated by sched_run_jobs_on().
 *
 * This function returns the number of jobs run, or -1 on error.
 */
int
schd_pack_queues(Job *jobs, QueueList *qlist, char *reason)
  {
  char   *id = "schd_pack_queues";
  Job    *job, *nextjob;
  QueueList *qptr;
  Queue *queue;
  int    allfull, jobsrun, ret;

  jobsrun = 0;

  DBPRT(("%s: scheduling queues", id));

  for (qptr = qlist; qptr != NULL; qptr = qptr->next)
    {
    if (!(qptr->queue->flags & QFLAGS_DISABLED) &&
        !(qptr->queue->flags & QFLAGS_NODEDOWN))
      DBPRT((" %s@%s", qptr->queue->qname, qptr->queue->exechost));
    }

  DBPRT((".\n"));

  if (jobs == NULL)
    {
    DBPRT(("No jobs available for QueueList %s%s - all done!",
           qlist->queue->qname, qlist->next ? " ..." : ""));
    return (0);
    }

  /*
   * Consider jobs from the list of queues.  For each job, if it appears
   * eligible to run, try to find a queue on which to place it.
   *
   * Note that schd_run_job_on() may remove the job from the list, so
   * this function must keep track of the current job's next pointer.
   */
  for (job = jobs; job != NULL; job = nextjob)
    {
    nextjob = job->next;

    /* Ignore any non-queued jobs in the list. */

    if (job->state != 'Q')
      continue;

    allfull = 1;  /* Assume all queues are full to start. */

    /* First check if there are *any* queues in which we *might* be
      * able to run a job. If not, then there's no point in going any
     * further.
     */
    for (qptr = qlist; qptr != NULL; qptr = qptr->next)
      {

      if (qptr->queue->rsrcs == NULL            ||
          (qptr->queue->flags & QFLAGS_FULL)     ||
          (qptr->queue->flags & QFLAGS_NODEDOWN) ||
          (qptr->queue->flags & QFLAGS_DISABLED) ||
          (qptr->queue->flags & QFLAGS_STOPPED))
        {
        continue;
        }

      if (!schd_evaluate_system(qptr->queue->rsrcs, reason))
        {
        DBPRT(("%s: evaluate_system: %s\n", id,
               qptr->queue->rsrcs ? reason : "No resources"));
        DBPRT(("%s: Marking queue %s@%s full\n", id,
               qptr->queue->qname, qptr->queue->exechost));
        qptr->queue->flags |= QFLAGS_FULL;
        continue;
        }

      /*
       * If an empty queue has not yet been found, check this one.
       * If this queue is not full, then at least one queue is not
        * full. Note this and continue-- if all queues are full at
       * the end of this exercise, we will give up.
       */
      if (allfull)
        if (!schd_check_queue_limits(qptr->queue, NULL))
          allfull = 0;

      }

    if (allfull &&
        (!(job->flags & JFLAGS_PRIORITY)) &&
        (!(job->flags & JFLAGS_WAITING)) &&
        ((job->flags & JFLAGS_RUNLIMIT) ||
         (job->flags & JFLAGS_CHKPTD)   ||
         (job->flags & JFLAGS_SUSPENDED)))
      {

      /* if all queues are full AND
       * this job is NOT high-priority, AND
       * its NOT a waiting job, AND
       * its either (ok to checkpoint, is checkpointed, or is suspended,
       * or is over its queue/user runlimit) then comment the job
       * and skip it
       */

      schd_comment_job(job, schd_JobMsg[NO_RESOURCES],
                       JOB_COMMENT_OPTIONAL);
      continue;
      }

    queue = find_best_exechost(job, qlist, reason);

    if (queue == NULL)
      {

      /* Hummm, we didn't find an acceptable queue. If this job is
       * NOT a low-priority job, then make room for it; otherwise
       * comment the job with the supplied reason, and go on to
       * the next job on the list. If for some reason we are unable
       * to make room for this job, then comment and skip it as well.
       */
      if ((job->flags & JFLAGS_PRIORITY) ||
          (job->flags & JFLAGS_WAITING)  ||
          ((!(job->flags & JFLAGS_CHKPTD)) &&
           (!(job->flags & JFLAGS_RUNLIMIT)) &&
           (!(job->flags & JFLAGS_SUSPENDED))))
        {

        queue = make_room_for_job(job, qlist, reason);
        }

      if (queue == NULL)
        {

        schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);
        continue;
        }
      }

    /* Attempt to run this job on the above-supplied queue */
    if (schd_run_job_on(job, queue, queue->exechost, SET_JOB_COMMENT))
      {
      (void)sprintf(log_buffer, "Unable to run batch job %s on queue %s",
                    job->jobid, queue->qname);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer))
      (void)sprintf(log_buffer, "marking host/queue %s DOWN",
                    queue->qname);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      queue->flags |= QFLAGS_NODEDOWN;
      }
    else
      {

      /* Lastly, account for this job's resource consumption. */
      schd_charge_job(job, queue, queue->rsrcs);
      jobsrun ++;
      }
    }

  return (jobsrun);
  }

int schd_cpus_left(Job *job, Resources *rsrcs)
  {
  return (rsrcs->ncpus_total - (rsrcs->ncpus_alloc + job->ncpus));
  }

Queue *find_best_exechost(Job *job, QueueList *qlist, char *reason)
  {
  /* char   *id = "find_best_exechost"; */
  int     best_ncpus, leftover_ncpus, ret;
  Queue  *queue, *best_queue;
  QueueList *qptr;

  /*
   * Look at all the Queues in the list that are currently available
   * for scheduling (as determined in schd_pack_queues() above); find
   * the best queue/exechost on which to run the candidate job; if it
   * not possible to run this job at this time, fill the reason array
   * with an explantion for the user. Otherwise, return a pointer to
   * the queue to use.
   */

  best_ncpus = 100000;
  best_queue = NULL;

  for (qptr = qlist; qptr != NULL; qptr = qptr->next)
    {
    queue = qptr->queue;

    /*
     * If this queue is missing its resource info, or if its
    * STOPPED, etc.,  skip it.
     */

    if (qptr->queue->rsrcs == NULL            ||
        (qptr->queue->flags & QFLAGS_FULL)     ||
        (qptr->queue->flags & QFLAGS_DISABLED) ||
        (qptr->queue->flags & QFLAGS_NODEDOWN) ||
        (qptr->queue->flags & QFLAGS_STOPPED))
      continue;

    /*
     * Check that the queue is actually available to pack jobs into.
     */
    if (schd_check_queue_limits(queue, reason))
      continue;

    /*
     * Check if *this* job can run in this queue or not, based on
    * queue minimum and maximum limits.
     */
    if (!schd_job_fits_queue(job, queue, reason))
      continue;

    /*
     * If this job has a user access control list, check that this
     * job can be allowed in it.
     */
    if (queue->useracl && (queue->flags & QFLAGS_USER_ACL))
      {
      if (!schd_useracl_okay(job, queue, reason))
        continue;
      }

    /*
     * Check primetime limits.  If it is primetime now, and the job will
    * complete before primetime, apply any PT limits. If it will overrun
    * primetime, be sure that the amount that falls within primetime
    * will not violate the primetime limit. Special jobs are not subject
    * to primetime walltime limits.
     */
    if (schd_ENFORCE_PRIME_TIME)
      {
      if (!(job->flags & JFLAGS_PRIORITY))
        if (schd_primetime_limits(job, queue, 0, reason))
          continue;
      }

    /*
     * Check this job against the execution host resource limits.
     */
    if (!schd_resources_avail(job, queue->rsrcs, reason))
      continue;

    /*
     * Check per-user queue limits.
     */
    if (schd_user_limits(job, queue, reason))
      continue;

    /*
     * Check if this user has suspended jobs on this queue that
     * came from the same oqueue as this job
     */
    ret = has_suspended_jobs(queue, job);

    if (ret)
      {
      strcpy(reason, "Too many running jobs for this user");
      continue;
      }

    /*
     * We found a queue on which this job can run. Now we need to decide
    * if this is the best queue for this job.
     */
    leftover_ncpus = schd_cpus_left(job, queue->rsrcs);

    if (leftover_ncpus < best_ncpus)
      {
      best_ncpus = leftover_ncpus;
      best_queue = queue;

      if (leftover_ncpus == 0)
        return(best_queue); /* Can't get much better than this! */
      }
    }

  return(best_queue);
  }

Queue *make_room_for_job(Job *priority_job, QueueList *qlist, char *reason)
  {
  Job    *jobptr;
  char   *id = "make_room_for_job";
  int     best_job_cnt, ncpus_avail, jobs_to_checkpoint, ret;
  Queue  *queue, *best_queue;
  QueueList *qptr;

  /*
   * The previous attempt to 'find_best_exec_host' failed-- most likely
   * there are not enough free resources available; so we now need to
   * look at all the Queues in the list that are currently running jobs,
   * and determine which jobs we need to suspend/checkpoint in order to
   * run our job. If it is not possible to run this job at this time (e.g.
   * if all the exec hosts are down (gasp!), then fill the reason array
   * with an explaination for the user. Otherwise, return a pointer to
   * the queue to use.
   */

  best_job_cnt = 100000;
  best_queue = NULL;

  for (qptr = qlist; qptr != NULL; qptr = qptr->next)
    {
    queue = qptr->queue;

    /*
     * If this queue is missing its resource info, or if its
    * STOPPED, etc.,  skip it.
     */

    if (qptr->queue->rsrcs == NULL            ||
        (qptr->queue->flags & QFLAGS_NODEDOWN) ||
        (qptr->queue->flags & QFLAGS_DISABLED) ||
        (qptr->queue->flags & QFLAGS_STOPPED))
      continue;

    /*
     * Verify that this architecture and/or exechost are
     * correct for this job.
     */
    if (priority_job->arch != NULL)
      {
      if (strcmp(priority_job->arch, qptr->queue->rsrcs->arch))
        {
        sprintf(reason, "%s (%s)", schd_JobMsg[NO_ARCH],
                priority_job->arch);
        continue;
        }
      }

    if (priority_job->exechost != NULL)
      {
      if (strcmp(priority_job->exechost, qptr->queue->exechost))
        {
        sprintf(reason, "%s (%s)", schd_JobMsg[WAIT_CHKPT_HOST],
                priority_job->exechost);
        continue;
        }
      }

    /*
     * Check if *this* job can run in this queue or not, based on
    * queue minimum and maximum limits.
     */
    if (!schd_job_fits_queue(priority_job, queue, reason))
      continue;

    /*
     * If this job has a user access control list, check that this
     * job can be allowed in it.
     */
    if (queue->useracl && (queue->flags & QFLAGS_USER_ACL))
      {
      if (!schd_useracl_okay(priority_job, queue, reason))
        continue;
      }

    /*
     * We found a queue on which this job can run. Now we need to decide
    * if this is the best queue for this job.
    *
    * First test to see if the user has any suspended jobs in this
    * queue from the same oqueue as the job in question. If so, try
    * next queue
    */

    ret = has_suspended_jobs(queue, priority_job);

    if (ret)
      {
      strcpy(reason, "Too many running jobs for this user");
      continue;
      }

    /*
     * Walk the list of jobs for
       * this queue, looking at those that we previously marked as being
     * eligable for checkpointing; count # jobs necessary to free enough
     * resources to run job, and amount freed by doing so.
            */

    ncpus_avail = queue->ncpus_max - queue->ncpus_assn;

    jobs_to_checkpoint = 0;

    DBPRT(("CHK: %s avail=%d job2ckp=%d\n", queue->qname, ncpus_avail,
           jobs_to_checkpoint));

    for (jobptr = queue->jobs; jobptr != NULL; jobptr = jobptr->next)
      {

      /* assumption: Running jobs are at top/front of list */
      if (jobptr->state != 'R')
        break;

      /* don't let an over-usage job push out a non-over-usage job
       * unless the target job is over its run limit, and they are
       * from the same oqueue, and they have different owners
       */

      if (priority_job->flags & JFLAGS_CHKPT_OK)
        {
        if (jobptr->flags & JFLAGS_CHKPT_OK)
          {
          /* both jobs are over usage...  */

          if (!strcmp(priority_job->oqueue, jobptr->oqueue) &&
              (priority_job->sort_order == jobptr->sort_order) &&
              (jobptr->flags & JFLAGS_RUNLIMIT) &&
              (strcmp(priority_job->owner, jobptr->owner)))
            {
            /* and from same oqueue with same sort order
             * and have different owners, and the target
             * job is over its runlimit*/
            ; /* then proceed; otherwise skip the job */
            }
          else
            continue;
          }
        else
          continue;
        }

      if (jobptr->flags & JFLAGS_CHKPT_OK ||
          jobptr->flags & JFLAGS_RUNLIMIT)
        {

        if (!(jobptr->flags & JFLAGS_PRIORITY) &&
            (!(jobptr->flags & JFLAGS_WAITING)))
          {

          /* only consider jobs from queues that have used more time
           * than the priority job has */
          if ((priority_job->flags & JFLAGS_PRIORITY) ||
              (priority_job->flags & JFLAGS_WAITING)  ||
              (priority_job->sort_order < jobptr->sort_order) ||

              ((priority_job->sort_order == jobptr->sort_order) &&
               (strcmp(priority_job->owner, jobptr->owner))))
            {

            jobs_to_checkpoint++;
            ncpus_avail += jobptr->ncpus;

            }
          }
        }

      if (ncpus_avail >= priority_job->ncpus)
        break;
      }

    if (jobs_to_checkpoint == 0 || ncpus_avail < priority_job->ncpus)
      continue;

    if (jobs_to_checkpoint < best_job_cnt)
      {
      best_job_cnt = jobs_to_checkpoint;
      best_queue = queue;
      }
    }

  if (jobs_to_checkpoint == 0 || best_queue == NULL)
    {

    /* Hummm, looking only at low-priority jobs didn't turn up anything.
    * So check how important this job is. If it's not High Priority
    * then just skip it. Otherwise, try harder...
    */

    if ((priority_job->flags & JFLAGS_PRIORITY) ||
        (priority_job->flags & JFLAGS_WAITING))
      {
      sprintf(log_buffer, "No low-pri jobs to checkpoint, retrying... ");
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      }
    else
      {
      return(NULL);
      }

    /*
     * Loop through all running jobs on all queues again, but this time
     * we are willing to checkpoint ANY non-Express, non-Waiting jobs.
     */

    best_job_cnt = 100000;

    best_queue = NULL;

    for (qptr = qlist; qptr != NULL; qptr = qptr->next)
      {
      queue = qptr->queue;

      /*
       * If this queue is missing its resource info, or if its
      * STOPPED, etc.,  skip it.
       */

      if (qptr->queue->rsrcs == NULL            ||
          (qptr->queue->flags & QFLAGS_DISABLED) ||
          (qptr->queue->flags & QFLAGS_NODEDOWN) ||
          (qptr->queue->flags & QFLAGS_STOPPED))
        continue;

      /*
       * Verify that this architecture and/or exechost are
       * correct for this job.
       */
      if (priority_job->arch != NULL)
        {
        if (strcmp(priority_job->arch, qptr->queue->rsrcs->arch))
          {
          sprintf(reason, "%s (%s)", schd_JobMsg[NO_ARCH],
                  priority_job->arch);
          continue;
          }
        }

      if (priority_job->exechost != NULL)
        {
        if (strcmp(priority_job->exechost, qptr->queue->exechost))
          {
          sprintf(reason, "%s (%s)", schd_JobMsg[WAIT_CHKPT_HOST],
                  priority_job->exechost);
          continue;
          }
        }

      /*
       * Check if *this* job can run in this queue or not, based on
      * queue minimum and maximum limits.
       */
      if (!schd_job_fits_queue(priority_job, queue, reason))
        continue;

      /*
       * If this job has a user access control list, check that this
       * job can be allowed in it.
       */
      if (queue->useracl && (queue->flags & QFLAGS_USER_ACL))
        {
        if (!schd_useracl_okay(priority_job, queue, reason))
          continue;
        }

      /*
       * We found a queue on which this job can run. Now we need to
      * decide if this is the best queue for this job. Walk the list
      * of jobs for this queue, looking at those that we previously
      * marked as being eligable for checkpointing; count # jobs
      * necessary to free enough resources to run job, and amount
      * freed by doing so.
       */

      ncpus_avail = queue->ncpus_max - queue->ncpus_assn;

      jobs_to_checkpoint = 0;

      for (jobptr = queue->jobs; jobptr != NULL; jobptr = jobptr->next)
        {

        DBPRT(("CHK: %s avail=%d job2ckp=%d\n", queue->qname,
               ncpus_avail, jobs_to_checkpoint));

        if (jobptr->state != 'R')
          break;

        if (!(jobptr->flags & JFLAGS_PRIORITY) &&
            !(jobptr->flags & JFLAGS_WAITING))
          {

          if ((priority_job->flags & JFLAGS_PRIORITY) ||
              (priority_job->flags & JFLAGS_WAITING))
            {

            jobs_to_checkpoint++;
            ncpus_avail += jobptr->ncpus;
            DBPRT(("CHK:   job %s %d ncpus\n", jobptr->jobid,
                   jobptr->ncpus));

            }
          else
            {
            if ((priority_job->sort_order < jobptr->sort_order) ||
                ((priority_job->sort_order == jobptr->sort_order) &&
                 (strcmp(priority_job->owner, jobptr->owner))))
              {
              jobs_to_checkpoint++;
              ncpus_avail += jobptr->ncpus;
              DBPRT(("CHK:   job %s %d ncpus\n", jobptr->jobid,
                     jobptr->ncpus));
              }
            }
          }

        if (ncpus_avail >= priority_job->ncpus)
          break;
        }

      if (jobs_to_checkpoint == 0 || ncpus_avail < priority_job->ncpus)
        continue;

      if (jobs_to_checkpoint < best_job_cnt)
        {
        best_job_cnt = jobs_to_checkpoint;
        best_queue = queue;
        }
      }

    if (jobs_to_checkpoint == 0 || best_queue == NULL)
      {
      sprintf(log_buffer, "Found NO jobs to checkpoint for %s",
              priority_job->jobid);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      return(NULL);
      }
    }

  /* If we reach this point, then we know the best queue to use. So walk
   * that queue again, stopping the the N jobs necessary to free the
   * resources we need.
   */

  ncpus_avail = best_queue->ncpus_max - best_queue->ncpus_assn;

  for (jobptr = best_queue->jobs; jobptr != NULL; jobptr = jobptr->next)
    {

    DBPRT(("CHK: Avail: %d of %d needed\n", ncpus_avail,
           priority_job->ncpus));

    if (priority_job->flags & JFLAGS_CHKPT_OK)
      {
      if (jobptr->flags & JFLAGS_CHKPT_OK)
        {
        /* both jobs are over usage...  */

        if (!strcmp(priority_job->oqueue, jobptr->oqueue) &&
            (priority_job->sort_order == jobptr->sort_order) &&
            (jobptr->flags & JFLAGS_RUNLIMIT) &&
            (strcmp(priority_job->owner, jobptr->owner)))
          {
          /* and from same oqueue with same sort order
          * and have different owners, and the target
          * job is over its runlimit*/
          ; /* then proceed; otherwise skip the job */
          }
        else
          continue;
        }
      else
        continue;
      }

    if (jobptr->flags & JFLAGS_CHKPT_OK ||
        jobptr->flags & JFLAGS_RUNLIMIT)
      {

      if (!(jobptr->flags & JFLAGS_PRIORITY) &&
          !(jobptr->flags & JFLAGS_WAITING))
        {

        if ((priority_job->flags & JFLAGS_PRIORITY) ||
            (priority_job->flags & JFLAGS_WAITING)  ||
            (priority_job->sort_order < jobptr->sort_order) ||
            ((priority_job->sort_order == jobptr->sort_order) &&
             (strcmp(priority_job->owner, jobptr->owner))))
          {

          DBPRT(("CHK: %s would free %d cpus\n", jobptr->jobid,
                 jobptr->ncpus));

          if (!schd_checkpoint_job(jobptr))
            {
            /* Hummm- checkpoint of this job failed; better skip it and
             * retry the whole routine again; given that we may have already
             * checkpointed some jobs, the next time thru hopefully we will
             * be able to run our high-priority job. If not, then what?
             */
            jobptr->flags &= ~JFLAGS_CHKPT_OK;
            jobptr->flags |=  JFLAGS_CHKPTD;
            sprintf(log_buffer, "WARNING: checkpoint error for %s, retrying...",
                    jobptr->jobid);
            log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
            return(make_room_for_job(priority_job, qlist, reason));
            }

          ncpus_avail += jobptr->ncpus;
          }
        }
      }

    if (ncpus_avail >= priority_job->ncpus)
      break;
    }

  if (ncpus_avail < priority_job->ncpus)
    {
    /* oops, didn't get enough cpus. Must have not been enough
     * low priority jobs; But since we are in this part of the
     * routine, we *know* that there are enough jobs, so we need
     * to make a second pass, this time getting ANY non-priority
     * or non-waiting jobs.
     */

    for (jobptr = best_queue->jobs;jobptr != NULL;jobptr = jobptr->next)
      {
      DBPRT(("CHK2: Avail: %d of %d needed\n", ncpus_avail,
             priority_job->ncpus));

      if (!(jobptr->flags & JFLAGS_PRIORITY) &&
          !(jobptr->flags & JFLAGS_WAITING)  &&
          (jobptr->state == 'R'))
        {

        if ((priority_job->flags & JFLAGS_PRIORITY) ||
            (priority_job->flags & JFLAGS_WAITING))
          {

          if (!schd_checkpoint_job(jobptr))
            {
            /* Hummm- checkpoint of this job failed; better skip it
             * and retry the whole routine again; given that we may
             * have already checkpointed some jobs, the next time
             * thru hopefully we will be able to run our high-priority
             * job. If not, then what?
             */
            jobptr->flags &= ~JFLAGS_CHKPT_OK;
            jobptr->flags |=  JFLAGS_CHKPTD;
            DBPRT(("CHK:   checkpoint error, retrying...\n"));
            return(make_room_for_job(priority_job, qlist, reason));
            }

          ncpus_avail += jobptr->ncpus;
          }
        else
          {
          if ((priority_job->sort_order < jobptr->sort_order) ||
              ((priority_job->sort_order = jobptr->sort_order) &&
               (strcmp(priority_job->owner, jobptr->owner))))
            {

            if (!schd_checkpoint_job(jobptr))
              {
              jobptr->flags &= ~JFLAGS_CHKPT_OK;
              jobptr->flags |=  JFLAGS_CHKPTD;
              DBPRT(("CHK:   checkpoint error, retrying...\n"));
              return(make_room_for_job(priority_job, qlist, reason));
              }

            ncpus_avail += jobptr->ncpus;
            }
          }
        }

      if (ncpus_avail >= priority_job->ncpus)
        break;
      }
    }

  /* If we still don't have enough cpus, then we have a real problem.
   * Bail outta here, and hope real hard that the next iteration things
   * will have improved.
   */
  if (ncpus_avail < priority_job->ncpus)
    return(NULL);

  /* otherwise, we have suspended/checkpointed/ or otherwise cleared space
   * for this priority job; return a pointer to this queue so we can run
   * this job
   */
  return(best_queue);
  }

int schd_checkpoint_job(Job *job)
  {
  char *id = "checkpoint_job";
  int ret = 0, done = 0;

  if (job->state == 'Q')
    {
    sprintf(log_buffer, "WARNING: tried to checkpoint QUEUED job %s; WHY?",
            job->jobid);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    return (1);
    }

  if (100.0 * job->time_left / job->walltime <= schd_SUSPEND_THRESHOLD)
    {
    /* Attempt to suspend the job */
    ret = pbs_sigjob(connector, job->jobid, "suspend", NULL);

    if (ret)
      {
      if (job_just_exited(ret, job))
        return(0); /* try again */
      else
        sprintf(log_buffer,
                "suspend job %s FAILED (%d); trying checkpoint",
                job->jobid, ret);
      }
    else
      {
      sprintf(log_buffer, "suspended job %s", job->jobid);
      done = 1;
      }

    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

  if (done)
    {
    schd_UNcharge_job(job, job->queue, job->queue->rsrcs);
    schd_move_job_to(job, schd_SubmitQueue->queue);
    return(done);

    }
  else
    {
    /* Attempt to checkpoint the job */
    ret = pbs_holdjob(connector, job->jobid, "s", NULL);

    if (ret)
      {
      if (job_just_exited(ret, job))
        return(0); /* try again */
      else
        sprintf(log_buffer, "checkpoint job %s FAILED (%d)",
                job->jobid, ret);
      }
    else
      {
      /* DEBUG: there's a window of opportunity between the Server
       * ACK'ing the qhold, and MOM actually completing the the
       * checkpoint. Bob is investigating this. Until its resolved,
       * we are disabling the forceable requeue of jobs (if both
       * suspend and checkpoint fail).
       */

      /*
       * if (schd_FORCE_REQUEUE)
              *   pbs_rerunjob(connector, job->jobid, NULL);
       */

      sprintf(log_buffer, "checkpointed/requeued job %s", job->jobid);
      done = 1;
      }

    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

  /* If we succeeded in stopping this job, then reduce the usage counters
   * for this job's resources.
   */
  if (done)
    {
    schd_UNcharge_job(job, job->queue, job->queue->rsrcs);

    /* Move the job from its run queue back to o-queue. */
    pbs_movejob(connector, job->jobid, job->oqueue, NULL);
    schd_move_job_to(job, schd_SubmitQueue->queue);
    }

  /* in either case, release the hold on the job... */
  pbs_rlsjob(connector, job->jobid, "s", NULL);

  return(done);
  }

int job_just_exited(int error_code, Job *job)
  {
  if (error_code == 15001 /* Unknown Job Id */)
    {
    job->state = 'E';
    schd_UNcharge_job(job, job->queue, job->queue->rsrcs);
    return (1);
    }

  return (0);
  }

int has_suspended_jobs(Queue *queue, Job *job)
  {
  Job    *jobptr, *nextjob;
  int count = 0;
  char   *id = "has_suspended_jobs";

  for (jobptr = queue->jobs; jobptr != NULL; jobptr = nextjob)
    {
    nextjob = jobptr->next;

    if (jobptr->flags & JFLAGS_SUSPENDED)
      {
      if (!strcmp(jobptr->owner, job->owner))
        {
        if (!strcmp(jobptr->oqueue, job->oqueue))
          {
          count++;
          }
        }
      }
    }

  return(count);
  }
