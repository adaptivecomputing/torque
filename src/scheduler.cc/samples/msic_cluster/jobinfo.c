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

#include "toolkit.h"
#include "gblxvars.h"

#include "log.h"
#include "net_connect.h"

extern int connector;

/*
 * This function takes a pointer to a struct batch_status for a job, and
 * fills in the appropriate fields of the supplied job struct.  It returns
 * the number of items that were found.
 */
int
schd_get_jobinfo(Batch_Status *bs, Job *job)
{
    int       changed = 0;
    int       istrue;
    char      tmp_str[120];
    char     *id = "schd_get_jobinfo";
    char     *host;
    char     *p, *tmp_p, *var_p;
    AttrList *attr;

    memset((void *)job, 0, sizeof(Job));

    job->jobid = schd_strdup(bs->name);	

    if (job->jobid == NULL) {
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
	    "schd_strdup(bs->name)");
	return (-1);
    }
    changed ++;

    for (attr = bs->attribs; attr != NULL; attr = attr->next) {

	/* 
	 * If this is the 'owner' field, chop it into 'owner' and 'host'
	 * fields, and copy them into the Job struct.
	 */
	if (!strcmp(attr->name, ATTR_owner)) {

	    /* Look for the '@' that separates user and hostname. */
	    strcpy(tmp_str, attr->value);
	    host = strchr(tmp_str, '@');

	    if (host) {
		*host = '\0';	/* Replace '@' with NULL (ends username). */
		host ++;	/* Move to first character of hostname. */
	    }

	    job->owner = schd_strdup(tmp_str);
	    if (job->owner == NULL) {
		log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		    "schd_strdup(job->owner)");
		return (-1);
	    }
	    changed ++;
	    continue;
	}

	/* The group to which to charge the resources for this job. */
	if (!strcmp(attr->name, ATTR_egroup)) {
	    job->group = schd_strdup(attr->value); 
	    if (job->group == NULL) {
		log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		    "schd_strdup(job->group)");
		return (-1);
	    }
	    changed ++;
	    continue;
	}

	/* The comment currently assigned to this job. */
	if (!strcmp(attr->name, ATTR_comment)) {
	    job->comment = schd_strdup(attr->value); 
	    if (job->comment == NULL) {
		log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		    "schd_strdup(job->comment)");
		return (-1);
	    }
	    changed ++;
	    continue;
	}

	/* The host on which this job is running (or was running for
	 * suspended or checkpointed jobs. */

	if (!strcmp(attr->name, ATTR_exechost)) {
	    job->exechost = schd_strdup(attr->value); 
	    if (job->exechost == NULL) {
		log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		    "schd_strdup(job->exechost)");
	        return (-1);
	    }
	    changed ++;
	    continue;
	}

	if (!strcmp(attr->name, ATTR_state)) {
	    /* State is one of 'R', 'Q', 'E', etc. */
	    job->state = attr->value[0];
	    changed ++;
	    continue;
	}

	if (!strcmp(attr->name, ATTR_queue)) {
	    job->qname = schd_strdup(attr->value);
	    if (job->qname == NULL) {
		log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		    "schd_strdup(job->qname)");
		return (-1);
	    }
	    job->flags |= JFLAGS_QNAME_LOCAL;
	    changed ++;
	    continue;
	}

	if (!strcmp(attr->name, ATTR_v)) {
	    var_p = schd_strdup(attr->value);
	    if (var_p == NULL) {
		log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		    "schd_strdup(Variable_List)");
		return (-1);
	    }

	    p = NULL;
	    tmp_p = strstr(var_p, "PBS_O_QUEUE");

	    if (tmp_p) {
		p = strtok(tmp_p, "=");
		p = strtok(NULL,  ", ");
	    }

	    if (p != NULL) {
		job->oqueue = schd_strdup(p);
	    } else {
		/* if the originating queue is unknown, default
		 * to the locally defined "submit" queue.
		 */
		job->oqueue = schd_strdup(schd_SubmitQueue->queue->qname);
	    }
	    free(var_p);
	    changed ++;
	    continue;
	}

	if (!strcmp(attr->name, ATTR_l)) {
	    if (!strcmp(attr->resource,"arch")) {
		job->arch = schd_strdup(attr->value);
		changed ++;

	    } else if (!strcmp(attr->resource,"mem")) {
		job->memory = schd_val2byte(attr->value);
		changed ++;

	    } else if (!strcmp(attr->resource,"ncpus")) {
		job->ncpus = atoi(attr->value);
		changed ++;

	    } else if (!strcmp(attr->resource, "walltime")) {
		job->walltime = schd_val2sec(attr->value);
		changed ++;

	    } 

	    /* That's all for requested resources. */
	    continue;
	}

	if (!strcmp(attr->name, ATTR_used)) {   
	    if (!strcmp(attr->resource, "walltime")) {
		job->walltime_used = schd_val2sec(attr->value);
		changed ++;
	    }

	    /* No other interesting cases. */
	    continue;
	}

	/* Creation time attribute. */
	if (!strcmp(attr->name, ATTR_ctime)) {
	    /* How long ago was it put in the queue ? */
	    job->time_queued = schd_TimeNow - atoi(attr->value);
	    continue;
	}

	/* Modified time attribute. */
	if (!strcmp(attr->name, ATTR_mtime)) {
	    /* When was the job last modified? */
	    job->mtime = atoi(attr->value);
	    continue;
	}

	/* Job Substate attribute. */
	if (!strcmp(attr->name, ATTR_substate)) {
            if (atoi(attr->value) == 43 /* JOB_SUBSTATE_SUSPEND */)
        	job->flags |= JFLAGS_SUSPENDED;
	    continue;
	}

	/* 
	 * When was the job last eligible to run?  When a user-hold is
	 * released, this value is updated to the current time.  This
	 * prevents users from gaining higher priority from holding their
	 * jobs.
	 */
	if (!strcmp(attr->name, ATTR_etime)) {
	    job->eligible = schd_TimeNow - atoi(attr->value);

	    continue;
	}
    }

    if (job->memory < 1) {
	job->memory = get_default_mem(job->oqueue);
	schd_alterjob(connector,job,ATTR_l,schd_byte2val(job->memory),"mem");
	changed++;
    }
    /* 
     * If this job is in the "Running" or "Suspended" state, compute how
     * many seconds remain until it is completed.
     */
    if (job->state == 'R' || job->state == 'S') {
	job->time_left = job->walltime - job->walltime_used;
    }

    /*
     * If this job was enqueued since the last time we ran, set the job
     * flag to indicate that we have not yet seen this job.  This makes it
     * a candidate for additional processing.  There may be some inaccuracy,
     * since the time_t has resolution of 1 second.  Attempt to err on the
     * side of caution.
     */
    if ((job->state == 'Q') && (job->time_queued != UNSPECIFIED)) {
        if (job->time_queued <= (schd_TimeNow - schd_TimeLast)) {
	    job->flags |= JFLAGS_FIRST_SEEN;
	}
    }

    /*
     * If this job was previously running and is now queued, then we
     * need to (a) flag it as having been checkpointed, and (b) move
     * it back to the submit queue, if its not already there.
     */
    if (job->exechost && job->state == 'Q') {
	job->flags |= JFLAGS_CHKPTD;
	if (strcmp(job->qname, schd_SubmitQueue->queue->qname)) {
	    sprintf(log_buffer, "moving Q'd job %s back to SUBMIT Q",
		job->jobid);
	    log_record(PBSEVENT_SYSTEM,PBS_EVENTCLASS_SERVER,id,log_buffer);
	    pbs_movejob(connector, job->jobid, schd_SubmitQueue->queue->qname,
		NULL);
	}
    }

    /* 
     * if this job is currently Suspended (a substate of 'R'unning), then
     * pretend its queued, so that the scheduling logic will work.
     */
    if (job->state == 'S') {
	job->state = 'Q';
	job->flags |= JFLAGS_SUSPENDED;
    }

    /* if this job is suspended, checkpointed, or otherwise "queued"
     * on an exection queue, update the internal representation of 
     * to pretend it is really on the submit queue.
     */

    if ((job->flags & JFLAGS_SUSPENDED) || (job->flags & JFLAGS_CHKPTD)) {
	free(job->qname);
	job->qname = schd_strdup(schd_SubmitQueue->queue->qname);
    }

    /*
     * If this job came from the EXPRESS queue, set the flag so that it
     * will be treated with the highest of priority.
     */
    if (!strcmp(job->oqueue, schd_EXPRESS_Q_NAME))
	job->flags |= JFLAGS_PRIORITY;

    /*
     * If the 'etime' attribute wasn't found, set it to the time the job has
     * been queued.  Most jobs will be eligible to run their entire lifetime.
     * The exception is a job that has been held - if it was a user hold,
     * the release will reset the etime to the latest value.
     * If not eligible time was given, use the job's creation time.
     */
    if (!job->eligible)
	job->eligible = job->time_queued;

    /* get the percentage of shares used */
    job->sort_order = get_share_rank(job);

    /* if this job has waited too long, and its queue is NOT over its
     * shares, then bump it up in priority.
     */
    if (job->eligible > schd_MAX_WAIT_TIME && job->sort_order <= 100)
        job->flags |= JFLAGS_WAITING;

    return (changed);
}

/*
 * Walk a list of jobs, freeing any information on them and then freeing
 * each element.  Returns number of elements freed.
 */
int
schd_free_jobs(Job *list)
{
    Job    *job, *next;
    int     numjobs = 0;

    for (job = list; job != NULL; job = next) {
        numjobs ++;
	next = job->next;

	if (job->jobid)		free(job->jobid);
	if (job->owner)		free(job->owner);
	if (job->group)		free(job->group);
	if (job->comment)	free(job->comment);
	if (job->exechost)	free(job->exechost);
	if (job->oqueue)	free(job->oqueue);
	if (job->arch)		free(job->arch);

	/*
	 * Do *not* free the Job's backpointer to the queue's name if it
	 * is a reference to the Queue's qname.  If it is local storage
	 * created by schd_strdup(), it should be freed to prevent a 
	 * memory leak.
	 */
	if (job->flags & JFLAGS_QNAME_LOCAL)
	    free(job->qname);

	free(job);
    }

    return (numjobs);
}

/* return the percentage of shares usage by the job owner's queue/share
 */
int
get_share_rank(Job *job)
{
    FairAccessList *FALptr;
    AccessEntry    *AE;
    int ret = 100;

    for (FALptr = schd_FairACL; FALptr != NULL; FALptr = FALptr->next) {
        for (AE = FALptr->entry; AE != NULL; AE = AE->next) {
            if (AE->name) {
	        if (!strcmp(AE->name, job->oqueue)) {
		    ret = (int) percent_shares_past(AE);
		    return (ret);
		}
            }
        }
    }
    return (ret);
}

size_t get_default_mem(char *qName)
{
    FairAccessList *FALptr;
    AccessEntry    *AE;
    size_t val, ret = 100*1024*1024;

    for (FALptr = schd_FairACL; FALptr != NULL; FALptr = FALptr->next) {
        for (AE = FALptr->entry; AE != NULL; AE = AE->next) {
            if (AE->name) {
	        if (!strcmp(AE->name, qName)) {
		    if (AE->default_mem > 1) {
		        ret = AE->default_mem;
		        return (ret);
		    } else {
			val = schd_get_queue_memory(qName);
			if (val != UNSPECIFIED) {
			    AE->default_mem = val;
			    return(val);
			}
			break;
		    }
		}
            }
        }
    }
    return (ret);
}
