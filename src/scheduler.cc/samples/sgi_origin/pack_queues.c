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

/* PBS header files */

#include "pbs_error.h"
#include "pbs_ifl.h"
#include "log.h"

/* Scheduler header files */

#include "toolkit.h"
#include "gblxvars.h"
#include "msgs.h"

#include "hpm.h"

extern int connector;

/*
 * Given a list of jobs, ordered from most-eligible to least-eligible to
 * run, attempt to place as many of them as possible into the queues listed
 * in qlist.  Jobs that will not currently fit are ignored (i.e. the entire
 * list is processed, running each job in order that fits).
 *
 * Jobs are run by run_job_on() as they are found on the list.  Resources 
 * must be supplied because the jobs are tested against them in 
 * 'schd_resource_limits()', and they are updated by sched_run_jobs_on().
 *
 * This function returns the number of jobs run, or -1 on error.
 */
int 
schd_pack_queues(Job *jobs, QueueList *qlist, char *reason)
{
    char   *id = "schd_pack_queues";
    Job    *job, *nextjob;
    QueueList *qptr;
    Queue *queue, *firstfit;
    int    allfull, jobsrun, rerun;
    char   str[PBS_MAXHOSTNAME + 32];
#ifdef NODEMASK
    int    nodesreq;
#endif	/* NODEMASK */

    jobsrun = 0;

    DBPRT(("%s: scheduling queues", id));
    for (qptr = qlist; qptr != NULL; qptr = qptr->next) {
	DBPRT((" %s@%s", qptr->queue->qname, qptr->queue->exechost));
    }
    DBPRT((".\n"));

    if (jobs == NULL) {
	DBPRT(("No jobs available for QueueList %s%s - all done!", 
	    qlist->queue->qname, qlist->next ? " ..." : ""));
	return (0);
    }

    /* This is the first run through this part of the code. */
    rerun = 0;

    /*
     * Consider jobs from the list of queues.  For each job, if it appears
     * eligible to run, try to find a queue on which to place it.
     *
     * Note that schd_run_job_on() may remove the job from the list, so 
     * this function must keep track of the current job's next pointer.
     */
run_job_list:

    for (job = jobs; job != NULL; job = nextjob) {
	nextjob = job->next;

	/* Ignore any non-queued jobs in the list. */
	if (job->state != 'Q')
	    continue;

	/*
	 * Would this job cause the user to exceed group's current allocation?
	 */
	if (schd_ENFORCE_ALLOCATION && schd_TimeNow >= schd_ENFORCE_ALLOCATION)
	{
	    if (job->group != NULL) {
		if (schd_is_over_alloc(job->group)) {

		    /* 
		     * schd_reject_over_alloc() will delete the job from
		     * PBS, and also from the queue's job list.
		     */
		    if (schd_reject_over_alloc(job)) {
			(void)sprintf(log_buffer,
			    "reject_over_alloc() failed for job %s\n", 
				job->jobid);
			log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, 
			    id, log_buffer);
		    }

		    continue;
		}
	    } else {
		(void)sprintf(log_buffer, 
		    "ENFORCE ALLOCATION set, but job %s has no group field", 
		    job->jobid);
		log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, 
		    log_buffer);

		continue;
	    }
	}

	/* See if any queues are available for scheduling now. */
	allfull = 1;		/* Assume all queues are full to start. */

	for (qptr = qlist; qptr != NULL; qptr = qptr->next) {

	    if ((qptr->queue->flags & QFLAGS_FULL == 0) && 
		((qptr->queue->rsrcs == NULL) ||
		(!schd_evaluate_system(qptr->queue->rsrcs, reason))))
	    {
		DBPRT(("%s: evaluate_system: %s\n", id, 
		    qptr->queue->rsrcs ? reason : "No resources"));
		DBPRT(("%s: Marking queue %s@%s full\n", id,
		    qptr->queue->qname, qptr->queue->exechost));
		qptr->queue->flags |= QFLAGS_FULL;
	    }

	    /* 
	     * If an empty queue has not yet been found, check this one.
	     * if this queue is not full, then all queues are not full.
	     * Note this and continue - if all queues are full at the
	     * end of this exercise, we will give up.
	     */
	    if (allfull)
		if (!schd_check_queue_limits(qptr->queue, NULL))
		    allfull = 0;
	}

	/* 
	 * Check each queue in the list in order, to see if this job 
	 * will fit, and if it should be run now.
	 */
	firstfit = NULL;	/* Haven't found the first queue it fits. */
	for (qptr = qlist; qptr != NULL; qptr = qptr->next) {
	    queue = qptr->queue;
	    /*
	     * If this is the first queue that this job will fit in, then
	     * note it.  This is the "best fit" queue (hopefully) and the
	     * job comment should not be modified unless the comment refers
	     * to this queue.
	     *
	     */
	    if (!schd_job_fits_queue(job, queue, reason))
		continue;
	    
	    /*
	     * If this job has a user access control list, check that this
	     * job can be allowed in it.
	     */
	    if (queue->useracl && (queue->flags & QFLAGS_USER_ACL)) {
		if (!schd_useracl_okay(job, queue, reason)) {
		    DBPRT(("%s: %s %s\n", id, job->jobid, reason));
		    continue;
		}
	    }

	    if (!firstfit)
		firstfit = queue;

	    /*
	     * Check that this job will not overrun a dedicated time.  The '0'
	     * indicates that we are interested in "now".
	     */
	    if (schd_ENFORCE_DEDTIME && schd_TimeNow >= schd_ENFORCE_DEDTIME) {
		if (!schd_dedicated_can_run(job, queue, 0, reason)) {
		    if (firstfit == queue)
			schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);
		    continue;
		}
	    }

	    /*
	     * Check primetime limits.  If it is primetime now, and the job
	     * will complete before primetime, limit it to 1 hour.  If it will
	     * overrun primetime, be sure that the amount that falls within
	     * primetime will not violate the primetime limit.
	     * Special jobs are not subject to primetime walltime limits.
	     */
	    if (schd_ENFORCE_PRIME_TIME && 
		(schd_TimeNow >= schd_ENFORCE_PRIME_TIME) &&
		(schd_SMALL_JOB_MAX <= 0))
	    {
		if (!(job->flags & JFLAGS_PRIORITY)) {
		    if (schd_primetime_limits(job, queue, 0, reason)) {
			schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);

			/* Note that a job could have run if it were not pt. */
			if (!rerun)
			    queue->flags |= QFLAGS_NPT_JOBS;

			continue;
		    }
		}
	    }

	    /*
	     * Check that this job will complete before the beginning of 
	     * nonprime. The '0' indicates that we are interested in "now".
	     */
	    if (schd_NONPRIME_DRAIN_SYS) {
		/* Note: Special jobs are not subject to this restriction */
		if (!(job->flags & JFLAGS_PRIORITY)) {
		    if (!schd_finish_before_np(job, queue, 0, reason)) {
			schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);

			/* Note that a job could have run if it were not pt. */
			if (!rerun)
			    queue->flags |= QFLAGS_NPT_JOBS;

			continue;
		    }
		}
	    }

	    /*
	     * Check that the queue is actually available to pack jobs
	     * into.  Although it was checked above, the above test is
	     * very inexpensive, so it's not a big deal to do it again. 
	     */

	    if (schd_check_queue_limits(queue, reason)) {
		if (firstfit == queue)
		    schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);
		continue;
	    }

	    /* 
	     * Check this job against the execution resource limits. 
	     * There is no point to going any further if this job would
	     * overrun the system limits.
	     * It is possible for the size of the queues to be larger than
	     * the available resources on the machine (i.e. a node board
	     * goes down).  Make sure that jobs are not considered that will
	     * not be able to run within the system's current resources.
	     */

	    if (!schd_resources_avail(job, queue->rsrcs, reason) ||
		schd_resource_limits(job, queue->rsrcs, reason))
	    {
		if (firstfit == queue)
		    schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);

		/* Continue to the next job. */
		continue;
	    }

	    /*
	     * Check that this job will complete before the beginning of 
	     * nonprime. The '0' indicates that we are interested in "now".
	     * Note: Special jobs are not subject to this restriction.
	     */
	    if (schd_NONPRIME_DRAIN_SYS && !(job->flags & JFLAGS_PRIORITY)) {
		if (!schd_finish_before_np(job, queue, 0, reason)) {
		   schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);
		   continue;
		}
	    }

	    /*
	     * If the queue is being drained, don't run the job in it unless
	     * (1) there are unreserved resources available, or 
	     * (2) it would *not* increase the time it will take to drain
	     * the queue. Note that drain_by is the absolute time, while
	     * the job->time_left is relative to now.
	     */

	    /* (1) are there unreserved nodes available?
	     */
	    if (job->nodes > 
	       (queue->nodes_max - (queue->nodes_assn + queue->nodes_rsvd))) {
		   /* no, so (2) see if we can backfill with this job... */

	        if (queue->flags & QFLAGS_DRAINING) {
		    if ((schd_TimeNow + job->walltime) > queue->drain_by) {
		        (void)sprintf(reason, "Queue %s is being drained.",
			    queue->qname);
		        if (firstfit == queue)
			    schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);
		        continue;
		    }
	        }
	    }
	    
	    /*
	     * Queue can accept a job, provided the job does not overrun the
	     * queue limits.
	     */
	    if (schd_user_limits(job, queue, reason)) {
		if (firstfit == queue)
		    schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);
		continue;
	    }

	    /*
	     * Check for fragmentation in the queue, and don't run it if so.
	     * fragment_okay() returns -1 if queue is nonsensical, 1 if this
	     * job would induce or perpetuate fragmentation.  If this job is
	     * waiting, we don't care - just run it.
	     */
	    if (schd_AVOID_FRAGS && !(job->flags & JFLAGS_WAITING)) {
		if (!schd_fragment_okay(job, queue, reason)) {
		    if (firstfit == queue)
			schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);
		    continue;
		}
	    }

#ifdef NODEMASK
	    /*
	     * Attempt to generate a nodemask for the job, if required.
	     * If a nodemask cannot be generated, ignore this job.
	     */
	    if (queue->flags & QFLAGS_NODEMASK) {
		nodesreq = job->nodes;

		if (!schd_alloc_nodes(nodesreq, queue, &(job->nodemask))) {
		    DBPRT(("%s: Can't alloc %d nodes from queue %s, "
			"skipping job %s\n", id, nodesreq, queue->qname, 
			job->jobid));
		    continue;
		}
	    }
#endif	/* NODEMASK */

	    /*
	     * If this job requires HPM support for its execution, check
	     * the state of the hardware counters on the queue's exechost.
	     * If that fails, comment the job, and go on to the next queue.
	     */

	    if (schd_MANAGE_HPM && job->flags & JFLAGS_NEEDS_HPM) {
		if (!(queue->rsrcs->flags & RSRCS_FLAGS_HPM_USER)) {
		    /* 
		     * Attempt to set the HPM counters on the execution host
		     * for this queue to user mode.
		     */
		    if (schd_hpm_ctl(queue->rsrcs, HPM_SETUP_USERMODE, reason))
		    {
			if (firstfit == queue)
			    schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);
			continue;
		    }
		}
	    }

	    /*
	     * Found a queue on which this job can run.  Attempt to run it.
	     */
	    break;
	}

	/*
	 * If the job was unable to fit on all of the queues, go on to the
	 * next one.  For "strict" packing, make this a 'break' and it will
	 * stop processing jobs when it finds the first one that does not
	 * fit.
	 */
	if (!qptr) {
	    /*
	     * If the job did not fit in any of the provided queues, assume
	     * that the queue it wants is not available.  Provide a comment,
	     * even if it is sort of vague.
	     */
	    if (firstfit == NULL) {
		schd_comment_job(job, schd_JobMsg[NO_RESOURCES], 
			JOB_COMMENT_OPTIONAL);
	    }
	    continue;
	}

	/* There is a queue free enough to run this job. */
	if (schd_run_job_on(job, qptr->queue, qptr->queue->exechost, 
	    SET_JOB_COMMENT))
	{
	    (void)sprintf(log_buffer,
		"Unable to run batch job %s on queue %s",
		job->jobid, qptr->queue->qname);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

	    return (-1);
	}

	/*
	 * Account for the job's impact on the queue.
	 * This includes subtracting the job's resource requests from the
	 * queue's available resources, as well as updating the available
	 * nodemask for the queue.
	 */

	schd_charge_job(job, queue, queue->rsrcs);

	jobsrun ++;
    }

    /*
     * Adjust the observance of primetime on the queues, if necessary.  Do
     * it only if the queue is observing primetime, it has been idle for a
     * while(*), it's close enough(*) to primetime, and there were some jobs
     * that could have been run if primetime had been not-observed.
     */
    if (!rerun && 
	(schd_ENFORCE_PRIME_TIME && schd_TimeNow >= schd_ENFORCE_PRIME_TIME) && 
	schd_NP_DRAIN_BACKTIME > 0 && 
	schd_prime_time(0) &&
	schd_secs_til_nonprime(0) <= schd_NP_DRAIN_BACKTIME)
    {
	for (qptr = qlist; qptr != NULL; qptr = qptr->next) {
	    queue = qptr->queue;

	    /* Were there any jobs that could have run if it were non-pt? */
	    if (!(queue->flags & QFLAGS_NPT_JOBS))
		continue;

	    if (queue->running)
		continue;

	    /* Already disabled or idle - ignore this queue. */
	    if (!queue->observe_pt)
		continue;

	    /* If a minimum idle time is given, check it. */
	    if ((schd_NP_DRAIN_IDLETIME > 0) && 
		(schd_TimeNow - queue->idle_since) <= schd_NP_DRAIN_IDLETIME)
		continue;

	    /* 
	     * Queue has been idle for some time.  Start non-primetime early 
	     * so those jobs that were refused can start running now.
	     */
	    (void)sprintf(log_buffer,
		"Turning off prime-time enforcement on queue %s\n", 
		    queue->qname);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

	    queue->observe_pt = 0;
	    rerun ++;
	}

	if (rerun) {
	    (void)sprintf(log_buffer,
		"Prime-time enforcement adjusted - reconsidering active jobs.");
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s: %s\n", id, log_buffer));

	    goto run_job_list;
	}
    }

    return (jobsrun);
}
