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

extern int connector;

static int  queue_claim_jobs(Queue *queue, Job **joblist_ptr);
static int  queue_sanity(Queue *queue);

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
    int     moved = 0, istrue;
    Batch_Status *bs;
    AttrList *attr;
    static AttrList alist[] =
    {
	{&alist[1],  ATTR_start,	"", ""},
	{&alist[2],  ATTR_enable,	"", ""},
	{&alist[3],  ATTR_count,	"", ""},
	{&alist[4],  ATTR_maxuserrun,	"", ""},
	{&alist[5],  ATTR_rescavail,	"", ""},
	{&alist[6],  ATTR_rescassn,	"", ""},
	{&alist[7],  ATTR_rescdflt,	"", ""},
	{&alist[8],  ATTR_rescmax,	"", ""},
	{&alist[9],  ATTR_rescmin,	"", ""},
	{&alist[10], ATTR_acluren,	"", ""},
	{&alist[11], ATTR_acluser,	"", ""},
	{NULL,       ATTR_maxrun,	"", ""}
    };
    size_t  mem_default = UNSPECIFIED;
    size_t  mem_assn    = UNSPECIFIED;
    size_t  mem_max     = UNSPECIFIED;
    size_t  mem_min     = UNSPECIFIED;
    int     cpu_default = UNSPECIFIED;
    int     cpu_assn    = UNSPECIFIED;
    int     cpu_max     = UNSPECIFIED;
    int     cpu_min     = UNSPECIFIED;
    int     mppe_default = UNSPECIFIED;
    int     mppe_assn    = UNSPECIFIED;
    int     mppe_max     = UNSPECIFIED;
    int     mppe_min     = UNSPECIFIED;
    int     nodes_from_cpu, nodes_from_mem;


    queue->running	= UNSPECIFIED;
    queue->queued	= UNSPECIFIED;
    queue->maxrun	= UNSPECIFIED;
    queue->userrun      = UNSPECIFIED;

    queue->nodes_max	= UNSPECIFIED;
    queue->nodes_min	= UNSPECIFIED;
    queue->nodes_default= UNSPECIFIED;
    queue->nodes_assn	= UNSPECIFIED;
    queue->nodes_rsvd	= UNSPECIFIED;
    queue->wallt_max	= UNSPECIFIED;
    queue->wallt_min	= UNSPECIFIED;
    queue->wallt_default= UNSPECIFIED;

    queue->flags	= 0;
    queue->rsrcs	= NULL;
    

    if (queue->jobs) {
	DBPRT(("%s: found jobs on queue '%s'!  Freeing them...\n", id, 
	    queue->qname));
	schd_free_jobs(queue->jobs);
    }

    if (queue->useracl) {
	DBPRT(("%s: found user ACL list on queue '%s'!  Freeing it...\n", id, 
	    queue->qname));
	schd_free_useracl(queue->useracl);
    }

    queue->jobs         = NULL;
    queue->useracl	= NULL;

    /* Ask the server for information about the specified queue. */

    if ((bs = pbs_statque(connector, queue->qname, alist, NULL)) == NULL) {
	sprintf(log_buffer, "pbs_statque failed, \"%s\" %d",
		queue->qname, pbs_errno);
	log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));
	return (-1);
    }

    /* Process the list of attributes returned by the server. */
	
    for (attr = bs->attribs; attr != NULL; attr = attr->next) {

	/* Is queue started? */
	if (!strcmp(attr->name, ATTR_start)) {
	    if (schd_val2bool(attr->value, &istrue) == 0) {
		if (istrue)	/* if true, queue is not stopped. */
		    queue->flags &= ~QFLAGS_STOPPED;
		else 
		    queue->flags |= QFLAGS_STOPPED;
	    } else {
		DBPRT(("%s: couldn't parse attr %s value %s to boolean\n", id,
		    attr->name, attr->value));
	    }
	    continue;
	}

	/* Is queue enabled? */
	if (!strcmp(attr->name, ATTR_enable)) {
	    if (schd_val2bool(attr->value, &istrue) == 0) {
		if (istrue)	/* if true, queue is not disabled. */
		    queue->flags &= ~QFLAGS_DISABLED;
		else 
		    queue->flags |= QFLAGS_DISABLED;
	    } else {
		DBPRT(("%s: couldn't parse attr %s value %s to boolean\n", id,
		    attr->name, attr->value));
	    }
	    continue;
	}

	/* How many jobs are queued and running? */
	if (!strcmp(attr->name, ATTR_count)) {
	    queue->queued = schd_how_many(attr->value, SC_QUEUED);
	    queue->running = schd_how_many(attr->value, SC_RUNNING);
	    continue;
	}

	/* Queue-wide maximum number of jobs running. */
	if (!strcmp(attr->name, ATTR_maxrun)) {
	    queue->maxrun = atoi(attr->value);
	    continue;
	}

	/* Per-user maximum number of jobs running. */
	if (!strcmp(attr->name, ATTR_maxuserrun)) {
	    queue->userrun = atoi(attr->value);
	    continue;
	}

	/* Is there an enabled user access control list on this queue? */
	if (!strcmp(attr->name, ATTR_acluren)) {
	    if (schd_val2bool(attr->value, &istrue) == 0) {
		if (istrue)	/* if true, queue has an ACL */
		    queue->flags |= QFLAGS_USER_ACL;
		else 
		    queue->flags &= ~QFLAGS_USER_ACL;
	    } else {
		DBPRT(("%s: couldn't parse attr %s value %s to boolean\n", id,
		    attr->name, attr->value));
	    }
	    continue;
	}

	if (!strcmp(attr->name, ATTR_acluser)) {
	    if (queue->useracl) {
		DBPRT(("queue %s acluser already set!\n", queue->qname));
		schd_free_useracl(queue->useracl);
	    }
	    queue->useracl = schd_create_useracl(attr->value);
	    continue;
	}

	/* Queue maximum resource usage. */
	if (!strcmp(attr->name, ATTR_rescmax)) {
	    if (!strcmp("mem", attr->resource)) {
		mem_max = schd_val2byte(attr->value);
		continue;
	    }

	    if (!strcmp("ncpus", attr->resource)) {
		cpu_max = atoi(attr->value);
		continue;
	    }

	    if (!strcmp("mppe", attr->resource)) {
		mppe_max = atoi(attr->value);
		continue;
	    }

	    if (!strcmp("walltime", attr->resource)) {
		queue->wallt_max = schd_val2sec(attr->value);
		continue;
	    }

	    continue;
	}

	/* Queue minimum resource usage. */
	if (!strcmp(attr->name, ATTR_rescmin)) {
	    if (!strcmp("mem", attr->resource)) {
		mem_min = schd_val2byte(attr->value);
		continue;
	    }

	    if (!strcmp("ncpus", attr->resource)) {
		cpu_min = atoi(attr->value);
		continue;
	    }

	    if (!strcmp("mppe", attr->resource)) {
		mppe_min = atoi(attr->value);
		continue;
	    }

	    if (!strcmp("walltime", attr->resource)) {
		queue->wallt_min = schd_val2sec(attr->value);
		continue;
	    }

	    continue;
	}

	/* Queue assigned (in use) resource usage. */
	if (!strcmp(attr->name, ATTR_rescassn)) {
	    if (!strcmp("mem", attr->resource)) {
		mem_assn = schd_val2byte(attr->value);
		continue;
	    }

	    if (!strcmp("ncpus", attr->resource)) {
		cpu_assn = atoi(attr->value);
	    }

	    continue;
	}

	if (!strcmp(attr->name, ATTR_rescdflt)) {
	    if (!strcmp("mem", attr->resource))	{
		mem_default = schd_val2byte(attr->value);
		continue;
	    }

	    if (!strcmp("ncpus", attr->resource)) {
		cpu_default = atoi(attr->value);
		continue;
	    }

	    if (!strcmp("mppe", attr->resource)) {
		mppe_default = atoi(attr->value);
		continue;
	    }

	    if (!strcmp("walltime", attr->resource))
		queue->wallt_default = schd_val2sec(attr->value);
	}

	/* Ignore anything else */
    }

    pbs_statfree(bs);

#if defined(sgi)
    /*
     * Calculate values for queue node limits, given memory and cpu values.
     * Note any discrepancies.
     */
    nodes_from_cpu = NODES_FROM_CPU(cpu_default);
    nodes_from_mem = NODES_FROM_MEM(mem_default);
    if (nodes_from_cpu != nodes_from_mem) {
	sprintf(log_buffer, 
	    "%s: Queue '%s' default cpu/mem (%d/%s) convert to %d != %d nodes",
	    id, queue->qname, cpu_default, schd_byte2val(mem_default), 
	    nodes_from_cpu, nodes_from_mem);
	log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));
    }

    nodes_from_cpu = NODES_FROM_CPU(cpu_max);
    nodes_from_mem = NODES_FROM_MEM(mem_max);
    if (nodes_from_cpu != nodes_from_mem) {
	sprintf(log_buffer, 
	    "%s: Queue '%s' maximum cpu/mem (%d/%s) convert to %d != %d nodes",
	    id, queue->qname, cpu_max, schd_byte2val(mem_max), 
	    nodes_from_cpu, nodes_from_mem);
	log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));
    }

    nodes_from_cpu = NODES_FROM_CPU(cpu_min);
    nodes_from_mem = NODES_FROM_MEM(mem_min);
    if (nodes_from_cpu != nodes_from_mem) {
	sprintf(log_buffer, 
	    "%s: Queue '%s' minimum cpu/mem (%d/%s) convert to %d != %d nodes",
	    id, queue->qname, cpu_min, schd_byte2val(mem_min), 
	    nodes_from_cpu, nodes_from_mem);
	log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));
    }

    /*
     * Note: The assigned cpus and memory need not be exactly the same
     * node equivalency.
     */
    if ((cpu_default != UNSPECIFIED) && (mem_default != UNSPECIFIED))
	queue->nodes_default = NODES_REQD(cpu_default, mem_default);
    if ((cpu_max != UNSPECIFIED) && (mem_max != UNSPECIFIED))
	queue->nodes_max     = NODES_REQD(cpu_max, mem_max);
    if ((cpu_min != UNSPECIFIED) && (mem_min != UNSPECIFIED))
	queue->nodes_min     = NODES_REQD(cpu_min, mem_min);
    if ((cpu_assn != UNSPECIFIED) && (mem_assn != UNSPECIFIED))
	queue->nodes_assn    = NODES_REQD(cpu_assn, mem_assn);

#endif /* defined(sgi) */

    /*
     * Move any jobs on this queue from the global list onto the queue's
     * list.  Keep track of when the longest-running job will end, and set
     * the 'empty_by' field to that value.  Maintain the ordering as it was
     * in "schd_AllJobs".
     */

    if (schd_AllJobs)
	moved = queue_claim_jobs(queue, &schd_AllJobs);
    
    if (moved < 0) {
	sprintf(log_buffer, "%s: WARNING! Queue '%s' failed to claim jobs.",
	    id, queue->qname);
	log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));
    }

    if (queue->nodes_assn == UNSPECIFIED)
	queue->nodes_assn = 0;
    if (queue->running    == UNSPECIFIED)
	queue->running    = 0;

    /*
     * Find out if the queue is idle, and if it was not before, set the idle
     * time to now.  If there are running jobs, the queue is not idle at the
     * start of this iteration - set idle_since to 0.
     */
    if (queue->running) {
	queue->idle_since = 0;
    } else {
	if (queue->idle_since == 0)
	    queue->idle_since = schd_TimeNow;
    }

    /*
     * Get the resources for this queue from the resource monitor (if
     * available).  If the resmom is not accessible, disable the queue.
     * If the resources were received okay, compute the available node
     * masks from the resources and jobs.
     * Don't bother with resources for the submit queue.
     */
    if (strcmp(queue->qname, schd_SubmitQueue->queue->qname) != 0)
    {
	queue->rsrcs = schd_get_resources(queue->exechost);
	if (queue->rsrcs != NULL) {
	    /* Account for this queue's resources. */
	    queue->rsrcs->nodes_alloc += queue->nodes_assn;
	    queue->rsrcs->njobs       += queue->running;

	} else {
	    (void)sprintf(log_buffer,
		"Can't get resources for %s@%s - marking unavailable.",
		queue->qname, queue->exechost);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s: %s\n", id, log_buffer));

	    queue->flags |= QFLAGS_DISABLED;
	}
    }

#ifdef DEBUG
    schd_dump_queue(queue, QUEUE_DUMP_JOBS);
#endif /* DEBUG */

    /*
     * It would probably be better to wait for the world to stabilize
     * than to try to impose some artificial order upon it.  Do not do
     * the sanity check if the queue is stopped.
     */
    if ((queue->flags & QFLAGS_STOPPED) == 0) {
	if (!queue_sanity(queue)) {
	    sprintf(log_buffer, "WARNING! Queue '%s' failed sanity checks.",
		queue->qname);
	    log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s: %s\n", id, log_buffer));

	    return (1);
	}
    }

    return (0);
}

#ifdef DEBUG
void 
schd_dump_queue(Queue *queue, int dumpjobs)
{
    Job    *job;
    UserAcl *aclent;
    char    num[32];
    char   *ptr;
    int     columns;

    DBPRT(("\nQueue '%s@%s': %sabled/%sed", 
	queue->qname, queue->exechost,
	(queue->flags & QFLAGS_DISABLED) ? "Dis" : "En",
	(queue->flags & QFLAGS_STOPPED) ? "Stopp" : "Start"));

    DBPRT(("%s%s%s%s ",
	(queue->flags & QFLAGS_FULL) ? "/Full" : "",
	(queue->flags & QFLAGS_MAXRUN) ? "/MaxRun" : "",
	(queue->flags & QFLAGS_DRAINING) ? "/Drain" : "",
	(queue->flags & QFLAGS_USER_ACL) ? "/ACL" : ""));

    if (schd_ENFORCE_PRIME_TIME && schd_TimeNow >= schd_ENFORCE_PRIME_TIME)
	DBPRT(("obsv_pt:%s", queue->observe_pt ? "Yes" : "No"));
    DBPRT(("\n"));

    sprintf(num, "%d", queue->running);
    DBPRT(("  Job counts: %s running, ", 
	queue->running != UNSPECIFIED ? num : "???"));

    sprintf(num, "%d", queue->maxrun);
    DBPRT(("%s max ", queue->maxrun != UNSPECIFIED ? num : "???"));

    sprintf(num, "%d", queue->userrun);
    DBPRT(("(%s/user), ", queue->userrun != UNSPECIFIED ? num : "???"));

    sprintf(num, "%d", queue->queued);
    DBPRT(("%s queued\n", queue->queued != UNSPECIFIED ? num : "???"));

    sprintf(num, "%d", queue->nodes_assn);
    DBPRT(("  Nodes:%s/", queue->nodes_assn != UNSPECIFIED ? num : "???"));

    sprintf(num, "%d", queue->nodes_max);
    DBPRT(("%s", queue->nodes_max != UNSPECIFIED ? num : "???"));

    sprintf(num, "%d", queue->nodes_default);
    DBPRT((" [def %s, ", queue->nodes_default != UNSPECIFIED ? num : "???"));

    sprintf(num, "%d", queue->nodes_min);
    DBPRT(("min %s], ", queue->nodes_min != UNSPECIFIED ? num : "???"));

    DBPRT(("wallt max %s ", (queue->wallt_max != UNSPECIFIED) ?
	    schd_sec2val(queue->wallt_max) : "???"));
    DBPRT(("[def %s ", 
        queue->wallt_default!=UNSPECIFIED ?
	    schd_sec2val(queue->wallt_default) : "???"));
    DBPRT(("min %s]\n", (queue->wallt_min != UNSPECIFIED) ?
	    schd_sec2val(queue->wallt_min) : "???"));

    if (queue->empty_by)
	/* ctime(2) returns a '\n'-terminated string, so no additional '\n' */
	DBPRT(("  Queue will empty by: %s", ctime(&queue->empty_by)));

    if (queue->idle_since)
	/* ctime(2) returns a '\n'-terminated string, so no additional '\n' */
	DBPRT(("  Queue idle since: %s", ctime(&queue->idle_since)));

    if (queue->useracl && (queue->flags & QFLAGS_USER_ACL)) {
	DBPRT(("    User ACL: "));
	columns = 9;		/* Start with 9 columns for 'User ACL: ' */
	for (aclent = queue->useracl; aclent != NULL; aclent = aclent->next) {
	    columns += strlen(aclent->user) + 1;
	    if (columns >= 72) {
		DBPRT(("\n    "));
		columns = 0;
	    }
	    DBPRT(("%s%s", 
		((columns == 0) || (aclent == queue->useracl)) ? "" : "/", 
		aclent->user));
	}
	DBPRT(("\n"));
    }

    if (dumpjobs && queue->jobs) {
	DBPRT(("  Jobs: "));

	columns = 5;		/* Start with 5 columns for 'Jobs: ' */

	for (job = queue->jobs; job != NULL; job = job->next) {

	    /* Just the job numbers -- but be sure to put the '.' back! */
	    if ((ptr = strchr(job->jobid, '.')) != NULL)
		*ptr = '\0';
	    
	    columns += strlen(job->jobid) + 3; /* 3 == job->state + '/' + ' ' */

	    if (columns >= 72) {
		DBPRT(("\n   "));
		columns = 0;
	    }

	    DBPRT((" %s/", job->jobid));
	    DBPRT(("%c",
		(job->flags & JFLAGS_PRIORITY) ? '!' :
		(job->flags & JFLAGS_WAITING) ? 'W' :
		job->state));
		
	    if (ptr != NULL)
		*ptr = '.';
	}
	DBPRT(("\n"));
    }
}
#endif /* DEBUG */

static int 
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
    longest	= NULL;

    /*
     * Keep track of some statistics about what jobs have been found in the
     * list.  These aren't really used (yet), but are easy to compute.
     */
    running	= 0;
    queued	= 0;
    held	= 0;
    other	= 0;

    /* 
     * The number of jobs that have been moved from the global list to the
     * per-queue list.
     */
    moved	= 0;

    /*
     * Find the last element of the list of jobs on this queue.  This is
     * probably unnecessary (since this should always be called with 
     * 'queue->jobs' pointing to NULL.  Still, it doesn't hurt to try.
     */
    qtail = NULL;
    if (queue->jobs) {
	for (qtail = queue->jobs; qtail->next != NULL; qtail = qtail->next)
	    /* Do nothing - just walk to next-to-last element of list */
	    ;
    }

    prev = NULL;
    for (job = *joblist_ptr; job != NULL; job = next) {
	next = job->next;

	if (strcmp(job->qname, queue->qname)) {
	    /* check if this is a challenge or background job that needs
	     * to be merged with the submit list of jobs 
 	     */
	    if (!strcmp(queue->qname,
 		 schd_SubmitQueue->queue->qname)) {  /* the submit list? */
		if ((strcmp(job->qname, schd_ChallengeQueue)) &&
		    (strcmp(job->qname, schd_SpecialQueue)) &&
		    (strcmp(job->qname, schd_BackgroundQueue))) {

	    		/* Wrong queue -- ignore this job. */
	    		prev = job;
	    		continue;
		}
	    }
	}

	/*
	 * This job belongs to this queue.  Remove it from the job list and
	 * place it at the tail of the queue's job list.  This is somewhat
	 * complicated since we have to remove it from the joblist first.
	 * If there is no "previous" job element, then the current job is
	 * the head of the list.
	 */
	if (job == *joblist_ptr) {

	    /* 
	     * This is the head of the list -- just point the list head to
	     * the job's next pointer and now the job is "orphaned".
	     */


	    *joblist_ptr = next;
	    prev = *joblist_ptr;

	} else {

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

	if (job->flags & JFLAGS_QNAME_LOCAL) {
	    /*
	     * The job has some memory that was allocated when it was created,
	     * that is used to store the name of the queue on which it resides.
	     * Since we know exactly what queue it lives on (the one pointed to
	     * by 'queue', to be exact), we can free the storage and point the 
	     * 'job->qname' at 'queue->qname'.  Also store a reference to the
	     * owner queue in the job itself.
	     */
	    free (job->qname);
	    job->qname = queue->qname;
	    job->queue = queue;

	    /* 
	     * Turn off the flag -- job->qname is now a reference to a queue.
	     */
	    job->flags &= ~JFLAGS_QNAME_LOCAL;
	}

	/* Now, count the job and increment the correct statistic. */
	moved ++;

	switch (job->state) {
	    case 'R':	
		running ++;	

		/*
		 * Adjust the time of any job that has run over its expected
		 * time to the JOB_OVERTIME_SLOP.
		 */
		if (job->time_left < 0) {
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

/*
 * PBS has been known to provide some bizarre information about the state
 * of the queue (i.e. no jobs are running in it, but some of its resources
 * are consumed).  Perform some consistency checks on the queue information
 * before agreeing that it is correct.  Generate a laundry list of everything
 * that appears wrong with it.
 */
static int
queue_sanity(Queue *queue)
{
    char   *id = "queue_sanity";
    Job    *job;
    int     queued, running;
    int     is_sane;

    is_sane = 1;	/* Assume the queue is sane for now. */

    /*
     * Count running and queued jobs and make sure the numbers match up.
     */
    queued = running = 0;

    for (job = queue->jobs; job != NULL; job = job->next) {
	switch (job->state) {
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

/* JJPJ -- this no longer makes much sense since we are adding 
 * 	   now moving challenge and background jobs into the queue
    if (queue->running != running) {
	sprintf(log_buffer, 
	    "Queue '%s' expected %d running jobs, but found %d",
	    queue->qname, queue->running, running);
	log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));
	is_sane = 0;
    }

    if (queue->queued != queued) {
	sprintf(log_buffer, 
	    "Queue '%s' expected %d queued jobs, but found %d",
	    queue->qname, queue->queued, queued);
	log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));
	is_sane = 0;
    }
 *
 */

    if (queue->running && (queue->empty_by < schd_TimeNow)) {
	sprintf(log_buffer, 
	    "Queue '%s' was expected to be empty %d seconds ago",
	    queue->qname, (schd_TimeNow - queue->empty_by));
	log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));
	is_sane = 0;
    }

    return (is_sane);
}
