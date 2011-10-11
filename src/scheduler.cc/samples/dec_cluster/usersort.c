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
#include <sys/types.h>
#include <sys/stat.h>

#include "toolkit.h"
#include "gblxvars.h"

#include "pbs_error.h"
#include "log.h"

extern int connector;

/* Per-user usage information. */

struct Uinfo *Users = NULL;
int nUsers;

/* Per-class job lists.  Point to a set of jobs in the input list. */
static Job **normalQ     = NULL;
static Job **otherQ      = NULL;
static Job **runningJobs = NULL;
static Job  *make_job_list(void);
static void  sort_jobs(void);
static int   split_jobs(Job *jobs);
static int   compare_prime_batch(const void *e1, const void *e2);
static int   compare_nonprime_batch(const void *e1, const void *e2);
static int   compare_running(const void *e1, const void *e2);
static int   compare_by_priority(const void *e1, const void *e2);
static int   nnormalQ, notherQ, nJQs, nJRs;
void   set_priorities(void);

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

  /* Create the list of all users with queued and running jobs */
  get_users();

  /* Assign jobs a priority value based on their originating queue */
  set_priorities();

  /* Sort jobs in normalQ. */
  sort_jobs();

  /* Convert arrays of jobs into a linked list. */
  newjobs = make_job_list();

  return (newjobs);
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
      nnormalQ ++;
      continue;
      }

    /* Some other state. Keep track of it so the memory isn't lost */
    notherQ++;

    continue;
    }

  if (!nJRs && !nJQs)
    {
    DBPRT(("%s: found no jobs in either state 'R' or 'Q'!\n", id));
    return (-1);
    }

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
   * Populate the arrays of pointers with
   */
  normalI = runningI = otherI = 0;

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

    normalQ[normalI++] = this;
    }

  (void)sprintf(log_buffer,
                "running:%d queued:%d other:%d total:%d",
                runningI,  normalI,  otherI, runningI + normalI + otherI);
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
  DBPRT(("%s: %s\n", id, log_buffer));

  return (0);

malloc_failed:

  if (normalQ)
    {
    free(normalQ);
    normalQ      = NULL;
    }

  if (otherQ)
    {
    free(otherQ);
    otherQ       = NULL;
    }

  if (runningJobs)
    {
    free(runningJobs);
    runningJobs  = NULL;
    }

  nJRs = nJQs = 0;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, "malloc failed");
  return (-1);
  }

int get_queue_priority(char *qname)
  {
  int i;
  int local_errno = 0;
  char *id = "queue_priority";
  QueueList *qptr;
  Batch_Status *bs;
  AttrList *attr;
  static AttrList alist[] = {{NULL, ATTR_p, "", ""}};

  /* First let's check the global array of priority value in the hope that
   * we have already gone to the trouble to look this queue up before.
   */

  for (i = 0; i < MAX_PRIORITIES; ++i)
    {
    if (QprioritiesArray[i].priority == UNSPECIFIED)
      break;
    else
      {
      if (!strcmp(qname, QprioritiesArray[i].qname))
        return (QprioritiesArray[i].priority);
      }
    }

  /* Hummm, if we got here, then we didn't find an entry for the requested
   * queue. So lets check the global Q lists...
   */

  if (!strcmp(qname, schd_SubmitQueue->queue->qname))
    {
    QprioritiesArray[i].priority = schd_SubmitQueue->queue->priority;
    strcpy(QprioritiesArray[i].qname, schd_SubmitQueue->queue->qname);
    return (QprioritiesArray[i].priority);
    }

  for (qptr = schd_BatchQueues; qptr != NULL; qptr = qptr->next)
    {
    if (!strcmp(qname, qptr->queue->qname))
      {
      QprioritiesArray[i].priority = qptr->queue->priority;
      strcpy(QprioritiesArray[i].qname, qptr->queue->qname);
      return (QprioritiesArray[i].priority);
      }
    }

  /* Okay, so if we got here, the queue that we're hunting for is unknown
   * to the scheduler. So lets ask the PBS server about it.
   */

  if ((bs = pbs_statque_err(connector, qname, alist, NULL, &local_errno)) == NULL)
    {
    sprintf(log_buffer, "pbs_statque failed, \"%s\" %d",
            qname, local_errno);
    log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    return (-1);
    }

  /* Process the list of attributes returned by the server. */

  for (attr = bs->attribs; attr != NULL; attr = attr->next)
    {
    if (!strcmp(attr->name, ATTR_p))
      {
      QprioritiesArray[i].priority = atoi(attr->value);
      strcpy(QprioritiesArray[i].qname, qname);
      pbs_statfree(bs);
      return (QprioritiesArray[i].priority);
      }
    }

  pbs_statfree(bs);

  /* otherwise, return an error */
  sprintf(log_buffer, "Unable to get priority for %s", qname);
  log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
  return(-1);
  }

void set_priorities(void)
  {
  int i;
  Job *job_ptr;

  for (i = 0; i < nnormalQ; ++i)
    {
    job_ptr = normalQ[i];
    job_ptr->priority = get_queue_priority(job_ptr->oqueue);
    }
  }

void sort_jobs(void)
  {
  int (*criterion)(const void *, const void *);

  criterion = compare_nonprime_batch;

  if (schd_ENFORCE_PRIME_TIME && schd_TimeNow >= schd_ENFORCE_PRIME_TIME)
    {
    if (schd_prime_time(0))
      criterion = compare_prime_batch;
    }

  criterion = compare_by_priority;

  /*
   * Sort the list of "normal" jobs, based upon the proper criterion
   * for the current time of day.
   */
  qsort(normalQ, nnormalQ, sizeof *normalQ, criterion);

  /*
   * Sort the list of running jobs, in ascending order by the expected
   * time to completion.
   */
  qsort(runningJobs, nJRs, sizeof *runningJobs, compare_running);

  return;
  }

/*
 * qsort() function to order jobs during primetime.
 *
 * Order shortest jobs first (ascending order of walltime requested)
 * Break ties by sorting in ascending order of requested number of CPUs.
 */
static int
compare_prime_batch(const void *e1, const void *e2)
  {
  Job    *job1 = *(Job **)e1;
  Job    *job2 = *(Job **)e2;

  /* Shortest to Longest */

  if (job1->walltime > job2->walltime)
    return 1;

  if (job1->walltime < job2->walltime)
    return -1;

  /* Largest to Smallest */
  if (job1->ncpus > job2->ncpus)
    return -1;

  if (job1->ncpus < job2->ncpus)
    return 1;

  /* Oldest to youngest. */
  if (job1->eligible > job2->eligible)
    return -1;

  if (job1->eligible < job2->eligible)
    return 1;

  return 0;
  }

/*
 * qsort() function to order jobs during non-primetime.
 *
 * Order largest jobs first (descending order of CPUs requested)
 * Break ties by sorting in ascending order of walltime requested.
 */
static int
compare_nonprime_batch(const void *e1, const void *e2)
  {
  Job    *job1 = *(Job **)e1;
  Job    *job2 = *(Job **)e2;

  /* Largest to Smallest */

  if (job1->ncpus > job2->ncpus)
    return -1;

  if (job1->ncpus < job2->ncpus)
    return 1;

  /* Shortest to Longest */
  if (job1->walltime > job2->walltime)
    return 1;

  if (job1->walltime < job2->walltime)
    return -1;

  /* Oldest to youngest. */
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
 * qsort() function to order jobs by priority.
 *
 * Order queued jobs by descending priority, breaking ties by sorting by
 * requested walltime from shortest to longest. Jobs with the same priority
 * end up in order of increasing waltime requests.
 */

static int
compare_by_priority(const void *e1, const void *e2)
  {
  Job    *job1 = *(Job **)e1;
  Job    *job2 = *(Job **)e2;


  /* descending by priority */

  if (job1->priority > job2->priority)
    return -1;

  if (job1->priority < job2->priority)
    return 1;


  /* ascending walltime */
  if (job1->walltime > job2->walltime)
    return 1;

  if (job1->walltime < job2->walltime)
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
  uinfo->running_jobs   = 0;
  uinfo->remaining_time = 0;
  uinfo->ncpus_used     = 0;
  uinfo->mem_used       = 0;

  return 0;
  }

/*
 * Construct a list of users who own one or more "normal" jobs
 */
void get_users(void)
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
   * Walk the list of "normal" jobs, adding any new users along the way.
   */
  for (i = 0; i < nnormalQ; ++i)
    {

    job_ptr = normalQ[i];
    name = job_ptr->owner;

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
    }

  /* Now, walk the list of running jobs and record each user's count. */
  for (i = 0; i < nUsers; ++i)
    {
    uname = Users[i].name;

    for (j = 0; j < nJRs; ++j)
      {
      job_ptr = runningJobs[j];
      name = job_ptr->owner;

      if (!strcmp(name, uname))
        {
        Users[i].running_jobs   ++;
        Users[i].remaining_time += job_ptr->time_left;
        Users[i].ncpus_used     += job_ptr->ncpus;
        Users[i].mem_used       += job_ptr->memory;
        }

      ++job_ptr;
      }
    }

  return;
  }
