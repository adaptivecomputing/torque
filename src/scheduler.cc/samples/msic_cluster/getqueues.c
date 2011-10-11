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
#include <errno.h>
#include <sys/param.h>
#if defined(sgi)
#include <sys/sysmacros.h>
#endif
#include <string.h>

/* Scheduler header files */
#include "toolkit.h"
#include "gblxvars.h"

/* PBS header files */
#include "pbs_error.h"
#include "list_link.h"
#include "attribute.h"
#include "pbs_nodes.h"
#include "log.h"


extern int connector;

/*
 * Get information about each of the queues in the list of lists.  If
 * schd_get_queue_limits() fails, return the error condition.  It may
 * be a transient or a hard failure, which the caller may want to deal
 * with.  If all queues are successful, return '0'.
 */
int
schd_get_queues(void)
  {
  int    ret;
  QueueList *qptr;

  ret = schd_get_queue_limits(schd_SubmitQueue->queue);

  if (ret)
    {
    DBPRT(("schd_get_queues: get_queue_limits for %s failed.\n",
           schd_SubmitQueue->queue->qname));
    return(-1);
    }

  for (qptr = schd_BatchQueues; qptr != NULL; qptr = qptr->next)
    {
    ret = schd_get_queue_limits(qptr->queue);

    if (ret)
      DBPRT(("schd_get_queues: get_queue_limits for %s failed.\n",
             qptr->queue->qname));
    }

  return (0);
  }

/*
 * schd_get_queue_limits - query queue information from the server.
 *
 * Returns 0 on success, -1 for "fatal errors", and 1 for a transient
 * error (i.e., the queue failed the sanity checks imposed by the
 * queue_sanity() function).
 */
int
schd_get_queue_limits(Queue *queue)
  {
  char   *id = "schd_get_queue_limits";
  int     istrue;
  int     local_errno = 0;
  Batch_Status *bs;
  AttrList *attr;
  static AttrList alist[] =
    {
      {&alist[1],  ATTR_start, "", ""},
    {&alist[2],  ATTR_enable, "", ""},
    {&alist[3],  ATTR_count, "", ""},
    {&alist[4],  ATTR_maxuserrun, "", ""},
    {&alist[5],  ATTR_rescavail, "", ""},
    {&alist[6],  ATTR_rescassn, "", ""},
    {&alist[7],  ATTR_rescdflt, "", ""},
    {&alist[8],  ATTR_rescmax, "", ""},
    {&alist[9],  ATTR_rescmin, "", ""},
    {&alist[10], ATTR_acluren, "", ""},
    {&alist[11], ATTR_acluser, "", ""},
    {NULL,       ATTR_maxrun, "", ""}
    };
  queue->running = UNSPECIFIED;
  queue->queued = UNSPECIFIED;
  queue->maxrun = UNSPECIFIED;
  queue->userrun      = UNSPECIFIED;
  queue->ncpus_max = UNSPECIFIED;
  queue->ncpus_min = UNSPECIFIED;
  queue->ncpus_default = UNSPECIFIED;
  queue->ncpus_assn = UNSPECIFIED;
  queue->mem_assn = UNSPECIFIED;
  queue->mem_max = UNSPECIFIED;
  queue->mem_min = UNSPECIFIED;
  queue->mem_default  = UNSPECIFIED;
  queue->wallt_max = UNSPECIFIED;
  queue->wallt_min = UNSPECIFIED;
  queue->wallt_default = UNSPECIFIED;
  queue->rsrcs = NULL;
  queue->flags = 0;

  if (queue->rsrcs)
    {
    DBPRT(("%s: found resource list on queue '%s'!  Freeing them...\n", id,
           queue->qname));
    cleanup_rsrcs(queue->rsrcs);
    queue->rsrcs        = NULL;
    }

  if (queue->jobs)
    {
    DBPRT(("%s: found jobs on queue '%s'!  Freeing them...\n", id,
           queue->qname));
    schd_free_jobs(queue->jobs);
    queue->jobs         = NULL;
    }

  if (queue->useracl)
    {
    DBPRT(("%s: found user ACL list on queue '%s'!  Freeing it...\n", id,
           queue->qname));
    schd_free_useracl(queue->useracl);
    queue->useracl = NULL;
    }


  /* Ask the server for information about the specified queue. */

  if ((bs = pbs_statque_err(connector, queue->qname, alist, NULL, &local_errno)) == NULL)
    {
    sprintf(log_buffer, "pbs_statque failed, \"%s\" %d",
            queue->qname, local_errno);
    log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    return (-1);
    }

  /* Process the list of attributes returned by the server. */

  for (attr = bs->attribs; attr != NULL; attr = attr->next)
    {

    /* Is queue started? */
    if (!strcmp(attr->name, ATTR_start))
      {
      if (schd_val2bool(attr->value, &istrue) == 0)
        {
        if (istrue) /* if true, queue is not stopped. */
          queue->flags &= ~QFLAGS_STOPPED;
        else
          queue->flags |= QFLAGS_STOPPED;
        }
      else
        {
        DBPRT(("%s: couldn't parse attr %s value %s to boolean\n", id,
               attr->name, attr->value));
        }

      continue;
      }

    /* Is queue enabled? */
    if (!strcmp(attr->name, ATTR_enable))
      {
      if (schd_val2bool(attr->value, &istrue) == 0)
        {
        if (istrue) /* if true, queue is not disabled. */
          queue->flags &= ~QFLAGS_DISABLED;
        else
          queue->flags |= QFLAGS_DISABLED;
        }
      else
        {
        DBPRT(("%s: couldn't parse attr %s value %s to boolean\n", id,
               attr->name, attr->value));
        }

      continue;
      }

    /* How many jobs are queued and running? */
    if (!strcmp(attr->name, ATTR_count))
      {
      queue->queued = schd_how_many(attr->value, SC_QUEUED);
      queue->running = schd_how_many(attr->value, SC_RUNNING);
      continue;
      }

    /* Queue-wide maximum number of jobs running. */
    if (!strcmp(attr->name, ATTR_maxrun))
      {
      queue->maxrun = atoi(attr->value);
      continue;
      }

    /* Per-user maximum number of jobs running. */
    if (!strcmp(attr->name, ATTR_maxuserrun))
      {
      queue->userrun = atoi(attr->value);
      continue;
      }

    /* Is there an enabled user access control list on this queue? */
    if (!strcmp(attr->name, ATTR_acluren))
      {
      if (schd_val2bool(attr->value, &istrue) == 0)
        {
        if (istrue) /* if true, queue has an ACL */
          queue->flags |= QFLAGS_USER_ACL;
        else
          queue->flags &= ~QFLAGS_USER_ACL;
        }
      else
        {
        DBPRT(("%s: couldn't parse attr %s value %s to boolean\n", id,
               attr->name, attr->value));
        }

      continue;
      }

    if (!strcmp(attr->name, ATTR_acluser))
      {
      if (queue->useracl)
        {
        DBPRT(("queue %s acluser already set!\n", queue->qname));
        schd_free_useracl(queue->useracl);
        }

      queue->useracl = schd_create_useracl(attr->value);

      continue;
      }

    /* Queue maximum resource usage. */
    if (!strcmp(attr->name, ATTR_rescmax))
      {
      if (!strcmp("mem", attr->resource))
        {
        queue->mem_max = schd_val2byte(attr->value);
        continue;
        }

      if (!strcmp("ncpus", attr->resource))
        {
        queue->ncpus_max = atoi(attr->value);
        continue;
        }

      if (!strcmp("walltime", attr->resource))
        {
        queue->wallt_max = schd_val2sec(attr->value);
        continue;
        }

      continue;
      }

    /* Queue minimum resource usage. */
    if (!strcmp(attr->name, ATTR_rescmin))
      {
      if (!strcmp("mem", attr->resource))
        {
        queue->mem_min = schd_val2byte(attr->value);
        continue;
        }

      if (!strcmp("ncpus", attr->resource))
        {
        queue->ncpus_min = atoi(attr->value);
        continue;
        }

      if (!strcmp("walltime", attr->resource))
        {
        queue->wallt_min = schd_val2sec(attr->value);
        continue;
        }

      continue;
      }

    /* Queue assigned (in use) resource usage. */
    if (!strcmp(attr->name, ATTR_rescassn))
      {
      if (!strcmp("mem", attr->resource))
        {
        queue->mem_assn = schd_val2byte(attr->value);
        continue;
        }

      if (!strcmp("ncpus", attr->resource))
        {
        queue->ncpus_assn = atoi(attr->value);
        }

      continue;
      }

    if (!strcmp(attr->name, ATTR_rescdflt))
      {
      if (!strcmp("mem", attr->resource))
        {
        queue->mem_default = schd_val2byte(attr->value);
        continue;
        }

      if (!strcmp("ncpus", attr->resource))
        {
        queue->ncpus_default = atoi(attr->value);
        continue;
        }

      if (!strcmp("walltime", attr->resource))
        queue->wallt_default = schd_val2sec(attr->value);
      }

    /* Ignore anything else */
    }

  pbs_statfree(bs);

  return (0);
  }

/* Get the rest of the info we need for the queues in order to do
 * scheduling: exechost resource usage, the jobs that belong in this
 * queue, etc.
 */

int
schd_get_queue_util(void)
  {
  int    ret;
  char  *id = "get_queue_util";
  QueueList *qptr;
  void schd_calc_suspended_jobs(void);

  /* first, get status of nodes from the server.
   */
  get_node_status();

  /* next, get status of queues for those nodes from the server.
   */
  qptr = schd_SubmitQueue;
  ret = schd_get_queue_info(qptr->queue);

  if (ret)
    DBPRT(("get_queue_util: get_queue_info for %s failed.\n",
           schd_SubmitQueue->queue->qname));

  for (qptr = schd_BatchQueues; qptr != NULL; qptr = qptr->next)
    {
    if (qptr->queue->flags & QFLAGS_NODEDOWN)
      {
      sprintf(log_buffer, "Skipping UNAVAILABLE node %s",
              qptr->queue->exechost);
      DBPRT(("%s: %s\n", id, log_buffer));
      log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
      }
    else
      {
      ret = schd_get_queue_info(qptr->queue);

      if (ret)
        DBPRT(("get_queue_util: get_queue_info for %s failed.\n",
               qptr->queue->qname));
      }
    }

  /*
   * Update queue resources assigned for suspended jobs.
   */
  schd_calc_suspended_jobs();

  return (0);
  }

/*
 * Populate the queue struct with the information needed for scheduling;
 * this includes "claiming" jobs that "belong" to this queue; and querying
 * the resource monitor for queue->exechost's information.
 */
int
schd_get_queue_info(Queue *queue)
  {
  char *id = "get_queue_info";
  double tmpval;
  int   moved = 0;

  /*
   * Move any jobs on this queue from the global list onto the queue's
   * list.  Keep track of when the longest-running job will end, and set
   * the 'empty_by' field to that value.  Maintain the ordering as it was
   * in "schd_AllJobs".
   */

  if (schd_AllJobs)
    moved = queue_claim_jobs(queue, &schd_AllJobs);

  if (moved < 0)
    {
    sprintf(log_buffer, "%s: WARNING! Queue '%s' failed to claim jobs.",
            id, queue->qname);
    log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    }

  if (queue->ncpus_assn == UNSPECIFIED)
    queue->ncpus_assn = 0;

  if (queue->mem_assn   == UNSPECIFIED)
    queue->mem_assn   = 0;

  if (queue->running    == UNSPECIFIED)
    queue->running    = 0;

  /*
   * Get the resources for this queue from the resource monitor (if
   * available).  If the resmom is not accessible, disable the queue.
   * Don't bother checking if the queue is Stopped.
   */

  if (strcmp(queue->qname, schd_SubmitQueue->queue->qname) != 0 &&
      (queue->flags & QFLAGS_STOPPED) == 0)
    {
    queue->rsrcs = schd_get_resources(queue->exechost);

    if (queue->rsrcs != NULL)
      {

      /* Account for this queue's resources. */
      queue->rsrcs->ncpus_alloc += queue->ncpus_assn;
      queue->rsrcs->mem_alloc   += queue->mem_assn;

      if (queue->rsrcs->freemem < 1)
        {

        tmpval = queue->rsrcs->mem_total - queue->rsrcs->mem_alloc;

        if (tmpval < 1)
          queue->rsrcs->freemem = 0;
        else
          queue->rsrcs->freemem = tmpval;
        }

      queue->rsrcs->njobs       += queue->running;

      queue->ncpus_max =
        (queue->ncpus_max <= queue->rsrcs->ncpus_total ?
         queue->ncpus_max  : queue->rsrcs->ncpus_total);
      }
    else
      {
      (void)sprintf(log_buffer,
                    "Can't get resources for %s@%s - marking unavailable.",
                    queue->qname, queue->exechost);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));

      queue->flags |= QFLAGS_NODEDOWN;
      }
    }

  return (0);
  }

int
queue_claim_jobs(Queue *queue, Job **joblist_ptr)
  {
  Job    *job, *prev, *next, *qtail, *longest;
  int     moved;
  int     running, queued, held, other;

  /*
   * The time at which this queue should be empty (i.e. when all jobs
   * currently running on it are completed).  Keep track of the longest
   * running job and compute the empties_at value from that.
   */
  longest = NULL;

  /*
   * Keep track of some statistics about what jobs have been found in the
   * list.  These aren't really used (yet), but are easy to compute.
   */
  running = 0;
  queued = 0;
  held = 0;
  other = 0;

  /*
   * The number of jobs that have been moved from the global list to the
   * per-queue list.
   */
  moved = 0;

  /*
   * Find the last element of the list of jobs on this queue.  This is
   * probably unnecessary (since this should always be called with
   * 'queue->jobs' pointing to NULL.  Still, it doesn't hurt to try.
   */
  qtail = NULL;

  if (queue->jobs)
    {
    for (qtail = queue->jobs; qtail->next != NULL; qtail = qtail->next)
      /* Do nothing - just walk to next-to-last element of list */
      ;
    }

  prev = NULL;

  for (job = *joblist_ptr; job != NULL; job = next)
    {
    next = job->next;

    /* Wrong queue -- ignore this job. */

    if (strcmp(job->qname, queue->qname))
      {
      prev = job;
      continue;
      }

    /*
     * This job belongs to this queue.  Remove it from the job list and
     * place it at the tail of the queue's job list.  This is somewhat
     * complicated since we have to remove it from the joblist first.
     * If there is no "previous" job element, then the current job is
     * the head of the list.
     */
    if (job == *joblist_ptr)
      {

      /*
       * This is the head of the list -- just point the list head to
       * the job's next pointer and now the job is "orphaned".
       */

      *joblist_ptr = next;
      prev = *joblist_ptr;

      }
    else
      {

      /*
       * This job lies in the middle of the list somewhere.  Jump over
       * it in the previous element, and we're done.  Note that since
       * we skipped this job, the previous job pointer does not change.
       */

      prev->next = job->next;
      }

    /*
     * Now 'job' is the only active handle on the job.  Place it at the
     * tail of the queue's list.  If 'qtail' is NULL, this is the first
     * job -- place it at the head of the list.  Otherwise, place it after
     * the element pointed to by the 'qtail'.  Either way, this is the
     * last element in the list, so point the qtail at it and clear its
     * next pointer.
     */
    if (qtail == NULL)
      queue->jobs = job;
    else
      qtail->next = job;

    qtail = job;

    job->next = NULL;

    if (job->flags & JFLAGS_QNAME_LOCAL)
      {
      /*
       * The job has some memory that was allocated when it was created,
       * that is used to store the name of the queue on which it resides.
       * Since we know exactly what queue it lives on (the one pointed to
       * by 'queue', to be exact), we can free the storage and point the
       * 'job->qname' at 'queue->qname'.  Also store a reference to the
       * owner queue in the job itself.
       */
      free(job->qname);
      job->qname = queue->qname;
      job->queue = queue;

      /*
       * Turn off the flag -- job->qname is now a reference to a queue.
       */
      job->flags &= ~JFLAGS_QNAME_LOCAL;
      }

    /* Now, count the jobs and increment the correct statistic. */
    moved ++;

    switch (job->state)
      {

      case 'R':
        running ++;

        /*
         * Adjust the time of any job that has run over its expected
         * time to the JOB_OVERTIME_SLOP.
         */

        if (job->time_left < 0)
          {
          job->time_left = JOB_OVERTIME_SLOP;
          }

        /* If this job will be running the longest, note that. */
        if ((longest == NULL) ||
            (job->time_left > longest->time_left))
          {
          longest = job;
          }

        break;

      case 'Q':
        queued ++;
        break;

      case 'H':
        held ++;
        break;

      default:
        other ++;
        break;
      }

    }

  /*
   * If any jobs were running, then set the empty_by time to the absolute
   * time (in seconds) when all jobs should be completed.  If none are
   * running, then set empty_by time to 0.
   */
  if (longest)
    queue->empty_by = schd_TimeNow + longest->time_left;
  else
    queue->empty_by = 0;

  return (moved);
  }

int queue_sanity(Queue *queue)
  {
  char   *id = "queue_sanity";
  Job    *job;
  int     queued, running;
  int     is_sane;

  is_sane = 1; /* Assume the queue is sane for now. */

  /*
   * Count running and queued jobs and make sure the numbers match up.
   */
  queued = running = 0;

  for (job = queue->jobs; job != NULL; job = job->next)
    {
    switch (job->state)
      {

      case 'R':
        running ++;
        break;

      case 'Q':
        queued ++;
        break;

      default:
        /* Empty */
        break;
      }
    }

  if (queue->running != running)
    {
    sprintf(log_buffer,
            "Queue '%s' expected %d running jobs, but found %d",
            queue->qname, queue->running, running);
    log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    is_sane = 0;
    }

  if (queue->queued != queued)
    {
    sprintf(log_buffer,
            "Queue '%s' expected %d queued jobs, but found %d",
            queue->qname, queue->queued, queued);
    log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    is_sane = 0;
    }

  if (queue->running && (queue->empty_by < schd_TimeNow))
    {
    sprintf(log_buffer,
            "Queue '%s' was expected to be empty %ld seconds ago",
            queue->qname, (long)(schd_TimeNow - queue->empty_by));
    log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    is_sane = 0;
    }

  return (is_sane);
  }

int
get_node_status(void)
  {
  char *id = "get_node_status";
  QueueList *qptr;
  int local_errno = 0;
  Batch_Status *bs, *bsp;
  AttrList *attr;
  static AttrList alist[] = {{NULL, ATTR_NODE_state, "", ""}};

  /* Query the server for status of all nodes, and then save this
   * info in the appropraite queue struct.
   */

  if ((bs = pbs_statnode_err(connector, NULL, alist, NULL, &local_errno)) == NULL)
    {
    sprintf(log_buffer, "pbs_statnode failed: %d", local_errno);
    log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    return (-1);
    }

  /* First lets assume all nodes are down; later we will revise
   * this if we learn otherwise; we want to assume down so that
   * we don't get hung trying to connect to a hung node later.
   */
  for (qptr = schd_BatchQueues; qptr != NULL; qptr = qptr->next)
    qptr->queue->flags |= QFLAGS_NODEDOWN;

  /* Process the list of nodes returned by the server. */
  for (bsp = bs; bsp != NULL; bsp = bsp->next)
    {
    for (attr = bsp->attribs; attr != NULL; attr = attr->next)
      {

      if ((strstr(attr->value, ND_free))    ||
          (strstr(attr->value, ND_busy))    ||
          (strstr(attr->value, ND_reserve)) ||
          (strstr(attr->value, "job-")))
        {

        for (qptr = schd_BatchQueues; qptr != NULL; qptr = qptr->next)
          {
          if (strstr(qptr->queue->exechost, bsp->name))
            {
            qptr->queue->flags &= ~QFLAGS_NODEDOWN;
            break;
            }
          }
        }
      else
        {
        sprintf(log_buffer, "%s (state=%s)",
                bsp->name, attr->value);
        log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id,
                   log_buffer);
        DBPRT(("%s: %s\n", id, log_buffer));
        }
      }
    }

  pbs_statfree(bs);

  return (0);
  }

/* schd_get_queue_memory - query queue memory limit from the server.
 */
size_t schd_get_queue_memory(char *qName)
  {
  char   *id = "schd_get_queue_limits";
  size_t  mem_max, mem_default;
  Batch_Status *bs;
  int local_errno = 0;
  AttrList *attr;
  static AttrList alist[] =
    {   {&alist[1],  ATTR_rescdflt, "", ""},
    {NULL,       ATTR_rescmax, "", ""}
    };

  mem_default  = (size_t)0;
  mem_max  = (size_t)0;

  /* Ask the server for information about the specified queue. */

  if ((bs = pbs_statque_err(connector, qName, alist, NULL, &local_errno)) == NULL)
    {
    sprintf(log_buffer, "pbs_statque failed, \"%s\" %d",
            qName, local_errno);
    log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    return (UNSPECIFIED);
    }

  /* Process the list of attributes returned by the server. */

  for (attr = bs->attribs; attr != NULL; attr = attr->next)
    {
    /* Queue maximum resource usage. */
    if (!strcmp(attr->name, ATTR_rescmax))
      {
      if (!strcmp("mem", attr->resource))
        {
        mem_max = schd_val2byte(attr->value);
        continue;
        }

      continue;
      }

    if (!strcmp(attr->name, ATTR_rescdflt))
      {
      if (!strcmp("mem", attr->resource))
        {
        mem_default = schd_val2byte(attr->value);
        continue;
        }
      }

    /* Ignore anything else */
    }

  pbs_statfree(bs);

  if (mem_default != (size_t)0)
    return(mem_default);

  if (mem_max != (size_t)0)
    return(mem_max);

  return (UNSPECIFIED);
  }

void schd_calc_suspended_jobs(void)
  {
  Job *job, *next;
  Queue *queue, *schd_find_queue(char *exechost);

  /* walk the job list and adjust resources "assigned" to suspended jobs.
   */

  for (job = schd_SubmitQueue->queue->jobs; job != NULL; job = next)
    {
    next = job->next;

    /*
     * Adjust the count of allocated resources for jobs that are
     * suspended. The queue still reports these as being used,
     * but since the job is suspended, we can reuse the resources
     * (since what was why we suspended them to begin with!).
     */

    if (job->flags & JFLAGS_SUSPENDED)
      {

      queue = schd_find_queue(job->exechost);

      if (queue->flags & QFLAGS_NODEDOWN)
        continue;

      if (queue != NULL)
        {
        queue->rsrcs->ncpus_alloc  -= job->ncpus;
        queue->ncpus_assn          -= job->ncpus;
        queue->rsrcs->mem_alloc    -= job->memory;
        queue->mem_assn            -= job->memory;
        queue->running             -= 1;
        queue->rsrcs->njobs        -= 1;
        }
      }
    }
  }

Queue *schd_find_queue(char *exechost)
  {
  QueueList *qptr;

  for (qptr = schd_BatchQueues; qptr != NULL; qptr = qptr->next)
    {

    if (!strcmp(qptr->queue->exechost, exechost))
      {
      return(qptr->queue);
      }
    }

  return(NULL);
  }
