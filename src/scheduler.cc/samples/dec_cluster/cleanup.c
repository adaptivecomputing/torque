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
#if defined(sgi)
#include <sys/sysmacros.h>
#endif
#include <string.h>

/* PBS header files */

#include "pbs_error.h"
#include "pbs_ifl.h"
#include "log.h"

/* Scheduler header files */

#include "toolkit.h"
#include "gblxvars.h"

static int cleanup_queue(Queue *queue);
int cleanup_rsrcs(Resources *rlist);

/*
**	Remove information from the queues in schd_SubmitQueue and
**	schd_BatchQueues.  Leave the lists in place.
**
**	Completely remove schd_AllJobs and schd_RsrcsList.
*/
int 
schd_cleanup(void)
{
    QueueList *qptr;

    for (qptr = schd_SubmitQueue; qptr != NULL; qptr = qptr->next)
	cleanup_queue(qptr->queue);

    for (qptr = schd_BatchQueues; qptr != NULL; qptr = qptr->next)
	cleanup_queue(qptr->queue);

    if (schd_AllJobs) {
	schd_free_jobs(schd_AllJobs);
	schd_AllJobs = NULL;
    }

    if (schd_RsrcsList) {
	cleanup_rsrcs(schd_RsrcsList);
	schd_RsrcsList = NULL;
    }

    return (0);
}

/*
**	Remove information from a Queue.  It will still exist, just
**	in a "clean" state.
*/
static int
cleanup_queue(Queue *queue)
{
    char *qname, *exechost;
    time_t idle_since;
    int   pt_obsvd;

    if (queue->jobs)
	schd_free_jobs(queue->jobs);

    if (queue->useracl)
	schd_free_useracl(queue->useracl);

    if (queue->featureA)
	free(queue->featureA);
    if (queue->featureB)
	free(queue->featureB);
    if (queue->featureC)
	free(queue->featureC);

    /* 
     * Rather than explicitly zeroing out all the fields in the queue, save
     * a reference to the queue name and host, which is all that is needed 
     * to identify the queue next time it is used.  Note that this sets the
     * jobs list to NULL, so it will not be accidentally traversed.
     *
     * Be sure to save and restore any other necessary information here!
     */

    qname      = queue->qname;
    exechost   = queue->exechost;
    idle_since = queue->idle_since;
    pt_obsvd   = queue->observe_pt;

    memset (queue, 0, sizeof (Queue));

    queue->qname      = qname;
    queue->exechost   = exechost;
    queue->idle_since = idle_since;
    queue->observe_pt = pt_obsvd;

    return (0);
}

/*
**	Remove a Resources list.
**	After doing this, rlist will point to garbage.
*/
int 
cleanup_rsrcs(Resources *rlist)
{
    Resources *rptr, *next;
    int    num = 0;

    for (rptr = rlist; rptr != NULL; rptr = next) {
	next = rptr->next;

	if (rptr->exechost)
	    (void)free(rptr->exechost);

	if (rptr->arch)
	    (void)free(rptr->arch);

	(void)memset (rptr, 0, sizeof (Resources));
	(void)free(rptr);

	num ++;
    }

    return (num);
}
