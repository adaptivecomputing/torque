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

extern int connector;

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
    int    allfull, jobsrun;

    jobsrun = 0;

    DBPRT(("%s: scheduling queues", id));
    for (qptr = qlist; qptr != NULL; qptr = qptr->next) {
        if (!(qptr->queue->flags & QFLAGS_DISABLED) &&
            !(qptr->queue->flags & QFLAGS_NODEDOWN))
	    DBPRT((" %s@%s", qptr->queue->qname, qptr->queue->exechost));
    }
    DBPRT((".\n"));

    if (jobs == NULL) {
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
    for (job = jobs; job != NULL; job = nextjob) {
	nextjob = job->next;

	/* Ignore any non-queued jobs in the list. */
	if (job->state != 'Q')
	    continue;

	allfull = 1;		/* Assume all queues are full to start. */

	/* First check if there are *any* queues in which we *might* be
 	 * able to run a job. If not, then there's no point in going any
	 * further.
	 */
	for (qptr = qlist; qptr != NULL; qptr = qptr->next) {

	    if (qptr->queue->rsrcs == NULL            ||
	       (qptr->queue->flags & QFLAGS_NODEDOWN) ||
	       (qptr->queue->flags & QFLAGS_FULL)     ||
	       (qptr->queue->flags & QFLAGS_DISABLED) ||
	       (qptr->queue->flags & QFLAGS_STOPPED)) {
		continue;

	    }
	    if (!schd_evaluate_system(qptr->queue->rsrcs, reason)) {
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

	/* If all queues are full, hop outta here. */
	if (allfull)
	    break;

	queue = find_best_exechost(job, qlist, reason);
	if (queue == NULL) {
	    /* Hummm, we didn't find an acceptable queue, so lets comment
	     * the job with the supplied reason, and go on to the next job
	     * on the list.
	     */
    	    schd_comment_job(job, reason, JOB_COMMENT_OPTIONAL);
	    continue;
	}

	/* Attempt to run this job on the above-supplied queue */
	if (schd_run_job_on(job, queue, queue->exechost, SET_JOB_COMMENT)) {
	    (void)sprintf(log_buffer,"Unable to run batch job %s on queue %s",
		job->jobid, queue->qname);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,id,log_buffer);
	    DBPRT(("%s: %s\n", id, log_buffer))
	    queue->flags |= QFLAGS_NODEDOWN;
	} else {

	    /* Lastly, account for this job's resource consumption. */
	    schd_charge_job(job, queue, queue->rsrcs);
	    jobsrun ++;
	}
    }

    /*
     * If the loop exited because all the queues are full, then note
     * this on any remaining jobs.  
     */
    if (allfull && (job != NULL)) {
	DBPRT(("All queues full! Commenting remaining jobs...\n"));
	for (/* job list from above */; job != NULL; job = job->next)
	    schd_comment_job(job, schd_JobMsg[NO_RESOURCES], 
	        JOB_COMMENT_OPTIONAL);
    }

    return (jobsrun);
}

int schd_cpus_left(Job *job, Resources *rsrcs)
{
    return (rsrcs->ncpus_total - (rsrcs->ncpus_alloc + job->ncpus));
}

Queue *find_best_exechost(Job *job, QueueList *qlist, char *reason)
{
    int     best_ncpus, leftover_ncpus;
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

#define PACKING_ROUNDROBIN 1
#ifdef PACKING_ROUNDROBIN
    best_ncpus = -1;
#else /* PACKING_PACKNODE */
    best_ncpus = 100000;
#endif
    best_queue = NULL;

    for (qptr = qlist; qptr != NULL; qptr = qptr->next) {
        queue = qptr->queue;

        /* 
         * If this queue is missing its resource info, or if its
 	 * STOPPED, etc.,  skip it.
         */
        if (qptr->queue->rsrcs == NULL            ||
	   (qptr->queue->flags & QFLAGS_NODEDOWN) ||
	   (qptr->queue->flags & QFLAGS_DISABLED) ||
	   (qptr->queue->flags & QFLAGS_FULL)     ||
	   (qptr->queue->flags & QFLAGS_STOPPED))
    	      continue;

	if (schd_job_exceeds_fairaccess(job, queue, reason))
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
        if (queue->useracl && (queue->flags & QFLAGS_USER_ACL)) {
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
        if (schd_ENFORCE_PRIME_TIME) {
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
         * We found a queue on which this job can run. Now we need to decide
	 * if this is the best queue for this job. 
         */
	leftover_ncpus = schd_cpus_left(job, queue->rsrcs);

#ifdef PACKING_ROUNDROBIN
	if (leftover_ncpus > best_ncpus) {
	    best_ncpus = leftover_ncpus;
	    best_queue = queue;
	}
#else /* PACKING_PACKNODES */
	if (leftover_ncpus < best_ncpus) {
	    best_ncpus = leftover_ncpus;
	    best_queue = queue;
	    if (leftover_ncpus == 0)
	        return(best_queue);	/* Can get much better than this! */
	}
#endif

    }
    return(best_queue);
}
