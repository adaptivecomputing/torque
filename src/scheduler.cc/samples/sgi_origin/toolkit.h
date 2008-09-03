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
#include <time.h>   /* For time_t declaration. */
#include "pbs_ifl.h"

static char *schd_VersionString = "Generic Origin 2000 PBS Scheduler (version 2.2.4c)\n";

#define UNSPECIFIED -1

#ifdef NODEMASK
#include "bitfield.h"
#endif /* NODEMASK */

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
#define JOB_OVERTIME_SLOP (3 * 60)

/*
 * Lengths of strings to allocate for holding dates and time in the following
 * formats.  Used for scheduled outages.
 */
#define DATE_LENGTH 10 /* length of "MM/DD/YYYY" */
#define TIME_LENGTH 5 /* length of "HH:MM" */

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif /* ! MAX */
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif /* ! MIN */

/*
 * Macro to compute the number of nodes required to supply the CPU an
 * memory requested.  Note that the MB_PER_NODE definition reflects the
 * physical configuration of the machine.
 *
 * !!!XXX!!!  MB_PER_NODE may vary if different memory configurations are
 * !!!XXX!!!  installed on the systems.  This value is currently correct
 * !!!XXX!!!  for all Origins at NAS, but may not be correct in all cases.
 */
#define MB_PER_NODE     ((size_t)512*1024*1024)
#define PE_PER_NODE     2
#define NODES_FROM_MEM(mem) (int)(((mem) + MB_PER_NODE - 1) / MB_PER_NODE)
#define NODES_FROM_CPU(cpu) (int)(((cpu) + PE_PER_NODE - 1) / PE_PER_NODE)
#define NODES_REQD(cpu,mem) (int)MAX(NODES_FROM_CPU(cpu),NODES_FROM_MEM(mem))

/*
 * One of the more time-consuming activities in the scheduler is modifying
 * the job comments (since it requires a round trip with the server on each
 * comment).  In order to reduce the impact this commenting has on the time
 * that each scheduling iteration takes, comment jobs only the first time
 * they are seen, or after their last modification time ages to more than
 * MIN_COMMENT_AGE seconds old.
 */
#define MIN_COMMENT_AGE  (5 * 60)

/* Valid state count search strings */
#define SC_TRANSIT "Transit"
#define SC_QUEUED  "Queued"
#define SC_HELD    "Held"
#define SC_WAITING "Waiting"
#define SC_RUNNING "Running"
#define SC_EXITING "Exiting"

/*
 * Typedef's for external PBS declarations.
 */

typedef struct batch_status Batch_Status;

typedef struct attrl AttrList;

typedef struct attropl AttrOpList;

struct job
  {

  struct job *next;

  /*
   * The following attributes are strdup()'d and must be freed before
   * the last handle to the job is given up.
   */
  char   *jobid;  /* Opaque job identifier.             */
  char   *owner;  /* Username of owner of this job.     */
  char   *host;  /* Host from whence this job came.    */
  char   *exechost;  /* Host on which this job is running. */
  char   *runon;  /* Host on which this job must run.   */
  char   *group;  /* Group of the owner of this job.    */
  char   *comment;  /* Current comment on this job.       */
  char   *oqueue;  /* Originating queue name.        */

  /*
   * This value is strdup()'d when a batch_struct is converted to
   * a Job struct.  When the Job is claimed by a queue, the 'queue'
   * field is pointed to its owner.  'qname' is then free()'d and
   * pointed to the queue->qname field.
   */
  char   *qname;  /* Name of queue this job lives in.   */

  struct queue *queue; /* Back pointer to queue struct.      */

#ifdef NODEMASK
  Bitfield nodemask;  /* set when moving to execution queue */
#endif /* NODEMASK */

  char    flags;   /* Flags for the state of this job:   */
  /*
   * JFLAGS_INTERACTIVE: Job is an "interactive" job (i.e. "qsub -I")
   * JFLAGS_QNAME_LOCAL: Job->qname points to a piece of storage, i.e.
   *       it is not a reference to a Queue->qname.
   * JFLAGS_WAITING:      Job is an "outstanding waiting" job, i.e it has
   *                          been waiting for so long (MAX_QUEUED_TIME)
   *       we need to go to heroic measures to make
   *       sure it runs.
   * JFLAGS_FIRST_SEEN: Job has arrived since the last scheduler run.
   * JFLAGS_PRIORITY: Job came from a high-priority queue.
   * JFLAGS_DEDICATED: Job came from a dedicated time queue.
   * JFLAGS_NEEDS_HPM: Job needs HPM support.  Make sure the counters on
   *       the execution host are set to user mode while
   *       this job runs.
   */
#define JFLAGS_INTERACTIVE (1 << 0)
#define JFLAGS_QNAME_LOCAL (1 << 1)
#define JFLAGS_WAITING         (1 << 2)
#define JFLAGS_FIRST_SEEN       (1 << 3)
#define JFLAGS_PRIORITY         (1 << 4)
#define JFLAGS_DEDICATED        (1 << 5)
#define JFLAGS_NEEDS_HPM (1 << 6)

  int     nodes;  /* Number of nodes requested by job.  */
  time_t  walltime;  /* Wallclock time the job requested.  */
  time_t  walltime_used; /* Wallclock time used by the job.    */
  int     time_queued;        /* How many secs since job queued.    */
  int     eligible;           /* How many secs job eligible to run. */
  int     time_left;  /* How many secs remain for this job. */
  time_t  mtime;  /* When this job was last modified.   */
  char    state;  /* Is the job Running, Queued, etc    */
  };

typedef struct job Job;

struct useracl
  {

  struct useracl *next;
  char    user[PBS_MAXUSER + 1];
  char   *host;
  };

typedef struct useracl UserAcl;

/* This structure is used to hold information about a queue. */

struct queue
  {
  /* The following fields are retained across scheduling interations. */
  char   *qname;  /* Pointer to queuename for this info */
  char   *exechost;  /* Host on which this queue executes. */
  time_t  idle_since;  /* The time this queue last went idle */
  int     observe_pt;  /* Observe primetime for this queue?  */

  /* These fields are recomputed for each run of the scheduler. */

  struct resources *rsrcs; /* Resources for this execution host. */

  /*
   * Flags for the internal state of the queue:
   *
   *   QFLAGS_STOPPED: Queue is stopped -- do not attempt to run jobs on it.
   *   QFLAGS_DISABLED: Queue is disabled -- do not queue or run jobs on it.
   *   QFLAGS_FULL: Queue has jobs running that consume all of at least
   *    one of the queue's limited resources.
   *   QFLAGS_MAXRUN: Queue has enough jobs running.  Resources may or may
   *    not be available, but don't run more jobs.
   *   QFLAGS_DRAINING: Queue is being drained, possibly to run a long-waiting
   *    large job.
   *   QFLAGS_USER_ACL: Queue has an active user access control list.
   *   QFLAGS_NODEMASK: Queue has a defined nodemask.
   *   QFLAGS_NPT_JOBS: Some jobs would have been runnable on this queue if
   *    primetime were not being observed.
   */
  int     flags;  /* Flags for this queue (see above).  */
#define QFLAGS_STOPPED  (1 << 0)
#define QFLAGS_DISABLED  (1 << 1)
#define QFLAGS_FULL   (1 << 2)
#define QFLAGS_MAXRUN   (1 << 3)
#define QFLAGS_DRAINING   (1 << 4)
#define QFLAGS_USER_ACL  (1 << 6)
#define QFLAGS_NODEMASK  (1 << 7)
#define QFLAGS_NPT_JOBS  (1 << 8)

  time_t  empty_by;  /* When this queue will be empty      */
  time_t  drain_by;  /* When this queue will be drained    */
  /*   enough to run the waiting job    */

  Job    *jobs;  /* Pointer to list of jobs on queue   */
  UserAcl *useracl;  /* Linked list of user ACL's          */

  int     running;  /* Num jobs running                   */
  int     queued;  /* Num jobs queued                    */
  int     maxrun;  /* Queue run limit                    */
  int     userrun;  /* Queue per-user run limit           */

  int     nodes_max;  /* Maximum node usage                 */
  int     nodes_min;  /* Minimum node usage                 */
  int     nodes_default; /* Default node request for jobs      */
  time_t  wallt_max;  /* Maximum walltime allowed (sec)     */
  time_t  wallt_min;  /* Minimum walltime allowed (sec)     */
  time_t  wallt_default; /* Default walltime for jobs (sec)    */

  int     nodes_assn;  /* Number of nodes assigned (used)    */
  int     nodes_rsvd;  /* Number of nodes reserved        */

#ifdef NODEMASK
  Bitfield queuemask;  /* Set of nodes assigned to the queue */
  Bitfield availmask;  /* Nodes available for scheduling     */
#endif /* NODEMASK */
  };

typedef struct queue Queue;

/*
 * External list of queues.  Needed so that there can be multiple
 * distinct orderings of the same set of structs.
 */

struct queuelist
  {

  struct queuelist *next;
  Queue    *queue;
  };

typedef struct queuelist QueueList;

/*
 * This structure contains information about a scheduled outage for a
 * machine.  Each execution host may have one or more scheduled outages
 * listed for it.  This relies upon a clone of the NAS 'schedule' program
 * (see README and dedicated.c for more information.)
 */

struct outage
  {

  struct outage *next;
  char  *exechost;    /* Outage for this host.    */
  time_t beg_time;    /* Time when outage begins. */
  time_t end_time;    /* Time the outage ends.    */
  int    flags;    /* Miscellaneous flags.     */
  /*
   * OUTAGE_FLAGS_SYSTEM:  This is a system outage, not a per-host one.
   */
#define OUTAGE_FLAGS_SYSTEM (1)

  char   beg_datestr[DATE_LENGTH + 1]; /* Date of outage start.    */
  char   beg_timestr[TIME_LENGTH + 1]; /* Time of outage start.    */
  char   end_datestr[DATE_LENGTH + 1]; /* Date of outage end.      */
  char   end_timestr[TIME_LENGTH + 1]; /* Time of outage end.      */
  };

typedef struct outage Outage;

/* This structure holds the values returned by the resource monitor. */

struct resources
  {

  struct resources *next; /* Pointer to next resource list  */
  char   *exechost;  /* Name of this execution host    */
  int     usrtime;  /* % wall time spent in user code */
  int     systime;  /* % wall time spent in syscalls  */
  int     idltime;  /* % wall time spent in idle loop */
  size_t  freemem;  /* Amount of free memory          */
  double  loadave;  /* System load average            */
  int     njobs;  /* number of jobs running         */
  int     nodes_total; /* Number of nodes on system      */
  int     nodes_alloc; /* Number of nodes allocated      */
#ifdef NODEMASK
  Bitfield availmask;  /* Configured nodes on this host  */
  Bitfield nodes_used; /* Nodes used by running jobs     */
#endif /* NODEMASK */
  int     flags;  /* Miscellaneous flags.           */
  /*
   * RSRCS_FLAGS_HPM_USER  The HPM counters are in user-mode on this host.
   * RSRCS_FLAGS_HPM_IN_USE One or more jobs are running on this host, and
   *        have the 'hpm' attribute set.
   */
#define RSRCS_FLAGS_HPM_USER (1)
#define RSRCS_FLAGS_HPM_IN_USE (1 << 1)

  };

typedef struct resources Resources;

/* Per-group allocation and usage database table constants. */
#ifndef MAX_USER_NAME_SIZE
#define MAX_USER_NAME_SIZE 10
#endif /* ! MAX_USER_NAME_SIZE */

#ifndef MAX_GROUP_NAME_SIZE
#define MAX_GROUP_NAME_SIZE 10
#endif /* ! MAX_GROUP_NAME_SIZE */

#ifndef MAX_GROUP
#define MAX_GROUP 1024
#endif /* ! MAX_GROUP */

#ifndef MAX_USERS
#define MAX_USERS 256
#endif /* ! MAX_USERS */

#ifndef MAX_TXT
#define MAX_TXT 127
#endif /* ! MAX_TXT */

/* Group usage and allocation counters. */

struct alloc_group
  {
  char    gname[MAX_GROUP_NAME_SIZE]; /* Name of the group      */
  double  total_usage;  /* Allocation usage YTD   */
  double  allocation;   /* Total Allocation given */
  };

typedef struct alloc_group Alloc_Group;

/*****************************************************************************/
/*          Exported function prototypes for the scheduler toolkit.          */
/*****************************************************************************/

/* acl_support.c */
UserAcl *schd_create_useracl(char *useracl);
int schd_free_useracl(UserAcl *);
int schd_useracl_okay(Job *job, Queue *queue, char *reason);

/* allocations.c */
void schd_alloc_info(void);
int schd_is_over_alloc(char *group);
int schd_reject_over_alloc(Job *job);

/* byte2val.c */
char *schd_byte2val(size_t bytes);

/* cleanup.c */
int schd_cleanup(void);

/* comment.c */
#define JOB_COMMENT_REQUIRED  0
#define JOB_COMMENT_OPTIONAL  1
void schd_comment_job(Job *job, char *reason, int optional);
void schd_comment_server(char *reason);
int schd_alterjob(int sv_conn, Job *job, char *name, char *value, char *rsrc);

/* dedicated.c */
void schd_clear_outage_cache(void);
Outage *schd_host_outage(char *exechost, time_t when);
int schd_dedicated_can_run(Job *job, Queue *srcq, time_t when, char *reason);

/* dedqueue.c */
int schd_handle_dedicated_time(Queue *dedq);

#ifdef NODEMASK
/* dyn_nodemask.c */
char *schd_format_nodemask(Bitfield *mask_mask, Bitfield *ndmask);
int schd_bits2mask(char *string, Bitfield *mask);
int schd_str2mask(char *maskstr, Bitfield *maskp);
int schd_mask2str(char *maskstr, Bitfield *maskp);
int schd_alloc_nodes(int nnodes, Queue *queue, Bitfield *job_mask);
int schd_node_count(Bitfield *mask);
#endif /* NODEMASK */

/* evaluate_system.c */
int schd_evaluate_system(Resources *rsrcs, char *reason);

/* file_chgs.c */
int schd_register_file(char *filename);
int schd_file_has_changed(char *filename, int reset_stamp);
int schd_forget_file(char *filename);

/* fragments.c */
int schd_fragment_okay(Job *job, Queue *queue, char *reason);

/* getat.c */
char *schd_getat(char *at, Batch_Status *bs, char *rs);

/* getconfig.c */
int schd_get_config(char *filename);

/* getjobs.c */
Job *schd_get_jobs(char *qname, char *state);

/* getqueues.c */
int schd_get_queue_limits(Queue *queue);
#ifdef DEBUG
#define QUEUE_DUMP_JOBS  1
#define QUEUE_DUMP_STATS_ONLY 0
void schd_dump_queue(Queue *queue, int dumpjobs);
#endif /* DEBUG */

/* getrsrcs.c */
Resources *schd_get_resources(char *exechost);
void schd_dump_rsrclist(void);

/* how_many.c */
int schd_how_many(char *str, char *state);

/* hpm.c */
int schd_hpm_job_count(Job *joblist);
int schd_hpm_ctl(Resources *rsrcs, int mode, char *reason);
int schd_revoke_hpm(void);

/* jobinfo.c */
int schd_get_jobinfo(Batch_Status *bs, Job *job);
int schd_free_jobs(Job *list);

/* misc.c */
void schd_timestamp(char *msg);
int schd_free_qlist(QueueList *qlist);
int schd_destroy_qlist(QueueList *qlist);
char *schd_strdup(char *string);
char *schd_lowercase(char *string);
char *schd_shorthost(char *fqdn);

/* movejob.c */
int schd_move_job_to(Job *job, Queue *destq);

/* overlaps.c */
Queue *schd_find_drain(QueueList *qlist, Job *job);

/* pack_queues.c */
int schd_pack_queues(Job *jlist, QueueList *qlist, char *reason);

/* pnp.c */
int schd_prime_time(time_t when);
int schd_secs_til_prime(time_t when);
int schd_secs_til_nonprime(time_t when);
int schd_read_holidays(void);
int schd_reset_observed_pt(QueueList *qlist);

/* queue_limits.c */
int schd_job_fits_queue(Job *job, Queue *queue, char *reason);
int schd_job_can_queue(Job *job);
int schd_check_queue_limits(Queue *queue, char *reason);

/* resource_limits.c */
int schd_resource_limits(Job *job, Resources *rsrcs, char *reason);
int schd_resources_avail(Job *job, Resources *rsrcs, char *reason);

/* rejectjob.c */
int schd_reject_job(Job *job, char *reason);

/* runjob.c */
#define SET_JOB_COMMENT 1
#define LEAVE_JOB_COMMENT 0
int schd_run_job_on(Job *job, Queue *queue, char *exechost, int set_comment);
int schd_charge_job(Job *job, Queue *queue, Resources *rsrcs);

/* schedinit.c */
int schedinit(int argc, char **argv);

/* sec2val.c */
char *schd_sec2val(int seconds);

/* time_limits.c */
int schd_primetime_limits(Job *job, Queue *queue, time_t when, char *reason);
int schd_finish_before_np(Job *job, Queue *queue, time_t when, char *reason);


/* user_limits.c */
int schd_user_limits(Job *job, Queue *queue, char *reason);

/* usersort.c */
Job *schd_sort_jobs(Job *jobs);
int schd_save_decay(void);
void schd_decay_info(char *mode);
void schd_update_resource_usage(Job *job);

/* val2bool.c */
int schd_val2bool(char *val, int *bool);
char *schd_bool2val(int bool);
int schd_val2datetime(char *string, time_t *when);
int schd_val2booltime(char *val, time_t *t);
char *schd_booltime2val(time_t bool);


/* val2byte.c */
size_t schd_val2byte(char *val);

/* val2sec.c */
time_t schd_val2sec(char *val);

#endif /* ! TOOLKIT_H_ */
