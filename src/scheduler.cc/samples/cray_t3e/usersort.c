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
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pbs_error.h"
#include "pbs_ifl.h"
#include "log.h"
#include "toolkit.h"
#include "gblxvars.h"

/*
 * Conditional compilation flags:
 *
 * USERSORT_DEBUG:      Enable additional debugging of queue and job structs.
 */

/* #define USERSORT_DEBUG */

extern int connector;
int     schd_NeedToGetDecayInfo;

/* User information. */

struct Uinfo
  {
  char name[MAX_TXT+1];
  int jobcount;
  int running_jobs;
  double nodehours;
  };

/* Decayed usage statistics. */

struct past_usage
  {
  char user[MAX_TXT+1];
  double usage;
  };

static time_t when_decay_saved;
static time_t when_decay_done;
static int    debug = 1;
static int    n_Resource_usage;

static struct past_usage *Resource_usage;

/* Per-user usage information. */

static struct Uinfo *Users = NULL;
static int    nUsers;

/* Per-class job lists.  Point to a set of jobs in the input list. */
static Job **runningJobs = NULL;
static int nJQs, nJRs;
static Job **normalQ     = NULL;
static int nnormalQ;
static Job **dedtimeQ    = NULL;
static int ndedtimeQ;
static Job **otherQ      = NULL;
static int notherQ;

static char *unknown = "???";

static int split_jobs(Job *jobs);
static void get_users(void);
static int make_uinfo(char *user, struct Uinfo *info);
static Job *make_job_list(void);
static int is_new_user(char *user, struct Uinfo *list, int len, int *which);
static void sort_users(void);
static int is_outstanding(Job *job);
static int compare_users(const void *e1, const void *e2);
static int compare_running(const void *e1, const void *e2);
static char *make_grp_usr_tuple(Job *job);
static void sort_jobs(void);

#ifdef USERSORT_DEBUG
static int print_jobs(Job *joblist);
#endif /* USERSORT_DEBUG */

/*
 * Take a list of jobs and sort them based on the priority field.
 * Return a pointer to a new list of jobs in the order in which
 * they should be run.
 */
Job *
schd_sort_jobs(Job *jobs)
  {
  Job *newjobs = NULL;

  if (split_jobs(jobs)) /* split jobs into sublists */
    return (NULL);

  /*
   * Create a list of the users with jobs queued.
   */
  get_users();

  /*
   * Sort user list in order of ascending past-usage.
   */
  sort_users();

  /*
   * Sort the jobs in runningQ, normalQ, and backgrdQ.
   */
  sort_jobs();

  /*
   * Convert arrays of jobs into a linked list.
   */
  newjobs = make_job_list();

#ifdef USERSORT_DEBUG
  /* Print the newly-created ordered job list. */
  print_jobs(newjobs);

#endif /* USERSORT_DEBUG */

  return (newjobs);
  }

/*
 * Determine if it is necessary to save the past usage information
 * to disk.
 */
int
schd_save_decay(void)
  {
  if (schd_TimeNow >= (when_decay_saved + 3600))
    return 1;

  return 0;
  }

/*
 * Read or write the past usage data to the disk file.
 */
void
schd_decay_info(char *mode)
  {
  char   *id = "schd_decay_info";
  FILE   *decay;
  char    buffer[MAX_TXT + 1 + 1]; /* size = MAX_TXT + a newline + a NULL */
  char    gname[50];
  char   *group_ptr;

  struct  past_usage *pusage_ptr;
  int     next = 0;
  int     last = -1;
  size_t  temp = 0;
  int     do_decay = 0;
  int     i;

  /*
   * If there is no pre-existing file, bootstrap it with a single record
   * for root:root.
   */

  if ((decay = fopen(DECAY_INFO_FILE, mode)) == NULL)
    {
    sprintf(log_buffer, "fopen(%s,%s) failed (%d)",
            DECAY_INFO_FILE, mode, errno);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    if ((decay = fopen(DECAY_INFO_FILE, "w")) == NULL)
      {
      /*
       * Could not open the file for writing.  Possibly the path is
       * invalid?
       */
      sprintf(log_buffer, "fopen(%s,%s) failed (%d)",
              DECAY_INFO_FILE, mode, errno);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

      return;
      }

    /*
     * The file pointed to by DECAY_INFO_FILE is writable.  Create the
     * bootstrap record.
     */

    strftime(buffer, sizeof buffer, "%a\n", &schd_TmNow);

    fputs(buffer, decay);

    fprintf(decay, "root:root 1.0\n");

    fclose(decay);

    sprintf(log_buffer, "wrote bootstrap record");

    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    /*
     * Attempt to reopen the file.  If it fails, then the bootstrap
     * failed as well.  This leaves the open stream pointer in 'decay'
     * as if it were opened successfully above.
     */
    if ((decay = fopen(DECAY_INFO_FILE, mode)) == NULL)
      {
      sprintf(log_buffer, "bootstrap:fopen(%s,%s) failed",
              DECAY_INFO_FILE, mode);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      return;
      }
    }

  /* Read or write based on mode. */
  if (mode[0] == 'r')
    {

    pusage_ptr = Resource_usage;
    pusage_ptr = realloc(pusage_ptr, ALLOC_1ST * sizeof * pusage_ptr);

    if (pusage_ptr == NULL)
      {
      temp = ALLOC_1ST * (sizeof * pusage_ptr);
      sprintf(log_buffer, "realloc(%ld) failed", temp);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      return;
      }

    last = ALLOC_1ST - 1;

    /* XXX No error checking. */
    fgets(buffer, sizeof buffer, decay); /* skip day_of_week */

    while (fgets(buffer, sizeof buffer, decay))
      {
      if (next > last)
        {
        last += ALLOC_INC;
        pusage_ptr = realloc(pusage_ptr,
                             (last + 1) * sizeof * pusage_ptr);

        if (pusage_ptr == NULL)
          {
          temp = last * (sizeof * pusage_ptr);
          sprintf(log_buffer, "realloc(%ld) failed", temp);
          log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                     id, log_buffer);
          return;
          }
        }

      /* XXX No error checking. */
      strcpy(pusage_ptr[next].user, strtok(buffer, " \n"));

      pusage_ptr[next].usage = atof(strtok(NULL, " \n"));

      if (debug)
        {
        sprintf(log_buffer, "Read: %-8s\t%.02f", pusage_ptr[next].user,
                pusage_ptr[next].usage);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                   id, log_buffer);
        }

      ++next;
      }

    fclose(decay);

    pusage_ptr = realloc(pusage_ptr, next * sizeof * pusage_ptr);

    if (pusage_ptr == NULL)
      {
      temp = next * (sizeof * pusage_ptr);
      sprintf(log_buffer, "realloc(%ld) failed", temp);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      return;
      }

    Resource_usage = pusage_ptr;

    n_Resource_usage = next;

    schd_NeedToGetDecayInfo = 0;
    when_decay_done = schd_TimeNow;

    }
  else if (mode[0] == 'w')
    {

    /* XXX 23 hours hardcoded */
    do_decay = schd_TimeNow > when_decay_done + 82800;

    strftime(buffer, sizeof buffer, "%a\n", &schd_TmNow);
    fputs(buffer, decay);

    pusage_ptr = Resource_usage;

    for (i = 0; i < n_Resource_usage; ++i)
      {

      if (do_decay)
        {
        /* extract group id from group:user tuple */
        strcpy(gname, pusage_ptr->user);
        group_ptr = strtok(gname, ":");

        if (schd_ENFORCE_ALLOCATION &&
            schd_TimeNow >= schd_ENFORCE_ALLOCATION &&
            schd_is_over_alloc(group_ptr))
          {
          pusage_ptr->usage = pusage_ptr->usage * schd_OA_DECAY_FACTOR;
          }
        else
          {
          pusage_ptr->usage = pusage_ptr->usage * schd_DECAY_FACTOR;
          }
        }

      if (pusage_ptr->usage > 0)
        {
        fprintf(decay, "%s %0.3f\n",
                pusage_ptr->user, pusage_ptr->usage);

        if (debug)
          {
          sprintf(log_buffer, "Wrote: %-8s\t%0.3f",
                  pusage_ptr->user, pusage_ptr->usage);
          log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                     id, log_buffer);
          }
        }

      ++pusage_ptr;
      }

    fclose(decay);

    if (do_decay)
      when_decay_done = schd_TimeNow;

    when_decay_saved = schd_TimeNow;

    }
  else
    {
    sprintf(log_buffer, "unknown mode [%s]", mode);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

  return;
  }

/*
 * Split the supplied list of Job's into separate arrays based on various
 * characteristics of the jobs (queued vs. running, waiting, special, etc)
 */
static int
split_jobs(Job *jobs)
  {
  char   *id = "split_jobs";
  Job    *this;
  QueueList *qptr;
  int   dedtimeI, normalI, otherI, runningI;

  /* Number of elements in the queue arrays. */
  nJRs = nJQs = 0;
  ndedtimeQ = nnormalQ = notherQ = 0;

  runningJobs = NULL;
  dedtimeQ = NULL;
  normalQ  = NULL;
  otherQ   = NULL;

  /*
   * Count the number of jobs that belong to each class, and allocate
   * an array of pointers to Job's for each non-empty class.  Note that
   * only jobs that are either 'R'unning or 'Q'ueued are of interest.
   * Place "other" jobs on the "other" list for completeness.
   */

  for (this = jobs; this != NULL; this = this->next)
    {
    if (this->state == 'R')
      {
      nJRs ++;
      continue;
      }

    if (this->state == 'Q')
      {
      nJQs ++;

      /* Does this job belong on the dedicated list? */

      if ((schd_DedQueues != NULL))
        {
        for (qptr = schd_DedQueues; qptr != NULL; qptr = qptr->next)
          {
          if (!strcmp(this->qname, qptr->queue->qname))
            {
            ndedtimeQ ++;
            break;
            }
          }

        if (qptr != NULL)   /* Match found with dedicated queue. */
          {
          this->flags |= JFLAGS_DEDICATED;
          continue;
          }
        }

      /* Does this job belong on the special queue list? */
      if ((schd_SpecialQueue != NULL) &&
          (!strcmp(this->qname, schd_SpecialQueue)))
        {
        this->flags |= (JFLAGS_WAITING | JFLAGS_PRIORITY);

        nnormalQ ++;
        continue;
        }

      /* Is the job in an outstanding condition? */
      if (is_outstanding(this))
        {
        this->flags |= JFLAGS_WAITING; /* Note the waiting condition. */
        nnormalQ ++;
        continue;
        }

      if (this->priority < 0)   /* a Background Job */
        {
        nnormalQ ++;
        continue;
        }

      /* Anything else must be a normal job */

      nnormalQ ++;

      continue;
      }

    /* Some other state.  Keep track of it so the memory isn't lost. */
    notherQ ++;

    continue;
    }

  /* No running or queued jobs.  that's odd. */
  if (!nJRs && !nJQs)
    return (-1);

  /*
   * Now allocate arrays of pointers large enough to hold a pointer to
   * each job in the class.  These arrays will be sorted, and the job
   * lists reordered to match the sorted values.
   */
  if (nJRs)
    {
    if ((runningJobs = (Job **)malloc(nJRs * sizeof(Job *))) == NULL)
      {
      DBPRT(("%s: malloc failed for %d Job *'s (%s)\n", id,
             nJRs, "runningJobs"));
      goto malloc_failed;
      }
    }

  if (ndedtimeQ)
    {
    if ((dedtimeQ = (Job **)malloc(ndedtimeQ * sizeof(Job *))) == NULL)
      {
      DBPRT(("%s: malloc failed for %d Job *'s (%s)\n", id,
             ndedtimeQ, "dedtimeQ"));
      goto malloc_failed;
      }
    }

  if (nnormalQ)
    {
    if ((normalQ = (Job **)malloc(nnormalQ * sizeof(Job *))) == NULL)
      {
      DBPRT(("%s: malloc failed for %d Job *'s (%s)\n", id,
             nnormalQ, "normalQ"));
      goto malloc_failed;
      }
    }

  if (notherQ)
    {
    if ((otherQ = (Job **)malloc(notherQ * sizeof(Job *))) == NULL)
      {
      DBPRT(("%s: malloc failed for %d Job *'s (%s)\n", id,
             notherQ, "otherQ"));
      goto malloc_failed;
      }
    }

  /*
   * Populate the arrays of pointers with the jobs
   */
  dedtimeI = normalI = otherI = runningI = 0;

  for (this = jobs; this != NULL; this = this->next)
    {
    if (this->state != 'R' && this->state != 'Q')
      {
      otherQ[otherI++] = this;
      continue;
      }

    if (this->state == 'R')
      {
      runningJobs[runningI++] = this;
      continue;
      }

    /* Does this job belong on the dedicated list? */
    if (this->flags & JFLAGS_DEDICATED)
      {
      dedtimeQ[dedtimeI++] = this;
      continue;
      }

    /* every other kind of job. */
    normalQ[normalI++] = this;
    }

  (void)sprintf(log_buffer,
                "run:%d norm:%d ded:%d othr: %d total:%d",
                runningI, normalI, dedtimeI, otherI,
                runningI + normalI + dedtimeI + otherI);
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
  DBPRT(("%s: %s\n", id, log_buffer));

  return (0);

malloc_failed:

  if (dedtimeQ)
    {
    free(dedtimeQ);
    dedtimeQ = NULL;
    }

  if (normalQ)
    {
    free(normalQ);
    normalQ  = NULL;
    }

  if (otherQ)
    {
    free(otherQ);
    otherQ   = NULL;
    }

  nJRs = nJQs = 0;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, "malloc failed");
  return (-1);
  }

/*
 * Determine whether a queued job has waited so long that extra emphasis
 * should be placed on running this job.
 */
static int
is_outstanding(Job *job)
  {
  /*
   * If it is currently primetime, consider an interactive job "waiting
   * for a long time" if it has waited for more time than some constant
   * plus the time it requested.
   *
   * A batch job can wait for much longer than an interactive job.  Given
   * the ability to submit an interactive job, assume that if the user was
   * actually waiting for the job to start, they would submit it as inter-
   * active.
   *
   * The lower-priority "background" jobs are not eligable for "long waiting
   * assistance".
   */

  if (!strcmp(job->oqueue, schd_BackgroundQueue))
    return(0);

  if (schd_INTERACTIVE_LONG_WAIT && job->flags & JFLAGS_INTERACTIVE)
    {
    if (job->eligible > (job->walltime + schd_INTERACTIVE_LONG_WAIT))
      if (schd_prime_time(0))
        return (1);
    }
  else
    {
    if (schd_MAX_QUEUED_TIME && (job->eligible > schd_MAX_QUEUED_TIME))
      return 1;
    }

#ifdef JAMES_DEBUG
  if (job->eligible > Min_Queued_Time)
    return job->walltime < schd_SMALL_QUEUED_TIME;

#endif /* JAMES_DEBUG */

  return 0;
  }

/*
 * Construct a list of users who own one or more "normal" jobs, and count
 * the number of jobs they own.
 */
static void
get_users(void)
  {
  char   *id = "get_users";
  Job    *job_ptr;
  char   *uname;
  char   *name;
  int     which;
  int     i;
  int     j;

  /*
   * Destroy any previously created list.
   */

  if (Users)
    free(Users);

  Users  = NULL;

  nUsers = 0;

  /*
   * Walk the list of "normal" jobs, creating group/owner tuples for each
   * new user.
   */
  for (i = 0; i < nnormalQ; ++i)
    {

    job_ptr = normalQ[i];
    name = make_grp_usr_tuple(job_ptr);

    /* Is this a new entry in the list? */

    if (is_new_user(name, Users, nUsers, &which))
      {

      ++nUsers;

      Users = realloc(Users, nUsers * sizeof * Users);

      if (!Users)
        {
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                   id, "realloc(Users)");
        return;
        }

      make_uinfo(name, &(Users[nUsers - 1]));
      }
    else
      {
      Users[which].jobcount++;
      }
    }

  /* Now, walk the list of running jobs and record each user's count. */
  for (i = 0; i < nUsers; ++i)
    {
    uname = Users[i].name;

    for (j = 0; j < nJRs; ++j)
      {
      job_ptr = runningJobs[j];
      /* Create the name tuple for this user. */
      name = make_grp_usr_tuple(job_ptr);

      if (!strcmp(name, uname))
        Users[i].running_jobs++;

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

  for (i = 0; i < len; ++i)
    {
    if (!strcmp(list_ptr->name, user))
      {
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
  uinfo->jobcount = 1;
  uinfo->running_jobs = 0;
  uinfo->nodehours = get_resource_usage(user);

  return 0;
  }

/*
 * Retrieve this user's info from the past usage database.  If 'user' is
 * not found, create and install an appropriate entry in the usage database.
 */
double
get_resource_usage(char *user)
  {
  char   *id = "get_resource_usage";

  struct past_usage *pusage_ptr;
  int     i;

  /* Search for 'user' in the existing database.  */
  pusage_ptr = Resource_usage;

  for (i = 0; i < n_Resource_usage; ++i)
    {
    if (!strcmp(pusage_ptr->user, user))
      return (pusage_ptr->usage);

    ++pusage_ptr;
    }

  /* No pre-existing entry.  Create a new one. */
  ++n_Resource_usage;

  pusage_ptr = realloc(Resource_usage, n_Resource_usage * sizeof * pusage_ptr);

  if (!pusage_ptr)
    {
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
               id, "realloc(Resource_usage)");
    return -1;
    }

  Resource_usage = pusage_ptr;

  pusage_ptr += n_Resource_usage - 1;
  strcpy(pusage_ptr->user, user);
  pusage_ptr->usage = 0.0;

  return 0;
  }

/*
 * Update past-usage database to account for a new job being run.
 */
void
schd_update_resource_usage(Job *job)
  {
  char   *id = "schd_update_resource_usage";
  int     i;
  char   *name;
  double  node_hours;

  /* create the name tuple for this guy */
  name = make_grp_usr_tuple(job);

  /* We want to evaluate a new past-usage charge rate.
   * instead of use this old equation:
   *
  node_hours = (job->walltime / 3600.0) * job->nodes;
   *
   * lets just have a flat charge per job. but lets not
   * charge anything for very short jobs.
   */

  if (job->walltime > (20*60)) /* greater than 20 minutes */
    node_hours = 10.0;
  else
    node_hours = 1.0;

  /*
   * First, update the recent past-usage database.  Find the appropriate
   * user entry, and add the job's expected usage to that user's total.
   */
  for (i = 0; i < n_Resource_usage; ++i)
    {
    if (strcmp(Resource_usage[i].user, name))
      continue;

    Resource_usage[i].usage += node_hours;

    if (debug)
      {
      sprintf(log_buffer, "%s has %f node-hour recent usage",
              Resource_usage[i].user, Resource_usage[i].usage);
      log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_REQUEST, id, log_buffer);
      }

    break;
    }

  /*
   * Next, update the FY-to-date usage database.  Same as above, but
   * operate on the per-group database.
   */

  for (i = 0; i < schd_NumAllocation; i++)
    {
    if (strcmp(schd_GroupTable[i].gname, job->group))
      continue;

    schd_GroupTable[i].total_usage += node_hours;

    break;
    }

  /*
   * Finally, update the user's running-job count.
   */
  for (i = 0; i < nUsers; ++i)
    {
    if (strcmp(Users[i].name, name))
      continue;

    Users[i].running_jobs++;

    if (debug)
      {
      sprintf(log_buffer, "%s has %d running jobs",
              Users[i].name, Users[i].running_jobs);
      log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_REQUEST, id, log_buffer);
      }

    break;
    }

  return;
  }

/*
 * Sort the list of users with jobs in the "normal" list in ascending order
 * by number of jobs queued.
 */
static void
sort_users(void)
  {
  qsort(Users, nUsers, sizeof *Users, compare_users);
  return;
  }

/*
 * qsort() comparison function.  Sort Uinfo records by increasing value of
 * the 'jobcount' fields.
 */
static int
compare_users(const void *e1, const void *e2)
  {

  struct Uinfo *u1 = (struct Uinfo *)e1;

  struct Uinfo *u2 = (struct Uinfo *)e2;
  int     jc1;
  int     jc2;

  jc1 = u1->jobcount;
  jc2 = u2->jobcount;

  return ((jc1 > jc2) ? 1 : ((jc1 < jc2) ? -1 : 0));
  }

/*
 * qsort() function to sort jobs from largest to smallest by priority
 */
static int
priority_ordering(const void *e1, const void *e2)
  {
  Job    *job1 = *(Job **)e1;
  Job    *job2 = *(Job **)e2;

  if (job1->priority > job2->priority)
    return -1;

  if (job1->priority < job2->priority)
    return 1;

  return 0;
  }

/*
 * Sort "normal" jobs by priority.
 */
static void
sort_jobs(void)
  {
  int (*criterion)(const void *, const void *);

  /*
   * Depending upon the time of day, use a different ordering routine.
   * This is where most of the policy is implemented, as the rest of
   * the scheduler code attempts to run jobs in as close to this order
   * as possible, assuming available resources, time, etc.
   */
  criterion = priority_ordering;

  if (schd_ENFORCE_PRIME_TIME && schd_TimeNow >= schd_ENFORCE_PRIME_TIME)
    {
    if (schd_prime_time(0))
      /* for now use the same ordering all the time */
      criterion = priority_ordering;
    }

  /*
   * Sort the jobs based upon the proper criterion
   * for the current time of day.
   */
  qsort(normalQ,  nnormalQ, sizeof  *normalQ, criterion);

  /*
   * Sort the list of running jobs, in ascending order by the expected
   * time to completion.
   */
  qsort(runningJobs, nJRs, sizeof *runningJobs, compare_running);

  return;
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
 *      running, normal, dedicated
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

  /* Walk the dedicated queue jobs and place them on the list. */
  for (i = 0; i < ndedtimeQ; i++)
    jobtail = jobtail->next = dedtimeQ[i];

  /* Place any remaining jobs on the end of the list. */
  for (i = 0; i < notherQ; i++)
    jobtail = jobtail->next = otherQ[i];

  /* Terminate the last element on the list with a NULL next pointer. */
  jobtail->next = NULL;

  /* Free any storage allocated for the lists. */
  if (runningJobs)
    free(runningJobs);

  if (dedtimeQ)
    free(dedtimeQ);

  if (normalQ)
    free(normalQ);

  if (otherQ)
    free(otherQ);

  /* And reset all the values. */
  runningJobs = dedtimeQ = normalQ = otherQ = NULL;

  nJRs = nJQs = ndedtimeQ = nnormalQ = notherQ = 0;

  /*
   * The first element on joblist is the pointer to the list_seed.  It's
   * next pointer points to the head of the real list - return that.
   */
  return (joblist->next);
  }

static char *
make_grp_usr_tuple(Job *job)
  {
  static char tuple[PBS_MAXUSER + MAX_GROUP_NAME_SIZE + 1 + 1];

  strncpy(tuple, job->group ? job->group : unknown, MAX_GROUP_NAME_SIZE);
  strcat(tuple, ":");
  strncat(tuple, job->owner ? job->owner : unknown, PBS_MAXUSER);

  return tuple;
  }

#ifdef USERSORT_DEBUG
static int
print_jobs(Job *joblist)
  {
  char   *id = "pntJ";
  Job    *job;

  if (joblist)
    {
    log_record(PBSEVENT_SCHED, PBS_EVENTCLASS_SERVER, id,
               "Sorted/Ordered Job List:");

    for (job = joblist; job != NULL; job = job->next)
      {
      sprintf(log_buffer, "%s %c owner=%s\tnodes=%d q=%s",
              job->jobid, job->state, job->owner, job->nodes, job->qname);
      log_record(PBSEVENT_SCHED, PBS_EVENTCLASS_SERVER, id, log_buffer);
      }
    }

  return 0;
  }

#endif /* USERSORT_DEBUG */
