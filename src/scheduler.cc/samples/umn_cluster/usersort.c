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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "toolkit.h"
#include "gblxvars.h"
#include "msgs.h"

#include "pbs_error.h"
#include "log.h"

extern int connector;

/* Antisipated schedule of jobs */
struct Sinfo *JobSchedule = NULL;
int nJobSchedule;

/* Per-class job lists.  Point to a set of jobs in the input list. */
static Job **normalQ     = NULL;
static Job **otherQ      = NULL;
static Job **runningJobs = NULL;
static Job  *make_job_list (void);
static int   nnormalQ, notherQ, nJQs, nJRs;
static void  sort_jobs(void);
static int   split_jobs             (Job *jobs);
static int   compare_fcfs           (const void *e1, const void *e2);
static int   compare_running        (const void *e1, const void *e2);
static int   compare_qtime          (const void *e1, const void *e2);
static void  count_user_jobs        (Job *job, int *run, int *tot, int *max);
static time_t next_shares_update    (void);

/* 
 * Take a list of jobs, break it into sublists, return a pointer to a
 * new list of jobs in the order in which they should be run.
 */
Job *
schd_sort_jobs(Job *jobs)
{
    Job *newjobs = NULL;

    if (split_jobs(jobs))
	return (NULL);

    /* Sort jobs based on local policy requirements */
    sort_jobs();

    /* Create the current 'schedule' of running jobs */
    create_job_schedule();

    /* Convert arrays of jobs into a linked list. */
    newjobs = make_job_list();

    return (newjobs);
}

/*
 * Split the supplied list of Job's into separate arrays based on various
 * characteristics of the jobs (queued vs. running, etc)
 */
static int
split_jobs(Job *jobs)
{
    char   *id = "split_jobs";
    Job    *this;
    int     normalI, runningI, otherI;

    /* Number of elements in the queue arrays. */
    nJRs = nJQs = nnormalQ = notherQ = 0;

    runningJobs = NULL;
    normalQ     = NULL;
    otherQ      = NULL;

    /*
     * Count the number of jobs that belong to each class, and allocate
     * an array of pointers to Job's for each non-empty class.  Note that
     * only jobs that are either 'R'unning or 'Q'ueued are of interest.
     * Place other jobs on "other" list for completeness.
     */
    for (this = jobs; this != NULL; this = this->next) {
	if (this->state == 'R') {
	    nJRs ++;
	    continue;
	}

	if (this->state == 'Q') {
	    nJQs ++;
	    nnormalQ ++;
	    continue;
	}
	
	/* Some other state. Keep track of it so the memory isn't lost */
	notherQ++;
	continue;
    }

    if (!nJRs && !nJQs) {
	DBPRT(("%s: found no jobs in either state 'R' or 'Q'!\n", id));
	return (-1);
    }

    /* 
     * Now allocate arrays of pointers large enough to hold a pointer to
     * each job in the class.  These arrays will be sorted, and the job
     * lists reordered to match the sorted values.
     */
    if (nJRs) {
	if ((runningJobs = (Job **)malloc(nJRs * sizeof (Job *))) == NULL) {
	    DBPRT(("%s: malloc failed for %d Job *'s (%s)\n", id, 
		nJRs, "runningJobs"));
	    goto malloc_failed;
	}
    }

    if (nnormalQ) {
	if ((normalQ = (Job **)malloc(nnormalQ * sizeof (Job *))) == NULL) {
	    DBPRT(("%s: malloc failed for %d Job *'s (%s)\n", id, 
		nnormalQ, "normalQ"));
	    goto malloc_failed;
	}
    }

    if (notherQ) {
	if ((otherQ = (Job **)malloc(notherQ * sizeof (Job *))) == NULL) {
	    DBPRT(("%s: malloc failed for %d Job *'s (%s)\n", id, 
		notherQ, "otherQ"));
	    goto malloc_failed;
	}
    }

    /*
     * Populate the arrays of pointers with 
     */
    normalI = runningI = otherI = 0;

    for (this = jobs; this != NULL; this = this->next) {
	if (this->state != 'R' && this->state != 'Q') {
	    otherQ[otherI++] = this;
	    continue;
	}

	if (this->state == 'R') {
	    runningJobs[runningI++] = this;
	    continue;
	}

	normalQ[normalI++] = this;
    }

    (void)sprintf(log_buffer,
	"running:%d queued:%d other:%d total:%d",
	    runningI,  normalI,  otherI, runningI + normalI + otherI);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));

    return (0);

malloc_failed:

    if (normalQ)      { free(normalQ);		normalQ      = NULL; }
    if (otherQ)       { free(otherQ);		otherQ       = NULL; }
    if (runningJobs)  { free(runningJobs);	runningJobs  = NULL; }
    nJRs = nJQs = 0;

    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, "malloc failed");
    return (-1);
}

void sort_jobs(void)
{
    /*
     * UMN desires jobs to be ordered first come first server (FCFS/FIFO).
     * Therefore we sort the jobs on the eligible field (in case some 
     * sneaky users reorder their jobs in the queue).
     */
    qsort(normalQ, nnormalQ, sizeof *normalQ, compare_fcfs);

    /* 
     * Sort the list of running jobs, in ascending order by the expected
     * time to completion.
     */
    qsort(runningJobs, nJRs, sizeof *runningJobs, compare_running);

    return;
}

/*
 * qsort() function to sort jobs into FCFS / FIFO order.
 *
 * Order queued jobs descending by how long queued
 */
static int
compare_fcfs(const void *e1, const void *e2)
{
    Job    *job1 = *(Job **)e1;
    Job    *job2 = *(Job **)e2;

    if (job1->time_queued > job2->time_queued)
	return -1;

    if (job1->time_queued < job2->time_queued)
	return 1;
	
    return 0;
}

/*
 * qsort() function to order running jobs.
 *
 * Order running jobs by remaining runtime from soonest-ending to latest-
 * ending.  Jobs end up in order of time of completion (first to complete
 * first).
 */
static int
compare_running(const void *e1, const void *e2)
{
    Job    *job1 = *(Job **)e1;
    Job    *job2 = *(Job **)e2;

    if (job1->time_left > job2->time_left)
	return 1;

    if (job1->time_left < job2->time_left)
	return -1;
	
    return 0;
}


/*
 * qsort() function to order Job Schedule
 *
 * Order Job Schedule by date/time stamp of events,
 * descending by number of seconds queued
 */
static int
compare_qtime(const void *e1, const void *e2)
{
    struct Sinfo    *ent1 = (struct Sinfo*)e1;
    struct Sinfo    *ent2 = (struct Sinfo*)e2;

    if (ent1->time > ent2->time)
	return 1;

    if (ent1->time < ent2->time)
	return -1;
	
    return 0;
}


/* 
 * This function creates a new linked list from the Job structs pointed to 
 * by the arrays of Job pointers.  Note that the reassembly is carried out
 * in place - only the links are modified, there is no allocation or freeing
 * other than at the end to free all the sublists.
 *
 * The final list will be reassembled and ordered as: 
 *
 *      running, normal
 */
static Job *
make_job_list(void)
{
    Job    list_seed, *joblist, *jobtail;
    int    i;

    memset(&list_seed, 0, sizeof(list_seed));

    /*
     * "Seed" the linked list by pointing to a bogus initial element.
     * Since the jobtail->next pointer will always be valid (either it
     * hangs off the seed or a real job) this simplifies the following
     * list operations considerably.
     */
    joblist = &list_seed;
    jobtail = &list_seed;
    jobtail->next = NULL;

    /* Walk the running jobs and place them on the list. */
    for (i = 0; i < nJRs; i++)
	jobtail = jobtail->next = runningJobs[i];

    /* Walk the normal jobs and place them on the list. */
    for (i = 0; i < nnormalQ; i++)
	jobtail = jobtail->next = normalQ[i];

    /* Place any remaining jobs on the end of the list. */
    for (i = 0; i < notherQ; i++)
	jobtail = jobtail->next = otherQ[i];

    /* Terminate the last element on the list with a NULL next pointer. */
    jobtail->next = NULL;

    /* Free any storage allocated for the lists. */
    if (runningJobs)
	free(runningJobs); 

    if (normalQ)
	free(normalQ);

    if (otherQ)
	free(otherQ);

    /* And reset all the values. */
    runningJobs = normalQ = otherQ = NULL;
    nJRs = nJQs = nnormalQ = notherQ = 0;

    /*
     * The first element on joblist is the pointer to the list_seed.  It's
     * next pointer points to the head of the real list - return that.
     */
    return (joblist->next);
}

void create_job_schedule()
{
    char   *id = "create_job_schedule";
    Job    *job_ptr;
    int     i;

    /*
     * Destroy any previously created list.
     */
    if (JobSchedule)
	free(JobSchedule);

    JobSchedule  = NULL;
    nJobSchedule = 0;

    /* 
     * Walk the list of "running" jobs, adding Start and End entries
     * to the global JobSchedule table along the way.
     */
    for (i = 0; i < nJRs; i++) {

	/* Start Entry */
	++nJobSchedule;
	JobSchedule = realloc(JobSchedule, nJobSchedule * sizeof *JobSchedule);
	if (!JobSchedule) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
	        id, "realloc(JobSchedule)");
	    return;
	}
	JobSchedule[nJobSchedule -1].event    = 'S';
	JobSchedule[nJobSchedule -1].time     = runningJobs[i]->mtime;
	JobSchedule[nJobSchedule -1].cpu      = runningJobs[i]->ncpus;
	JobSchedule[nJobSchedule -1].mem      = runningJobs[i]->memory;
	JobSchedule[nJobSchedule -1].walltime = runningJobs[i]->walltime +2*60;
	strcpy(JobSchedule[nJobSchedule -1].qname, runningJobs[i]->qname);
	strcpy(JobSchedule[nJobSchedule -1].group, runningJobs[i]->group);
	strcpy(JobSchedule[nJobSchedule -1].arch, 
	    get_queue_arch(runningJobs[i]->qname));

	/* End Entry */
	++nJobSchedule;
	JobSchedule = realloc(JobSchedule, nJobSchedule * sizeof *JobSchedule);
	if (!JobSchedule) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
	        id, "realloc(JobSchedule)");
	    return;
	}
	JobSchedule[nJobSchedule -1].event 	= 'E';
	JobSchedule[nJobSchedule -1].time 	= 
		schd_TimeNow + runningJobs[i]->time_left + 2*60;
	JobSchedule[nJobSchedule -1].cpu 	= runningJobs[i]->ncpus;
	JobSchedule[nJobSchedule -1].mem 	= runningJobs[i]->memory;
	JobSchedule[nJobSchedule -1].walltime 	= 0;
	strcpy(JobSchedule[nJobSchedule -1].qname, runningJobs[i]->qname);
	strcpy(JobSchedule[nJobSchedule -1].group, runningJobs[i]->group);
	strcpy(JobSchedule[nJobSchedule -1].arch, 
	    get_queue_arch(runningJobs[i]->qname));
    }

    /* now sort the table into chronological order */
    qsort(JobSchedule, nJobSchedule, sizeof *JobSchedule, compare_qtime);

    schd_print_schedule();

    return;
}

void schd_print_schedule()
{
    int i;

    /* print Schedule for sanity check */
#if 0
    printf("DEBUG: (job schedule)\n");
    for (i=0; i<nJobSchedule; i++) {
	printf("%c %ld %s %d cpus %ld(b)memory %ld\n", JobSchedule[i].event,
		JobSchedule[i].time, JobSchedule[i].qname, JobSchedule[i].cpu,
		schd_byte2val(JobSchedule[i].mem), JobSchedule[i].walltime);
    }
#endif
}

void schd_dump_schedule(QueueList *qlist)
{
    int i, avail_cpu;
    size_t avail_mem;
    Queue  *queue, *best_queue;
    QueueList *qptr;
    FILE *fptr;

    fptr = fopen(schd_PREDICTED_SCHEDULE, "w");
    if (fptr == NULL) {
	;
    }

    for (qptr = qlist; qptr != NULL; qptr = qptr->next) {
        queue = qptr->queue;

	if (queue->rsrcs == NULL)
	    continue;

	avail_cpu = queue->rsrcs->ncpus_total;
	avail_mem = queue->rsrcs->mem_total;
        for (i=0; i<nJobSchedule; i++) {
	    if (!strcmp(queue->qname,JobSchedule[i].qname)) {
		if (JobSchedule[i].event == 'S') {
		    avail_cpu -= JobSchedule[i].cpu;
		    avail_mem -= JobSchedule[i].mem;
		}
		if (JobSchedule[i].event == 'E') {
		    avail_cpu += JobSchedule[i].cpu;
		    avail_mem += JobSchedule[i].mem;
		}
	        fprintf(fptr, "%s %s %ld %d cpus %ld mem\n",
		    queue->rsrcs->arch, queue->qname, JobSchedule[i].time,
		    avail_cpu, avail_mem);
	    }
	}
    }
    fclose(fptr);
}

void schd_update_schedule(Job *job, Queue *queue, time_t start, char *reason)
{
    char   *id = "update_schedule";
    char   tmpstring[500];
    char   tmpstring2[500];
    char  *pstr;
    struct tm *tmptr;
    int i;

    /* Start Entry */
    ++nJobSchedule;
    JobSchedule = realloc(JobSchedule, nJobSchedule * sizeof *JobSchedule);
    if (!JobSchedule) {
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
            id, "realloc(JobSchedule)");
        return;
    }
    JobSchedule[nJobSchedule -1].event       = 'S';
    JobSchedule[nJobSchedule -1].time        = start;
    JobSchedule[nJobSchedule -1].cpu         = job->ncpus;
    JobSchedule[nJobSchedule -1].mem         = job->memory;
    JobSchedule[nJobSchedule -1].walltime    = job->walltime + 2*60;
    strcpy(JobSchedule[nJobSchedule -1].qname, queue->qname);
    strcpy(JobSchedule[nJobSchedule -1].group, job->group);
    strcpy(JobSchedule[nJobSchedule -1].arch, 
	    get_queue_arch(queue->qname));

    /* End Entry */
    ++nJobSchedule;
    JobSchedule = realloc(JobSchedule, nJobSchedule * sizeof *JobSchedule);
    if (!JobSchedule) {
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
            id, "realloc(JobSchedule)");
        return;
    }
    JobSchedule[nJobSchedule -1].event       = 'E';
    JobSchedule[nJobSchedule -1].time        = start + job->walltime;
    JobSchedule[nJobSchedule -1].cpu         = job->ncpus;
    JobSchedule[nJobSchedule -1].mem         = job->memory;
    JobSchedule[nJobSchedule -1].walltime    = 0;
    strcpy(JobSchedule[nJobSchedule -1].qname, queue->qname);
    strcpy(JobSchedule[nJobSchedule -1].group, job->group);
    strcpy(JobSchedule[nJobSchedule -1].arch, 
	    get_queue_arch(queue->qname));

    /* now sort the table into chronological order */
    qsort(JobSchedule, nJobSchedule, sizeof *JobSchedule, compare_qtime);

    /* finially, update the job comment */

    if (reason) {
        if ((pstr = strstr(reason, "Est.")) != NULL)
            strcpy(tmpstring2, reason+16);
        else
            strcpy(tmpstring2, reason);
    } else
        sprintf(tmpstring2, "%s (%s)", schd_JobMsg[NO_ARCH], job->arch);
  
    tmptr = localtime(&start);
    sprintf(tmpstring, "Est.%0.2d/%0.2d %0.2d:%0.2d] %s",
        tmptr->tm_mon+1, tmptr->tm_mday, tmptr->tm_hour,
        tmptr->tm_min, tmpstring2);
    strcpy(reason, tmpstring);
}

time_t schd_when_can_job_start(Job *job, Queue *queue, char *reason)
{
    char *id = "when_can_job_start";
    int i, count, now_cpu, now_run, can_run, arch_cpu, save_arch_cpu;
    size_t now_mem, arch_mem, save_arch_mem;
    time_t steptime, lasttime, possible_start;

    count   = 0;
    now_cpu = 0;
    now_mem = 0;
    now_run = 0;
    arch_cpu = 0;
    arch_mem = 0;
    save_arch_cpu = 0;
    save_arch_mem = 0;
    lasttime = 0;
    steptime = 0;
    possible_start = 0;

    /*
     * walk schedule looking for a window in time in which we could
     * run this job on this queue/exechost
     */

    for (i=0; i<nJobSchedule; i++) {

	/* Count up running jobs for the requested arch; we'll need
	 * this info later if we find a matching slot/host.
	 */

        if ((!strcmp(JobSchedule[i].arch, queue->rsrcs->arch)) &&
           (!strcmp(JobSchedule[i].group, job->group))) {
            if (JobSchedule[i].event == 'S') {
	        arch_cpu += JobSchedule[i].cpu;
	        arch_mem += JobSchedule[i].mem;
	    }

	    if (JobSchedule[i].event == 'E') {
	        arch_cpu -= JobSchedule[i].cpu;
		if (arch_cpu < 0)
		    arch_cpu = 0;

		if (arch_mem < JobSchedule[i].mem)
		    arch_mem = 0;
		else
	            arch_mem -= JobSchedule[i].mem;
	    }

	    if (JobSchedule[i].time <= schd_TimeNow) {
		save_arch_cpu = arch_cpu;
		save_arch_mem = arch_mem;
	    }
	}

        /* Only look at scheduled events for the requested queue */
        if (!strcmp(JobSchedule[i].qname, queue->qname)) {
	    count++;

            if (JobSchedule[i].event == 'S') {
	        now_cpu += JobSchedule[i].cpu;
	        now_mem += JobSchedule[i].mem;
                now_run ++;
	    }

	    if (JobSchedule[i].event == 'E') {
	        now_cpu -= JobSchedule[i].cpu;
	        now_mem -= JobSchedule[i].mem;
	        now_run --;
	    }


 	    can_run = schd_job_could_run(now_cpu, now_mem, now_run,
		arch_mem, arch_cpu, job, queue, reason);

	    if (JobSchedule[i].time <= schd_TimeNow) {
	        if (can_run)
	            possible_start = schd_TimeNow;
		else
	            possible_start = 0;
	    }

	    if (JobSchedule[i].time > schd_TimeNow)  { /* future time */

		if (possible_start) {
 	            if (possible_start + job->walltime + 2*60 <
		        JobSchedule[i].time)
  		        break;

		    if (can_run)
                        steptime = possible_start;

	        } else {
 	            if (can_run)
                        steptime = JobSchedule[i].time;
 	        }

    	        if (can_run)
	            possible_start = steptime;
	        else
		    possible_start = 0;
	    }
	}
    }

    if (nJobSchedule == 0 || count == 0) {
	can_run = schd_job_could_run(0, 0, 0, save_arch_mem, save_arch_cpu,
	    job, queue, reason);

        if (can_run)
	    possible_start = schd_TimeNow;
    }

    if (possible_start != 0 && possible_start < schd_TimeNow)
	    possible_start = schd_TimeNow;

    return(possible_start);
}
