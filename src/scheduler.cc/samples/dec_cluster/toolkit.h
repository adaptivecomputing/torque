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

#ifndef TOOLKIT_H_
#define TOOLKIT_H_

#include <sys/types.h>
#include <time.h>			/* For time_t declaration. */

#include "pbs_config.h"
#include "pbs_ifl.h"

#define	UNSPECIFIED -1
#define MAX_PRIORITIES 20

/*
 * Turn On/Off Debugging Info
 */
#ifdef	DEBUG
#ifndef DBPRT
#define	DBPRT(x)	printf x;
#endif  /* !DBPRT */
#else  	/* !DEBUG */
#ifndef DBPRT
#define	DBPRT(x)
#endif	/* !DEBPRT */
#endif 	/* DEBUG */

/*
 * Location of the accounting HOLIDAYS file.
 */
#ifndef HOLIDAYS_FILE
#define HOLIDAYS_FILE "/usr/lib/acct/holidays"
#endif /* ! HOLIDAYS_FILE */

/*
 * It may take this long for mom to kill a job that runs over its expected
 * run time (i.e. a "1:00:00" job may run for up to 5 minutes longer than
 * the expected hour).  When this is the case, the time left for the job to
 * complete becomes negative.  Since this is clearly bogus, and there is no
 * way to tell when PBS will actually terminate the job, adjust the time
 * remaining for any job that has passed its expected completion time to
 * be JOB_OVERTIME_SLOP seconds in the future.
 */
#define JOB_OVERTIME_SLOP	(3 * 60)

/*
 * Lengths of strings to allocate for holding dates and time in the following
 * formats.  Used for scheduled outages.
 */
#define DATE_LENGTH	10	/* length of "MM/DD/YYYY" */
#define TIME_LENGTH	5	/* length of "HH:MM" */

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif /* ! MAX */
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif /* ! MIN */

/*
 * One of the more time-consuming activities in the scheduler is modifying
 * the job comments (since it requires a round trip with the server on each
 * comment).  In order to reduce the impact this commenting has on the time
 * that each scheduling iteration takes, comment jobs only the first time
 * they are seen, or after their last modification time ages to more than 
 * MIN_COMMENT_AGE seconds old.
 */
#define MIN_COMMENT_AGE		(5 * 60)

/* Valid state count search strings */
#define SC_TRANSIT "Transit"
#define SC_QUEUED  "Queued"
#define SC_HELD    "Held"
#define	SC_WAITING "Waiting"
#define	SC_RUNNING "Running"
#define	SC_EXITING "Exiting"

/* Names of Custom Job/Queue Attributes for this scheduler. To change
 * the name of one of these attributes, change the name in quotes below,
 * and change it in PBSSRC/src/include/site_resc_attr_def.h
 */
#define FEATURE_A "featureA"	/* String */
#define FEATURE_B "featureB"	/* String */
#define FEATURE_C "featureC"	/* String */
#define FEATURE_D "featureD"	/* Long Integer */
#define FEATURE_E "featureE"	/* Long Integer */
#define FEATURE_F "featureF"	/* Long Integer */
#define FEATURE_G "featureG"	/* Boolean Integer */
#define FEATURE_H "featureH"	/* Boolean Integer */
#define FEATURE_I "featureI"	/* Boolean Integer */

/* 
 * Typedef's for external PBS declarations. 
 */

typedef struct batch_status Batch_Status;
typedef struct attrl AttrList;
typedef struct attropl AttrOpList;

struct job {
    struct job *next;

    /* 
     * The following attributes are strdup()'d and must be freed before 
     * the last handle to the job is given up.
     */
    char   *jobid;		/* Opaque job identifier.             */
    char   *owner;		/* Username of owner of this job.     */
    char   *exechost;		/* Host on which this job is running. */
    char   *runon;		/* Host on which this job must run.   */
    char   *group;		/* Group of the owner of this job.    */
    char   *comment;		/* Current comment on this job.       */
    char   *oqueue;		/* Originating queue name. 	      */
    char   *arch;		/* Requested Architecture of this job */ 

    /*
     * This value is strdup()'d when a batch_struct is converted to 
     * a Job struct.  When the Job is claimed by a queue, the 'queue'
     * field is pointed to its owner.  'qname' is then free()'d and
     * pointed to the queue->qname field.
     */
    char   *qname;		/* Name of queue this job lives in.   */
    struct queue *queue;	/* Back pointer to queue struct.      */
    char    flags; 		/* Flags for the state of this job:   */
/*
 * JFLAGS_INTERACTIVE:	Job is an "interactive" job (i.e. "qsub -I")
 * JFLAGS_QNAME_LOCAL:	Job->qname points to a piece of storage, i.e.
 *			    it is not a reference to a Queue->qname.
 * JFLAGS_FIRST_SEEN:	Job has arrived since the last scheduler run.
 * JFLAGS_PRIORITY:	Job came from a high-priority queue.
 */
#define JFLAGS_INTERACTIVE	(1 << 0)
#define JFLAGS_QNAME_LOCAL	(1 << 1)
#define JFLAGS_FIRST_SEEN       (1 << 3)
#define JFLAGS_PRIORITY	        (1 << 4)

    int     ncpus;		/* Number of ncpus requested by job.  */
    int     time_queued;        /* How many secs since job queued.    */
    int     eligible;           /* How many secs job eligible to run. */
    int     time_left;		/* How many secs remain for this job. */
    char    state;		/* Is the job Running, Queued, etc    */
    size_t  memory;		/* Amount of memory requested by job  */
    size_t  tmpdir;		/* Amount of /tmp disk space needed   */
    time_t  walltime;		/* Wallclock time the job requested.  */
    time_t  walltime_used;	/* Wallclock time used by the job.    */
    time_t  mtime;		/* When this job was last modified.   */
    int	    speed;		/* Min CPU speed requested by the job */
    int	    priority;		/* Priority from Originating Queue    */
    char   *featureA;		/* Custom featureA: string	      */
    char   *featureB;		/* Custom featureB: string	      */
    char   *featureC;		/* Custom featureC: string	      */
    long    featureD;		/* Custom featureD: long	      */
    long    featureE;		/* Custom featureE: long	      */
    long    featureF;		/* Custom featureF: long	      */
    int     featureG;		/* Custom featureG: bool (int)	      */
    int     featureH;		/* Custom featureH: bool (int)	      */
    int     featureI;		/* Custom featureI: bool (int)	      */
};
typedef struct job Job;

struct useracl {
    struct useracl *next;
    char    user[PBS_MAXUSER + 1];
    char   *host;
};
typedef struct useracl UserAcl;

/* This structure is used to hold information about a queue. */
struct queue {
    /* The following fields are retained across scheduling interations. */
    char   *qname;		/* Pointer to queuename for this info */
    char   *exechost;		/* Host on which this queue executes. */
    time_t  idle_since;		/* The time this queue last went idle */
    int     observe_pt;		/* Observe primetime for this queue?  */

    /* These fields are recomputed for each run of the scheduler. */
    struct resources *rsrcs;	/* Resources for this execution host. */

/*
 * Flags for the internal state of the queue:
 *
 *   QFLAGS_STOPPED:	Queue is stopped -- do not attempt to run jobs on it.
 *   QFLAGS_DISABLED:	Queue is disabled -- do not queue or run jobs on it.
 *   QFLAGS_FULL:	Queue has jobs running that consume all of at least
 *				one of the queue's limited resources.
 *   QFLAGS_MAXRUN:	Queue has enough jobs running.  Resources may or may
 *				not be available, but don't run more jobs.
 *   QFLAGS_DRAINING:	Queue is being drained, possibly to run a long-waiting
 *				large job.
 *   QFLAGS_USER_ACL:	Queue has an active user access control list.
 */
    int     flags;		/* Flags for this queue (see above).  */
#define QFLAGS_STOPPED		(1 << 0)
#define QFLAGS_DISABLED 	(1 << 1)
#define QFLAGS_FULL	 	(1 << 2)
#define QFLAGS_MAXRUN	 	(1 << 3)
#define QFLAGS_DRAINING	 	(1 << 4)
#define QFLAGS_NODEDOWN		(1 << 5)
#define QFLAGS_USER_ACL		(1 << 6)

    time_t  empty_by;		/* When this queue will be empty      */
    time_t  drain_by;		/* When this queue will be drained    */
				/*   enough to run the waiting job    */
    Job    *jobs;		/* Pointer to list of jobs on queue   */
    UserAcl *useracl;		/* Linked list of user ACL's          */

    int     running;		/* Num jobs running                   */
    int     queued;		/* Num jobs queued                    */
    int     maxrun;		/* Queue run limit                    */
    int     userrun;		/* Queue per-user run limit           */
    int     ncpus_max;		/* Maximum CPU usage                  */
    int     ncpus_min;		/* Minimum CPU usage                  */
    int     ncpus_default;	/* Default CPU request for jobs       */
    size_t  mem_max;		/* Maximum Memory usage               */
    size_t  mem_min;		/* Minimum Memory usage               */
    size_t  mem_default;	/* Default Memory request for jobs    */
    time_t  wallt_max;		/* Maximum walltime allowed (sec)     */
    time_t  wallt_min;		/* Minimum walltime allowed (sec)     */
    time_t  wallt_default;	/* Default walltime for jobs (sec)    */
    int     ncpus_assn;		/* Number of ncpus assigned (used)    */
    size_t  mem_assn;		/* Amount of memory assigned (used)   */
    int     speed;		/* Maximum CPU speed of node	      */
    int     priority;		/* Priority value for this queue      */
    char   *featureA;		/* Custom featureA: string	      */
    char   *featureB;		/* Custom featureB: string	      */
    char   *featureC;		/* Custom featureC: string	      */
    long    featureD;		/* Custom featureD: long	      */
    long    featureE;		/* Custom featureE: long	      */
    long    featureF;		/* Custom featureF: long	      */
    int     featureG;		/* Custom featureG: bool (int)	      */
    int     featureH;		/* Custom featureH: bool (int)	      */
    int     featureI;		/* Custom featureI: bool (int)	      */
};
typedef struct queue Queue;

/*
 * External list of queues.  Needed so that there can be multiple
 * distinct orderings of the same set of structs.
 */
struct queuelist {
    struct queuelist *next;
    Queue    *queue;
};
typedef struct queuelist QueueList;

/* This structure holds the values returned by the resource monitor. */
struct resources {
    struct resources *next;	/* Pointer to next resource list  */
    char   *exechost;		/* Name of this execution host    */
    char   *arch;		/* Architecture of execution host */
    int     usrtime;		/* % wall time spent in user code */
    int     systime;		/* % wall time spent in syscalls  */
    int     idltime;		/* % wall time spent in idle loop */
    int     flags;		/* Miscellaneous flags.           */
    int     njobs;		/* Number of jobs running         */
    int     ncpus_total;	/* Number of ncpus on system      */
    int     ncpus_alloc;	/* Number of ncpus allocated      */
    double  loadave;		/* System load average            */
    size_t  freemem;		/* Amount of free memory          */
    size_t  tmpdir;		/* Amount of free memory          */
    size_t  mem_total;		/* Amount of memory on system     */
    size_t  mem_alloc;		/* Amount of memory allocated     */
};
typedef struct resources Resources;

#ifndef MAX_TXT
#define MAX_TXT 127
#endif /* ! MAX_TXT */

/* User information. */
struct Uinfo {
    char name[MAX_TXT+1];
    int running_jobs;
    int remaining_time;
    int ncpus_used;
    size_t mem_used;
};

struct fairacl {
    char name[30];		/* user name                     */
    int  max_rjobs;		/* maximum running jobs limit    */
    int  max_minutes;		/* maximum time-credit (minutes  */
    struct fairacl *next;
};
typedef struct fairacl FairACL;
    
struct accessentry {
    char    *name;		/* queue name for access entry   */
    FairACL *list;		/* list of users and limits      */
    struct accessentry *next;	
};
typedef struct accessentry AccessEntry;

struct accesslist {
    struct accesslist *next;	/* UNUSED at this time */
    AccessEntry       *entry;
};
typedef struct accesslist FairAccessList;

struct q_priorities {
    char qname[MAX_TXT+1];
    int  priority;
};
typedef struct q_priorities Qpriorities;

/*****************************************************************************/
/*          Exported function prototypes for the scheduler toolkit.          */
/*****************************************************************************/

/* acl_support.c */
UserAcl *schd_create_useracl (char *useracl);
int schd_free_useracl (UserAcl *);
int schd_useracl_okay (Job *job, Queue *queue, char *reason);

/* cleanup.c */
int schd_cleanup (void);
int cleanup_rsrcs(Resources *rsrcs);

/* comment.c */
#define JOB_COMMENT_REQUIRED		0
#define JOB_COMMENT_OPTIONAL		1
void schd_comment_job (Job *job, char *reason, int optional);
void schd_comment_server (char *reason);
int schd_alterjob(int sv_conn, Job *job, char *name, char *value, char *rsrc);

/* evaluate_system.c */
int schd_evaluate_system(Resources *rsrcs, char *reason);

/* fair_access.c */
int arg_to_fairacl(char *arg, char *sep, FairAccessList **fairacl_ptr);
void schd_print_fairacl(void);
int schd_user_accesslimits(char *user, char *qname, int *maxjobs, int *maxtime);
int schd_job_exceeds_fairaccess(Job *job, Queue *queue, char *reason);

/* getconfig.c */
int schd_get_config (char *filename);
void schd_print_fairacl(void);


/* getjobs.c */
Job *schd_get_jobs (char *qname, char *state);

/* getqueues.c */
int schd_get_queues(void);
int schd_get_queue_limits(Queue *queue);
int schd_get_queue_util(void);
int schd_get_queue_info(Queue *queue);
int queue_claim_jobs(Queue *queue, Job **joblist_ptr);
int queue_sanity(Queue *queue);
int get_node_status(void);

/* getrsrcs.c */
Resources *schd_get_resources (char *exechost);
void schd_dump_rsrclist (void);

/* jobinfo.c */
int schd_get_jobinfo (Batch_Status *bs, Job *job);
int schd_free_jobs (Job *list);

/* overlaps.c */
Queue *schd_find_drain(QueueList *qlist, Job *job);

/* pack_queues.c */
int   schd_pack_queues (Job *jlist, QueueList *qlist, char *reason);
Queue *find_best_exechost(Job *job, QueueList *qlist, char *reason);

/* queue_limits.c */
int schd_check_queue_limits (Queue *queue, char *reason);
int schd_job_fits_queue (Job *job, Queue *queue, char *reason);
int schd_job_can_queue (Job *job);

/* resource_limits.c */
int schd_resources_avail (Job *job, Resources *rsrcs, char *reason);

/* rejectjob.c */
int schd_reject_job(Job *job, char *reason);

/* runjob.c */
#define SET_JOB_COMMENT 1
#define LEAVE_JOB_COMMENT 0
int schd_run_job_on (Job *job, Queue *queue, char *exechost, int set_comment);
int schd_charge_job (Job *job, Queue *queue, Resources *rsrcs);

/* schedinit.c */
int schedinit (int argc, char **argv);

/* time_limits.c */
int schd_primetime_limits (Job *job, Queue *queue, time_t when, char *reason);
int schd_finish_before_np(Job *job, Queue *queue, time_t when, char *reason);

/* toolkit.c */
extern	char	schd_VersionString[];
int     schd_register_file (char *filename);
int     schd_file_has_changed (char *filename, int reset_stamp);
int     schd_forget_file (char *filename);

char   *schd_byte2val(size_t bytes);
char   *schd_bool2val (int bool);
char   *schd_booltime2val (time_t bool);
char   *schd_sec2val (int seconds);
int     schd_val2bool (char *val, int *bool);
int     schd_val2booltime (char *val, time_t *t);
int     schd_val2datetime (char *string, time_t *when);
size_t  schd_val2byte (char *val);
time_t  schd_val2sec (char *val);

void	init_holidays (void);
int     schd_read_holidays (void);
void    schd_check_primetime(void);
int     schd_prime_time (time_t when);
int     schd_secs_til_prime (time_t when);
int     schd_secs_til_nonprime (time_t when);
int     schd_reset_observed_pt (QueueList *qlist);

char   *schd_strdup (char *string);
char   *schd_lowercase (char *string);
char   *schd_shorthost (char *fqdn);
char   *schd_getat (char *at, Batch_Status *bs, char *rs);
int     schd_how_many (char *str, char *state);
int     schd_move_job_to (Job *job, Queue *destq);
int     schd_free_qlist (QueueList *qlist);
int     schd_destroy_qlist (QueueList *qlist);
void    schd_timestamp (char *msg);

/* user_limits.c */
int schd_user_limits (Job *job, Queue *queue, char *reason);

/* usersort.c */
Job *schd_sort_jobs (Job *jobs);
void get_users(void);

#endif /* ! TOOLKIT_H_ */
