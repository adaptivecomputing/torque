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

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pbs_ifl.h"
#include "log.h"

/* Scheduler header files */
#include "toolkit.h"
#include "gblxvars.h"

/*
 * Attempt to find a queue in which this waiting or special job will
 * be able to run.  Also take into account whether it is the shortest
 * wait or not.  Given the choice between two equivalent wait times,
 * try to choose the smallest queue for the job.
 *
 * Return the expected time at which the queue will be sufficiently
 * drained to run 'job' in qlist->queue->drainby. If there is no queue
 * capable of running the job, do not modify Q->drain_by, and return
 * (Queue *)NULL.
 */
Queue *
schd_find_drain(QueueList *qlist, Job *job)
{
    char   *id = "schd_find_drain";
    QueueList *qptr;
    Queue  *shortest, *queue;
    Job    *rjob;
    char    buff[256];
    time_t  drainby, mintime;
    int     running;
    int	    drainnodes;
    int	    freenodes;

    /*
     * Find the smallest, shortest-wait queue in which this job will
     * fit.  If it is empty, great.  If not, mark it to be drained,
     * in anticipation of the job being run soon. Also update the
     * count of nodes that need to be reserved for this job.
     */

    shortest = NULL;
    drainby  = 0;
    mintime  = 0;

    for (qptr = qlist; qptr != NULL; qptr = qptr->next) {

	queue = qptr->queue;

	/* The job will never fit in this queue. */
	if (!schd_job_fits_queue(job, queue, NULL))
	    continue;

	/* Ignore stopped or disabled queues */
	if (queue->flags & (QFLAGS_STOPPED|QFLAGS_DISABLED))
	    continue;

	/* Is this job's owner allowed to run in this queue? */
	if ((queue->flags & QFLAGS_USER_ACL) &&
	    (!schd_useracl_okay(job, queue, NULL)))
	{
	    continue;
	}

	/* 
	 * Are there sufficient nodes available to run this job already?  If
	 * so, there is no need to drain the queue to run this job.  Return
	 * NULL - no action is necessary.
	 */
	if ((queue->nodes_max - queue->nodes_assn) >= job->nodes)
	    return (NULL);

	/*
	 * The job cannot be run in this queue at the expected drain
	 * time.
	 */
	drainby = schd_TimeNow;

	running = queue->running;
	if (running == 0) {
	    /* "This can't happen." */
	    DBPRT(("%s: Queue %s not running, but too few nodes for job %s!\n",
		id, queue->qname, job->jobid));
	    continue;	/* Move on to the next queue. */
	}

	/*
	 * Walk down the list of running jobs, counting up the resources that 
	 * will be available after each job completes.  At each step, find 
	 * out if the waiting job will be able to run then.
	 *
	 * Note that this assumes the jobs will be sorted by ending time
	 * from soonest to longest ending.
	 */

	/* Start out with what is available now. */
	freenodes = queue->nodes_max - queue->nodes_assn;
	drainnodes = freenodes;

	for (rjob = queue->jobs; rjob != NULL; rjob = rjob->next) {
	    /* Not running -- don't bother. */
	    if (rjob->state != 'R')
		continue;
	    
	    /*
	     * Pretend that the running job has finished.  If the waiting
	     * job will be able to run at this time, then break out and 
	     * start this queue draining.
	     */
	    running --;
	    drainnodes += rjob->nodes;

	    if (running < 0) {
		DBPRT(("Queue %s RAN OUT OF RUNNING JOBS!?\n", queue->qname));
		rjob = NULL;
		break;
	    }

	    /* Would there be too many jobs running if this job were to run? */
	    if ((queue->maxrun != UNSPECIFIED) && (running >= queue->maxrun)) 
		continue;

	    /*
	     * If there are not yet sufficient resources, then go on to the 
	     * next running job (if there is one).
	     */
	    if (drainnodes < job->nodes)
		continue;

	    /*
	     * Calculate the absolute time at which this job is expected to 
	     * complete.  If the waiting job will not be able to run at that 
	     * future time, then go on to the next running job.
	     */
	    drainby = schd_TimeNow + rjob->time_left;

	    /*
	     * If we've walked past the empty_by time for the queue, then 
	     * the empty_by has been pushed forward.  This will be done to 
	     * support draining a few jobs from a queue.  That's it -- 
	     * don't bother going further.
	     */
	    if (drainby > queue->empty_by) {
		DBPRT(("exceeds '%s' empty_by time\n", queue->qname));
		DBPRT(("%s: %s already draining for another job.\n", id, 
		    queue->qname));

		break;
	    }

	    if (!schd_dedicated_can_run(job, queue, drainby, buff))
		continue;

	    if (schd_primetime_limits(job, queue, drainby, buff))
		continue;

	    /* Stop examining waiting jobs. */
	    break;
	}

	/*
	 * If 'rjob' is non-NULL, it points to a job in the current queue.  
	 * The waiting job will be able to run when this job ompletes.
	 * If not, then the job cannot run in this queue.  If this is the 
	 * case, go on to the next queue.
	 */
	if (rjob == NULL)
	    continue;
	else
	    DBPRT(("%s: %s can run in %s when %s completes (in %s).\n", 
		id, job->jobid, queue->qname, rjob->jobid, 
		schd_sec2val(rjob->time_left)));

	/*
	 * If there is no shortest queue yet, then this is it.  Otherwise,
	 * see if the wait for this queue will be less than the current
	 * shortest one.
	 */
	if ((shortest == NULL) || (drainby < mintime)) {
	    mintime  = drainby;
	    shortest = queue;
	}
    }

    if (shortest) {
	/* reserve the number of currently free nodes that will be needed
	 * later by this job. We only need to reserve what we will actully
	 * use. Set the count of these reserved nodes in the Q struct.
	 */
	if (freenodes)
	    shortest->nodes_rsvd = freenodes - (drainnodes - job->nodes);
	else
	    shortest->nodes_rsvd = freenodes;

	if (shortest->running)
	    shortest->drain_by = mintime;
	else
	    shortest->drain_by = schd_TimeNow;

    }

    return (shortest);
}
