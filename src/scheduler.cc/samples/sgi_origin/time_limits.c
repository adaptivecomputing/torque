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
#include <string.h>
#include <time.h>

/* PBS header files */

#include "pbs_ifl.h"
#include "log.h"

/* Scheduler header files */

#include "toolkit.h"
#include "gblxvars.h"

/*
 * Determine whether a job will fit the primetime walltime limits.  This
 * means that the sum of time the job would run while it is primetime must
 * be no more than the appropriate prime-time walltime limit.
 */
/* ARGSUSED */
int 
schd_primetime_limits(Job *job, Queue *queue, time_t when, char *reason)
{
#ifdef DEBUGLOTS
    char   *id = "schd_primetime_limits";
#endif /* DEBUGLOTS */
    int     is_pt;
    int     secs_in_pt, secs_left;
    int     length;
#ifdef DEBUGLOTS
    char    buff[64];
#endif /* DEBUGLOTS */

    /* If not enforcing primetime, than no limits apply. */
    if (!(schd_ENFORCE_PRIME_TIME && schd_TimeNow >= schd_ENFORCE_PRIME_TIME))
	return (0);

    /* If this particular queue is ignoring primetime, no limits apply. */
    if (!queue->observe_pt)
	return (0);

    /* High-priority jobs do not have to meet primetime limits. */
    if (job->flags & JFLAGS_PRIORITY)
	return (0);

    /* If 'when' is 0, initialize it to the "current time". */
    if (when == 0)
	when = schd_TimeNow;

#ifdef DEBUGLOTS
    DBPRT(("%s: check primetime limits for job %s (%s)\n", id, job->jobid,
	schd_sec2val(job->walltime)));
#endif /* DEBUGLOTS */

    secs_in_pt = 0;
    secs_left  = job->walltime;

    while (secs_left > 0) {
	/* Is it prime-time at the time 'when' indicates? */
	is_pt = schd_prime_time(when);

	/* Find out how long the current period will last. */
	if (is_pt)
	    length = schd_secs_til_nonprime(when);
	else
	    length = schd_secs_til_prime(when);

	/* 
	 * If this is a prime-time period, account for the time the job
	 * will spend within it.  This is either the length of the period
	 * itself, or the time remaining (if it doesn't completely span 
	 * the prime-time period).
	 */
	if (is_pt)
	    secs_in_pt += (secs_left < length) ? secs_left : length;

	/* Account for the time spent in this period. */
	secs_left -= (secs_left < length) ? secs_left : length;

#ifdef DEBUGLOTS
	strcpy(buff, ctime(&when));
	buff[19] = '\0';
	DBPRT(("%s:    %sprime period at %s (for %s), %d in_pt %d remain\n", id,
	    is_pt ? "" : "non-", buff, schd_sec2val(length), secs_in_pt, 
	    secs_left));
#endif /* DEBUGLOTS */

	/* Push "time" forward to the beginning next period. */
	when += length;
    }

#ifdef DEBUGLOTS
    DBPRT(("%s: Job requests %d secs, %d of which are in prime time.\n",
	id, job->walltime, secs_in_pt));
#endif /* DEBUGLOTS */

    /* If there is no overlap into primetime, then the job is okay. */
    if (secs_in_pt == 0)
	return (0);

    /*
     * Check the appropriate time limit for this job (based on cpu count). 
     * Note that, if PT_SMALL_NODE_LIMIT is not set, the reason is different
     * from the small/large case.
     */
    if (schd_PT_SMALL_NODE_LIMIT > 0) {
	if (job->nodes <= schd_PT_SMALL_NODE_LIMIT) {
	    if (secs_in_pt > schd_PT_SMALL_WALLT_LIMIT) {
		if (reason)
		    (void)sprintf(reason, 
			"Exceeds primetime walltime limit (%s)",
			schd_sec2val(schd_PT_SMALL_WALLT_LIMIT));

		return (1);
	    }
	} else {
	    if (secs_in_pt > schd_PT_WALLT_LIMIT) {
		if (reason)
		    (void)sprintf(reason, 
			"Exceeds primetime walltime limit (%s)",
			schd_sec2val(schd_PT_WALLT_LIMIT));
		return (1);
	    }
	}
    } else {
	if (secs_in_pt > schd_PT_WALLT_LIMIT) {
	    if (reason)
		(void)sprintf(reason, 
		    "Exceeds primetime walltime limit (%s)",
		    schd_sec2val(schd_PT_WALLT_LIMIT));

	    return (1);
	}
    }

    /* Job is okay. */
    return (0);
}

/*
 * Determine whether or not this job will be able to complete
 * before the next non-primetime, if started "when" (usually
 * when==now). Returns 1: yes, job can finish before nonprimetime
 *                 or  0: no, could not complete.
 */

int schd_finish_before_np(Job *job, Queue *queue, time_t when, char *reason)
{
    int nextnpt = 0;

    /* If not primetime, than no limit. */
    if (!(schd_ENFORCE_PRIME_TIME && schd_TimeNow >= schd_ENFORCE_PRIME_TIME))
	return (1);

    /* If the job is a special-priority job, this limit does not apply. */
    if (job->flags & JFLAGS_PRIORITY)
	return (1);

    /* If this particular queue is ignoring primetime, no limits apply. */
    if (!queue->observe_pt)
	return (1);

    if (schd_prime_time(when)) {
        nextnpt = schd_secs_til_nonprime(when);

	/* 
	 * If the non-primetime drain has been violated (for a special job,
	 * most likely), don't bother trying to enforce it.
	 */
	if (queue->empty_by && ((queue->empty_by - schd_TimeNow) >= nextnpt))
	    return(1);

        if (job->walltime >= nextnpt) {
	    if (reason)
		(void)sprintf(reason, "System draining for non-primetime at %s",
		    schd_sec2val(schd_PRIME_TIME_END));
	    return(0);
	}
    }
    return(1);
}
