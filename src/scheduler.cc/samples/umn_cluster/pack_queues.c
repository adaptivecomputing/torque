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
     * Consider jobs from the list of queues. For each job, if it appears
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

	strcpy(reason, "Requested architecture not currently available");
	queue = schd_static_backfill(job, qlist, reason);

	if (queue == NULL) {
	    /* Hummm, there is not a queue currently available in which
	     * to run this job. But resources have been researved for
	     * this job by the static_backfill() routine, and the job
	     * comment contains the estimated start time and the reason
	     * why it can't run now. Comment the job and go on to the
	     * next job on the list.
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
	} else {

	    /* Lastly, account for this job's resource consumption. */
	    schd_charge_job(job, queue, queue->rsrcs);
	    jobsrun ++;
	}
    }

    schd_print_schedule();

    return (jobsrun);
}

Queue *schd_static_backfill(Job *job, QueueList *qlist, char *reason)
{
    char   *id = "schd_static_backfill";
    int     best_ncpus;
    time_t best_start, start_time;
    Queue  *queue, *best_queue;
    QueueList *qptr;

    /* 
     * Look at all the Queues in the list that are currently available
     * for scheduling (as determined in schd_pack_queues() above); find
     * the queue/exechost on which the job will run soonest; if it not
     * possible to run this job at this time, determine when we *will*
     * run the job, update the current 'schedule' of jobs with this info,
     * and then save that time in the reason array with an explantion
     * for the user.
     *
     * Otherwise, return a pointer to the queue to use.
     */

    best_start = 0;
    best_queue = NULL;

    for (qptr = qlist; qptr != NULL; qptr = qptr->next) {
        queue = qptr->queue;

	if (!schd_queue_available(job, queue, reason))
	    continue;

	start_time = schd_when_can_job_start(job, queue, reason);

	if (start_time == 0)
	    continue;

	if (best_start == 0) {
	    best_start = start_time;
	    best_queue = queue;
	} 
	else if (start_time < best_start) {
	    best_start = start_time;
	    best_queue = queue;
	}
    }



    if (best_start != 0) {
	/* It will be possible to run this run (at some point, possibly
	 * immediately, so add an entry to the global JobSchedule table,
	 * and prepend the estimated start time to the reason string.
	 */
        schd_update_schedule(job, best_queue, best_start, reason);
    }

    if (best_start == schd_TimeNow)
        return(best_queue);

   return(NULL);
}
