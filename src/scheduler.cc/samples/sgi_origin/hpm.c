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

/*
 * HPM stands for Harware Performance Monitor.  This is an attribute that
 * allows users to take advantage of such software as "perfex".  SGI 
 * Origin2000's only allow one global counter at a time, so when the system 
 * is using it users are unable to and vice versa.
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pbs_ifl.h"
#include "log.h"

#include "toolkit.h"
#include "gblxvars.h"

#include "hpm.h"
#include "rm.h"

static int setup_hpm (Resources *rsrcs, int mode);
static int revoke_unused_hpm (Resources *rsrcs);

/*
 * Count up the number of jobs, if any, in the list that are running with 
 * HPM support.
 */
int
schd_hpm_job_count(Job *joblist)
{
    Job    *job;
    int     num_hpms = 0;

    for (job = joblist; job != NULL; job = job->next)
	if ((job->state == 'R') && (job->flags & JFLAGS_NEEDS_HPM))
	    num_hpms ++;

    return (num_hpms);
}

/* Interface to the resource monitor hpm_ctl command. */
int 
schd_hpm_ctl(Resources *rsrcs, int mode, char *reason)
{
    char    *host;

    /* 
     * Attempt to set up the hpm counters on the remote host.  If it 
     * succeeds, return a success code.  Otherwise, if there is a reason
     * buffer, sprintf a message into that buffer, and return a fail code.
     */
    if (setup_hpm(rsrcs, mode) == 0)
	return (0);

    if (reason) {
	/* Get the (maybe shorter) name for this host. */
	host = schd_shorthost(rsrcs->exechost);
	if (host == NULL)
	    host = rsrcs->exechost;

	(void)sprintf(reason, "Cannot set state of HPM counters on %s", host);

	free(host);
    }

    return (1);
}

/*
 * Walk the lists of execution queues, looking for queues that live on
 * hosts with the HPM counters in user mode, but no jobs using them.
 */
int 
schd_revoke_hpm(void)
{
    QueueList *qptr;
    int    errors = 0;

    for (qptr = schd_BatchQueues; qptr != NULL; qptr = qptr->next)
	if (qptr->queue->rsrcs)
	    if (revoke_unused_hpm(qptr->queue->rsrcs))
		++ errors;

    for (qptr = schd_DedQueues; qptr != NULL; qptr = qptr->next)
	if (qptr->queue->rsrcs)
	    if (revoke_unused_hpm(qptr->queue->rsrcs))
		++ errors;

    for (qptr = schd_ExternQueues; qptr != NULL; qptr = qptr->next)
	if (qptr->queue->rsrcs)
	    if (revoke_unused_hpm(qptr->queue->rsrcs))
		++ errors;

    return (errors);
}

/*
 * Attempt to set the state of the hpm counters on the host associated
 * with the given Resources.  Mode must be one of HPM_SETUP_USERMODE or
 * HPM_SETUP_GLOBALMODE.  Return 0 on success, non-zero otherwise.
 */
static int
setup_hpm(Resources *rsrcs, int mode)
{
    char   *id = "setup_hpm";
    char   *response, *value;
    char    hpm_ctl[64];
    int     rm;

    switch (mode) {
    case HPM_SETUP_USERMODE:

	/* Sanity check - is the host already in the requested mode? */
	if (rsrcs->flags & RSRCS_FLAGS_HPM_USER) {
	    DBPRT(("%s: hpm user mode requested for %s, but already set!\n",
		id, rsrcs->exechost));
	    return (0);
	}

	(void)sprintf(hpm_ctl, HPM_CTL_FORMAT_STR, HPM_CTL_USERMODE_STR);
	break;

    case HPM_SETUP_GLOBALMODE:

	/* Sanity check - is the host already in the requested mode? */
	if (!(rsrcs->flags & RSRCS_FLAGS_HPM_USER)) {
	    DBPRT(("%s: hpm global mode requested for %s, but already set!\n",
		id, rsrcs->exechost));
	    return (0);
	}

	(void)sprintf(hpm_ctl, HPM_CTL_FORMAT_STR, HPM_CTL_GLOBALMODE_STR);
	break;

    case HPM_SETUP_REVOKE:
	/* Sanity check - is the host already in the requested mode? */
	if (!(rsrcs->flags & RSRCS_FLAGS_HPM_USER)) {
	    DBPRT(("%s: hpm revocation requested for %s, but already global!\n",
		id, rsrcs->exechost));
	    return (0);
	}

	(void)sprintf(hpm_ctl, HPM_CTL_FORMAT_STR, HPM_CTL_REVOKE_STR);
	break;

    default:
	DBPRT(("%s: Bogus mode %d - bailing.\n", id, mode));
	return (1);
    }

    DBPRT(("%s: '%s' @ %s\n", id, hpm_ctl, rsrcs->exechost));

    if ((rm = openrm(rsrcs->exechost, 0)) < 0) {
	(void)sprintf(log_buffer,
	    "Unable to contact resmom@%s (%d)", rsrcs->exechost, pbs_errno);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	return (1);
    }

    /* Ask the resource monitor on the remote host to set the mode for us. */
    response = NULL;
    if (addreq(rm, hpm_ctl) == 0)
	response = getreq(rm);

    closerm(rm);

    if (response == NULL) {
	(void)sprintf(log_buffer, "bad return from getreq(%s) @%s, %d",
	    hpm_ctl, rsrcs->exechost, pbs_errno);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));
	return (1);
    }

    /* 
     * If a full response was received, move forward to the first character
     * of the value (following the '=' in the attribute-value pair).
     */
    if (value = strchr(response, '='))
	response = ++value;

    /* 
     * If the hpm_ctl request succeeded, log the fact, and set the flag in 
     * the resources for this host to indicate that it is now in the other
     * state.
     */
    if (strcmp(response, HPM_CTL_OKAY_STR) == 0) {
	if (mode == HPM_SETUP_USERMODE)
	    rsrcs->flags |= RSRCS_FLAGS_HPM_USER;
	else
	    rsrcs->flags &= ~RSRCS_FLAGS_HPM_USER;

	(void)sprintf(log_buffer, "%s on %s succeeded", hpm_ctl, 
	    rsrcs->exechost);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));

	return (0);
    }

    /* If it's an error string, just report the error message returned. */
    if (strncmp(response, HPM_CTL_ERROR_STR, strlen(HPM_CTL_ERROR_STR)) == 0) {

	response += strlen(HPM_CTL_ERROR_STR);	/* Skip the error string. */
	while (*response == ' ')		/* Skip leading whitespace. */
	    ++ response;

	/* And generate the log message from the request and the response. */
	(void)sprintf(log_buffer, "%s: %s (%s)", hpm_ctl, response,
	    rsrcs->exechost);
    } else {
	(void)sprintf(log_buffer, "cannot parse response %s to request %s@%s", 
	    response, hpm_ctl, rsrcs->exechost);
    }
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    return (1);
}

static int
revoke_unused_hpm(Resources *rsrcs)
{
    char    *id = "revoke_unused_hpm";
    int      rc;

    /* 
     * If the HPM counters on the host associated with this Resources
     * struct are user mode, and the counters are not being used by any 
     * jobs on the host, then attempt to return them to the global 
     * system mode.
     */

    if ((rsrcs->flags & RSRCS_FLAGS_HPM_USER) &&
	!(rsrcs->flags & RSRCS_FLAGS_HPM_IN_USE)) 
    {
	DBPRT(("revoke_unused_hpm: host %s HPM in user mode, but nobody "
	    "using it.\n", rsrcs->exechost));
	rc = schd_hpm_ctl(rsrcs, HPM_SETUP_GLOBALMODE, NULL);
	if (!schd_REVOKE_HPM || !rc)
	    return rc;

	/* 
	 * Somebody is using the hpm counters, but nobody has asked for them. 
	 * Attempt a forcible revocation.  This will kill processes on the
	 * remote host to free the counters.
	 */
	
	(void)sprintf(log_buffer, 
	    "Attempting forcible revocation of HPM counters for host %s",
	    rsrcs->exechost);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	return schd_hpm_ctl(rsrcs, HPM_SETUP_REVOKE, NULL);
    }

    return (0);		/* Nothing to do. */
}
