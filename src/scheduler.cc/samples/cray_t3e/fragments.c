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

int
schd_fragment_okay(Job *job, Queue *queue, char *reason)
{
    /* char   *id = "fragment_okay"; */
    int     nodes_avail, frag_size, avg_nodes;

    /*
     * If this job has been waiting for a long time, don't allow anything to
     * interfere with it's execution.
     */
    if (job->flags & JFLAGS_WAITING)
	return (1);

    /* 
     * "Fragments" only make sense if there is a node limit and a maximum
     * number of running jobs.  If not, fragmentation is not really an issue.
     */
    if ((queue->nodes_max == UNSPECIFIED) ||
	(queue->maxrun == UNSPECIFIED) ||
	(queue->maxrun < 2))
    {
	return (1);
    }

    frag_size = (queue->nodes_max / queue->maxrun);
    nodes_avail = (queue->nodes_max - queue->nodes_assn);

    /*
     * See if the queue has fragments of less than 2 nodes, and return if
     * so -- fragmentation is nonsensical for <2 nodes/frag.
     */
    if (frag_size < 2)
	return (1);

    /*
     * If there is less than a fragment left, do not run a job that would
     * cause this queue to be fragmented when the other jobs exit.
     */
    if (nodes_avail < frag_size) {
	if ((job->walltime + schd_TimeNow) > queue->empty_by) {
	    if (reason)
		sprintf(reason, 
		    "Would leave fragments in %s (must be <= %s walltime)", 
		    queue->qname, schd_sec2val(queue->empty_by - schd_TimeNow));
	    return (0);
	}

	/*
	 * This job will end before the queue is drained, so go ahead and
	 * run it.  It may temporarily increase fragmentation, but it will
	 * not be a problem for long.
	 */
	return (1);
    }

    /*
     * Find the average number of nodes consumed by jobs in this queue.  This
     * number is rounded to the nearest fragment size.  If there are no 
     * running jobs, allow anything to run and deal with the consequences
     * later.  This should help prevent starvation of very small jobs.
     */
    if (queue->running)
	avg_nodes = ((queue->nodes_assn + (frag_size / 2)) / queue->running);
    else
	return (1);

    /*
     * If the average number of nodes in use by the processes running in the
     * queues is less than the fragment size, then run a job that is smaller
     * than the fragment size, as long as it won't go beyond the time limit
     * of the longest running job.  If there are no running jobs, than let 
     * the fragment run.
     */
    if (job->nodes < frag_size) {

	/* This job is a fragment, and the average job in the queue is not. */

	if (avg_nodes >= frag_size) {
	    if (reason) {
		sprintf(reason, 
		    "Would fragment queue %s (must be >= %d nodes)",
		    queue->qname, frag_size);
	    }
	    return (0);
	}

	if (queue->running && 
	    ((job->walltime + schd_TimeNow) > queue->empty_by))
	{
	    if (reason) {
		sprintf(reason, 
		    "Would perpetuate fragmentation (must be < %d sec)",
		    (queue->empty_by - schd_TimeNow));
	    }
	    return (0);
	}
    }

    return (1);
}
