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
#include <sys/stat.h>

#include "toolkit.h"
#include "gblxvars.h"

#include "pbs_error.h"
#include "log.h"

extern int connector;

int    schd_NeedToGetShareInfo;
static time_t when_shares_saved;
time_t when_shares_done;

/* Per-user usage information. */
struct Uinfo *Users = NULL;
int nUsers;

/* Per-class job lists.  Point to a set of jobs in the input list. */
static Job **normalQ     = NULL;
static Job **otherQ      = NULL;
static Job **runningJobs = NULL;
static Job  *make_job_list (void);
static int   nnormalQ, notherQ, nJQs, nJRs;
static void  sort_jobs(void);
static void  mark_overusage_jobs    (void);
static int   split_jobs             (Job *jobs);
static int   compare_fairshare      (const void *e1, const void *e2);
static int   compare_running        (const void *e1, const void *e2);
static int   compare_users          (const void *e1, const void *e2);
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

    /* Create a list of all users with queued and running jobs */
    get_users();

    /* Mark all jobs that are primary candidates for checkpointing. */
    mark_overusage_jobs();

    /* Sort jobs in normalQ, taking into consideration their past system
     * usage (percentage shares of system), number of jobs queued and 
     * running, etc. */
    sort_jobs();

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
     * Sort the list of "normal" jobs, based upon the proper criterion
     * for the current time of day.
     */
    qsort(normalQ, nnormalQ, sizeof *normalQ, compare_fairshare);

    /* 
     * Sort the list of running jobs, in ascending order by the expected
     * time to completion.
     */
    qsort(runningJobs, nJRs, sizeof *runningJobs, compare_running);

    return;
}

/*
 * qsort() function to order jobs by fair share algorithm
 *
 * order jobs by share_rank; break ties by:
 * order shortest jobs first (ascending order of walltime requested)
 * ascending order of requested number of CPUs.
 */
static int
compare_fairshare(const void *e1, const void *e2)
{
    Job    *job1 = *(Job **)e1;
    Job    *job2 = *(Job **)e2;
    int     running_A, total_A, max_A;
    int     running_B, total_B, max_B;


    /* sort PRIORITY jobs to top to list, in fifo order 
     * GOAL: give highest priority to a select few jobs
     */
    if (job1->flags & JFLAGS_PRIORITY || job2->flags & JFLAGS_PRIORITY) {
	if (!(job1->flags & JFLAGS_PRIORITY && job2->flags & JFLAGS_PRIORITY)) {
	    /* only one of the two has priority... */

	    if (job1->flags & JFLAGS_PRIORITY)
	        return -1;
	    else
	        return 1;
	}
    }

    count_user_jobs(job1, &running_A, &total_A, &max_A); 
    count_user_jobs(job2, &running_B, &total_B, &max_B); 

    /* sort WAITING jobs to near top to list, in fifo order, but discount
     * any waiting jobs from any user who already has at least MAX_RUNNING
     * jobs running.
     * GOAL: give higher priority to a long-waiting jobs, but don't let a
     * WAITING job exceed the max run limit based on its WAIT status alone.
     */
    if (job1->flags & JFLAGS_WAITING || job2->flags & JFLAGS_WAITING) {
	if (!(job1->flags & JFLAGS_WAITING && job2->flags & JFLAGS_WAITING)) {
	    /* only one of the two is waiting... */

	    if (job1->flags & JFLAGS_WAITING) {
		if (running_A < max_A)
	            return -1;
	    } else {
		if (running_B < max_B)
	            return 1;
	    }
	}
    }

    /* sort jobs with least percentage used (shares) to highest used 
     * GOAL: provide fair distribution at the Queue/Share level
     */
    if (job1->sort_order < job2->sort_order)
        return -1;
    if (job1->sort_order > job2->sort_order)
        return 1;


    /* For the remaining tests...
     * GOAL: provide fair distribution amoung users within the same 
     *       Share/Queue level
     */

    /* if running this job would cause the user to exceed the running
     * jobs soft limit, then opt for the other job, but skip to next
     * test if *both* jobs would go over the limit.
     */
    if (!(running_A + 1 > max_A && running_B + 1 > max_B )) {

        if (running_A + 1 > max_A )
            return 1;
        if (running_B + 1 > max_B )
            return -1;
    }

    if (total_A < total_B)
        return -1;
    if (total_A > total_B)
        return 1;


    /* If we have fallen through to this point, give preference to 
     * suspended jobs, since we would like to clear them out of the
     * way; they would not have been suspended if a higher priority
     * job had not come along.
     */
    if (job1->flags & JFLAGS_SUSPENDED || job2->flags & JFLAGS_SUSPENDED) {
      if (!(job1->flags & JFLAGS_SUSPENDED && job2->flags & JFLAGS_SUSPENDED)){
	    /* only one of the two is suspended... */

	    if (job1->flags & JFLAGS_SUSPENDED)
	        return -1;
	    else
	        return 1;
	}
    }
    /* otherwise, opt for checkpointed jobs, since they are waiting
     * anyway...
     */
    if (job1->flags & JFLAGS_CHKPTD || job2->flags & JFLAGS_CHKPTD) {
	if (!(job1->flags & JFLAGS_CHKPTD && job2->flags & JFLAGS_CHKPTD)) {
	    /* only one of the two is checkpointed... */

	    if (job1->flags & JFLAGS_CHKPTD)
	        return -1;
	    else
	        return 1;
	}
    }

    /* if we get this far, break ties by sorting shortest to longest. */
    if (job1->walltime < job2->walltime)
	return -1;
    if (job1->walltime > job2->walltime)
	return 1;

    /* If everything else is equal, then sort oldest to youngest. */
    if (job1->eligible > job2->eligible)
	return -1;
    if (job1->eligible < job2->eligible)
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

/*
 * Construct a list of users who own one or more "normal" jobs
 */
void get_users(void)
{
    char   *id = "get_users";
    Job    *job_ptr;
    char   *uname, *name;
    char   que_usr_tuple[120]; 
    int     which, i, j, max, unused;
    static int is_new_user();
    static int make_uinfo();

    /*
     * Destroy any previously created list.
     */
    if (Users)
	free(Users);

    Users  = NULL;
    nUsers = 0;

    /* 
     * Walk the list of "normal" jobs, adding any new users along the way.
     */
    for (i = 0; i < nnormalQ; ++i) {

	job_ptr = normalQ[i];
	sprintf(que_usr_tuple, "%s:%s", job_ptr->owner, job_ptr->oqueue);

	/* Is this a new entry in the list? */
	if (is_new_user(que_usr_tuple, Users, nUsers, &which)) {

	    ++nUsers;

	    Users = realloc(Users, nUsers * sizeof *Users);
	    if (!Users) {
		log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
			   id, "realloc(Users)");
		return;
	    }
	    make_uinfo(que_usr_tuple, &(Users[nUsers - 1]));
	} else {
	    Users[which].jobcount++;
	}
    }

    /* Now, walk the list of running jobs and record each user's count. */
    for (i = 0; i < nUsers; ++i) {
	uname = Users[i].name;
	for (j = 0; j < nJRs; ++j) {
	    job_ptr = runningJobs[j];
	    sprintf(que_usr_tuple, "%s:%s", job_ptr->owner, job_ptr->oqueue);

	    if (!strcmp(que_usr_tuple, uname)) {
		Users[i].running_jobs   ++;
	        schd_accesslimits(job_ptr->oqueue, &max, &unused);
		if (Users[i].running_jobs > max)
		    job_ptr->flags |= JFLAGS_RUNLIMIT;
	    }
	    ++job_ptr;
	}
    }
    return;
}

/* Search the list of users, looking for 'user'. */
static int
is_new_user(char *user, struct Uinfo *list, int len, int *which)
{
    struct Uinfo *list_ptr = list;
    int     i;

    for (i = 0; i < len; ++i) {
	if (!strcmp(list_ptr->name, user)) {
	    *which = i;
	    return 0;
	}
	++list_ptr;
    }

    return 1;
}

/* Build a record of the pertinent data about a job owner. */
static int
make_uinfo(char *user, struct Uinfo *uinfo)
{
    strncpy(uinfo->name, user, sizeof(uinfo->name) - 1);
    uinfo->running_jobs   = 0;
    uinfo->jobcount       = 1;

    return 0;
}

/* Determine if it is time to save the Share Usage information to disk */
int
schd_save_shares(void)
{
    					/* save shares hourly */
    if (schd_TimeNow >= (when_shares_saved + 3600))
	return 1;
    return 0;
}

/* 
 * Read or write the Share Usage data to the disk file.
 */
void
schd_share_info(char *mode)
{
    char   *id = "schd_share_info";
    FairAccessList *Fptr;
    AccessEntry    *AEptr;
    FILE   *shares;
    char   *p;
    char    buffer[MAX_TXT + 1 + 1]; /* size = MAX_TXT + a newline + NULL */
    char    qname[50], share_today[10], share_day[10];
    int     do_shares = 0;
    int	    past_ndays;
    double  past_percent, today_usage, today_max;


    /* get the current day of the week (name) */
    strftime(share_today, sizeof share_today, "%a", &schd_TmNow);

    /* If there is no pre-existing file, bootstrap it with the date */
    if ((shares = fopen(schd_SHARE_INFOFILE, mode)) == NULL) {

	if ((shares = fopen(schd_SHARE_INFOFILE, "w")) == NULL) {
	    /*
	     * Could not open the file for writing.  Possibly the path is
	     * invalid?
	     */
	    sprintf(log_buffer, "fopen(%s,%s) failed (%d)",
		    schd_SHARE_INFOFILE, mode, errno);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

	    return;
	}

	/* 
	 * The file pointed to by schd_SHARE_INFOFILE is writable.  Create the
	 * bootstrap record.
	 */

	sprintf(buffer, "# Day-of-Week\n# queue  ndays past%% today_usage today_max\n");
	fputs(buffer, shares);

	sprintf(buffer, "%s\n", share_today);
	fputs(buffer, shares);

	fclose(shares);

	sprintf(log_buffer, "wrote bootstrap record");
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

	/* 
	 * Attempt to reopen the file.  If it fails, then the bootstrap failed
 	 * as well.  Otherwise, this leaves the open stream pointer in 'shares'
 	 * as if it were opened successfully above.
	 */
	if ((shares = fopen(schd_SHARE_INFOFILE, mode)) == NULL) {
	    sprintf(log_buffer, "bootstrap:fopen(%s,%s) failed",
		    schd_SHARE_INFOFILE, mode);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    return;
	}
    }

    /* Read or write based on mode. */
    if (mode[0] == 'r') {

	fgets(buffer, sizeof buffer, shares);	/* skip both header comments */
	fgets(buffer, sizeof buffer, shares);

	fgets(buffer, sizeof buffer, shares);	/* save Day-of-week stamp */
	p = strtok(buffer, " \n");
	strcpy(share_day, p);

	while (fgets(buffer, sizeof buffer, shares)) {

	    p = strtok(buffer, " \n");
	    strcpy(qname, p);

	    p = strtok(NULL, " \n");
	    past_ndays = atoi(p);

	    p = strtok(NULL, " \n");
	    past_percent = atof(p);

	    p = strtok(NULL, " \n");
	    today_usage = atof(p);

	    p = strtok(NULL, " \n");
	    today_max = atof(p);

	    
	    /* now save this info in the global per-Queue Shares lists */
            for (Fptr = schd_FairACL; Fptr != NULL; Fptr = Fptr->next) {
                for (AEptr = Fptr->entry; AEptr != NULL; AEptr = AEptr->next) {
                    if (AEptr->name) {
                        if (!strcmp(AEptr->name, qname)) {
	                    AEptr->past_ndays   = past_ndays;
	                    AEptr->past_percent = past_percent;
	                    AEptr->today_usage  = today_usage;
	                    AEptr->today_max    = today_max;

			    if(strcmp(share_today, share_day)) {
	    		        /* If Day is not Today, then we need to   */
				/* apply the "today" usage data to the    */
				/* "past", and clear the today counters;  */
				/* this is needed in the off chance the   */
				/* scheduler stopped between the hourly   */
				/* updates and the daily today-past usage */
				/* conversion. We don't want to loose any */
				/* Share usage data.		 	  */

				update_share_usage(AEptr);
			    }
	    		    sprintf(log_buffer,
			        "%10s: %d shares; averaged %3.1f shares "
				"over past %d days", qname, AEptr->max_percent,
				AEptr->past_percent, AEptr->past_ndays);
	    		    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
				id,log_buffer);
	                }
                    }
                }
            }
	}
	fclose(shares);

	schd_NeedToGetShareInfo = 0;
	when_shares_done = next_shares_update();

    } else if (mode[0] == 'w') {

	do_shares = schd_TimeNow > when_shares_done;

	sprintf(buffer, "# Day-of-Week\n# queue ndays past%% today_usage today_max\n");
	fputs(buffer, shares);
	sprintf(buffer, "%s\n", share_today);
	fputs(buffer, shares);

        for (Fptr = schd_FairACL; Fptr != NULL; Fptr = Fptr->next) {
            for (AEptr = Fptr->entry; AEptr != NULL; AEptr = AEptr->next) {
                if (AEptr->name) {
	            if (do_shares) {

			/* convert today usage into past usage */
			update_share_usage(AEptr);

	    	        sprintf(log_buffer,
		            "%10s: %d shares; averaged %3.1f shares over past %d days",
			    AEptr->name, AEptr->max_percent, AEptr->past_percent,
			    AEptr->past_ndays);
	    	        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
			    log_buffer);
	            }

		    /* now write out the current values */
	    	    fprintf(shares, "%-10s %5d %3.3f %10.3f %10.3f\n",
			AEptr->name, AEptr->past_ndays, AEptr->past_percent,
			AEptr->today_usage, AEptr->today_max);
		}
	    }
	}
	fclose(shares);
	if (do_shares)
	    when_shares_done = next_shares_update();

	when_shares_saved = schd_TimeNow;

    } else {
	sprintf(log_buffer, "unknown mode [%s]", mode);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

    return;
}

static time_t
next_shares_update(void)
{
    struct tm *tmptr;
    time_t  when, next, secSinceMidnite;

    /* Calculate number of seconds elapsed since midnight (00:00:00). */
    time(&when);
    tmptr = localtime(&when);
    secSinceMidnite = ((tmptr->tm_hour * 60 * 60 ) +
                      ( tmptr->tm_min  * 60 )      +
		        tmptr->tm_sec );

	/* schedule next update at 23:00 tonight */
    next = schd_TimeNow - secSinceMidnite + 60*60*23;
		
    return(next);
}

static void
count_user_jobs(Job *job, int *running, int *total, int *max)
{
    char   que_usr_tuple[120];
    int i, unused;
    *running = 0;
    *total   = 0;
    *max     = 1;

    sprintf(que_usr_tuple, "%s:%s", job->owner, job->oqueue);

    for (i = 0; i < nUsers; ++i) {
	if (!strcmp(que_usr_tuple, Users[i].name)) {
	    *running = Users[i].running_jobs;
	    *total   = Users[i].jobcount;
	    schd_accesslimits(job->oqueue, max, &unused);
	    break;
        }
    }
}

static void mark_overusage_jobs(void)
{
    char   *id = "mark_overusage_jobs";
    int i, running, total, max;
    Job    *job_ptr;

    /* Walk running jobs list marking any jobs that are possible candidates
     * for suspension/checkpointing. We do this here so that *if* we need
     * find a job to checkpoint later, we will have already have the list
     * to choose from.
     */
    for (i = 0; i < nJRs; i++) {
	
	job_ptr = runningJobs[i];

	/* skip Priority or Long Waiting jobs */
	if (job_ptr->flags & JFLAGS_PRIORITY ||
	    job_ptr->flags & JFLAGS_WAITING )
	    continue;

	/* check if share usage exceeds 100 % */
	if (job_ptr->sort_order > 100) {
	    job_ptr->flags |= JFLAGS_CHKPT_OK;
	    continue;
	}

	/* can add other checks here */
    }

    /* now mark any Q'd jobs from overusage queue/etc. */
    for (i = 0; i < nnormalQ; i++) {
	job_ptr = normalQ[i];

	/* check if user has exceeded max running jobs limit */
	count_user_jobs(job_ptr, &running, &total, &max);
	if (running > max) {
	    job_ptr->flags |= JFLAGS_RUNLIMIT;
	    continue;
	}

	/* check if share usage exceeds 100 % */
	if (job_ptr->sort_order > 100) {
	    job_ptr->flags |= JFLAGS_CHKPT_OK;
	    continue;
	}
    }
}
