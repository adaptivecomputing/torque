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
 * Perform some basic analysis of system resources to determine if 
 * jobs should be run.
 */

#include <stdio.h>
#include <stdlib.h>

/* PBS header files */
#include "pbs_error.h"
#include "pbs_ifl.h"
#include "log.h"

/*  Scheduler head files */
#include "toolkit.h"
#include "gblxvars.h"

#define DONT_START_JOB 0
#define	START_JOB 1

int
schd_evaluate_system(Resources *rsrcs, char *reason)
{
    double loadchk;

    if (rsrcs->ncpus_alloc == rsrcs->ncpus_total) {
	if (reason)
	    (void)sprintf(reason, "No ncpus available to schedule");
	return (DONT_START_JOB);
    }

    /* Does the system have a sufficient load?  If not, start a job. */
    loadchk = (double)((schd_TARGET_LOAD_PCT - schd_TARGET_LOAD_MINUS) * 
	rsrcs->ncpus_total);
    if (rsrcs->loadave < loadchk) {
	if (reason)
	    (void)sprintf(log_buffer, "Load Average too low (%.02f/%.02f min)",
		rsrcs->loadave, loadchk);
	return (START_JOB);
    }

    /* Does the system have too much work to do?  Don't start a new job. */
    loadchk = (double)((schd_TARGET_LOAD_PCT + schd_TARGET_LOAD_PLUS) * 
	rsrcs->ncpus_total);
    if (rsrcs->loadave > loadchk) {
	if (reason)
	    (void)sprintf(reason, "Load Average too high (%.02f/%.02f max)",
		rsrcs->loadave, loadchk);
	return (DONT_START_JOB);
    }

    /*
     *  If we are not running the nominal number of jobs or if
     *  we are taking idle, then it's okay to start jobs.
     */
    loadchk = (double)(schd_TARGET_LOAD_PCT * rsrcs->ncpus_total);
    if ((rsrcs->loadave < loadchk) || rsrcs->idltime) {
	if (reason)
	    (void)sprintf(log_buffer, 
		"Load average below nominal (%.02f/%.02f nom).  Start jobs.",
		rsrcs->loadave, loadchk);
	return (START_JOB);
    }

    if (reason)
	(void)sprintf(reason, "Done.  Don't need to run any jobs.");

    /*
     *  If we reached here, then we really don't need to start 
     *  another job.  If we do, then the system could become over
     *  burdened with work.
     */
    return (DONT_START_JOB);
}
