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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "pbs_ifl.h"
#include "log.h"
#include "net_connect.h"
#include "toolkit.h"
#include "gblxvars.h"

extern int connector;
static int bump_rsrc_requests(Job *job, int ncpus, size_t mem);

/*
 * This function takes a pointer to a struct batch_status for a job, and
 * fills in the appropriate fields of the supplied job struct.  It returns
 * the number of items that were found.
 */
int
schd_get_jobinfo(Batch_Status *bs, Job *job)
  {
  char *id = "schd_get_jobinfo";
  int     changed = 0;
  int     cpu_req = 0;
  size_t  mem_req = 0;
  char     *host;
  char *p, *tmp_p, *var_p;
  AttrList *attr;
  char      canon[PBS_MAXHOSTNAME + 1];
  int     istrue;

  memset((void *)job, 0, sizeof(Job));

  job->jobid = schd_strdup(bs->name);

  if (job->jobid == NULL)
    {
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
               "schd_strdup(bs->name)");
    return (-1);
    }

  changed ++;

  for (attr = bs->attribs; attr != NULL; attr = attr->next)
    {

    /*
     * If this is the 'owner' field, chop it into 'owner' and 'host'
     * fields, and copy them into the Job struct.
     */
    if (!strcmp(attr->name, ATTR_owner))
      {

      /* Look for the '@' that separates user and hostname. */
      host = strchr(attr->value, '@');

      if (host)
        {
        *host = '\0'; /* Replace '@' with NULL (ends username). */
        host ++; /* Move to first character of hostname. */
        }

      job->owner = schd_strdup(attr->value);

      if (job->owner == NULL)
        {
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   "schd_strdup(job->owner)");
        return (-1);
        }

      changed ++;

      job->host  = schd_strdup(host);

      if (job->host == NULL)
        {
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   "schd_strdup(job->host)");
        return (-1);
        }

      changed ++;

      /*
       * We don't "own" the attribute strings, so put back the '@'
       * character we removed above, in case something else expects
       * it to be there.
       * Note that 'host' points to the first character of the host-
       * name, not the hole one character behind.
       */

      if (host)
        {
        host --; /* Step back one character. */
        *host = '@'; /* Replace the '@' that was deleted above. */
        }

      /* That's all for the owner field. */
      continue;
      }

    /* The group to which to charge the resources for this job. */
    if (!strcmp(attr->name, ATTR_egroup))
      {
      job->group = schd_strdup(attr->value);

      if (job->group == NULL)
        {
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   "schd_strdup(job->group)");
        return (-1);
        }

      changed ++;

      continue;
      }

    /* The comment currently assigned to this job. */
    if (!strcmp(attr->name, ATTR_comment))
      {
      job->comment = schd_strdup(attr->value);

      if (job->comment == NULL)
        {
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   "schd_strdup(job->comment)");
        return (-1);
        }

      changed ++;

      continue;
      }

    /* The host on which this job is running. */
    if (!strcmp(attr->name, ATTR_exechost))
      {
      job->exechost = schd_strdup(attr->value);

      if (job->exechost == NULL)
        {
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   "schd_strdup(job->exechost)");
        return (-1);
        }

      changed ++;

      continue;
      }

    if (!strcmp(attr->name, ATTR_inter))
      {
      /* Is this job interactive or not? */
      if (schd_val2bool(attr->value, &istrue) == 0)
        {
        if (istrue)
          job->flags |= JFLAGS_INTERACTIVE;
        else
          job->flags &= ~JFLAGS_INTERACTIVE;

        changed ++;
        }
      else
        {
        DBPRT(("%s: can't parse %s = %s into boolean\n", id,
               attr->name, attr->value));
        }

      continue;
      }

    if (!strcmp(attr->name, ATTR_state))
      {
      /* State is one of 'R', 'Q', 'E', etc. */
      job->state = attr->value[0];
      changed ++;
      continue;
      }

    if (!strcmp(attr->name, ATTR_queue))
      {
      job->qname = schd_strdup(attr->value);

      if (job->qname == NULL)
        {
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   "schd_strdup(job->qname)");
        return (-1);
        }

      job->flags |= JFLAGS_QNAME_LOCAL;

      changed ++;
      continue;
      }

    if (!strcmp(attr->name, ATTR_v))
      {
      var_p = schd_strdup(attr->value);

      if (var_p == NULL)
        {
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   "schd_strdup(Variable_List)");
        return (-1);
        }

      p = NULL;

      tmp_p = strstr(var_p, "PBS_O_QUEUE");

      if (tmp_p)
        {
        p = strtok(tmp_p, "=");
        p = strtok(NULL,  ", ");
        }

      if (p != NULL)
        {
        job->oqueue = schd_strdup(p);
        }
      else
        {
        /* if the originating queue is unknown, default
         * to the locally defined "submit" queue.
         */
        job->oqueue = schd_strdup(schd_SubmitQueue->queue->qname);
        }

      free(var_p);

      changed ++;
      continue;
      }

    if (!strcmp(attr->name, ATTR_l))
      {
      if (!strcmp(attr->resource, "walltime"))
        {
        job->walltime = schd_val2sec(attr->value);
        changed ++;

        }
      else if (!strcmp(attr->resource, "ncpus"))
        {
        cpu_req = atoi(attr->value);
        job->nodes = MAX(job->nodes, cpu_req);
        changed ++;

        }
      else if (!strcmp(attr->resource, "mppe"))
        {
        cpu_req = atoi(attr->value);
        job->nodes = MAX(job->nodes, cpu_req);
        changed ++;

        }
      else if (!strcmp(attr->resource, "mem"))
        {
        mem_req = schd_val2byte(attr->value);
        job->nodes = MAX(job->nodes, NODES_FROM_MEM(mem_req));
        changed ++;

#if PE_MASK != 0
        }
      else if (!strcmp(attr->resource, "pe_mask"))
        {
        if (schd_str2mask(attr->value, &job->nodemask))
          {
          (void)sprintf(log_buffer,
                        "bad pe_mask %s for job %s", attr->value, job->jobid);
          log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                     log_buffer);
          }
        else
          changed++; /* Job pe_mask was valid. */

#endif /* PE_MASK */
        }

      /* That's all for requested resources. */
      continue;
      }

    if (!strcmp(attr->name, ATTR_used))
      {
      if (!strcmp(attr->resource, "walltime"))
        {
        job->walltime_used = schd_val2sec(attr->value);

        changed ++;
        }

      /* No other interesting cases. */
      continue;
      }

    /* Session ID for running jobs (used to correlate GRM info */
    if (!strcmp(attr->name, ATTR_session))
      {
      job->session = atoi(attr->value);
      continue;
      }

    /* Job Priority attribute (inherited from queue) */
    if (!strcmp(attr->name, ATTR_p))
      {
      job->priority = atoi(attr->value);
      continue;
      }

    /* Creation time attribute. */
    if (!strcmp(attr->name, ATTR_ctime))
      {
      /* How long ago was it put in the queue ? */
      job->time_queued = schd_TimeNow - atoi(attr->value);

      continue;
      }

    /* Modified time attribute. */
    if (!strcmp(attr->name, ATTR_mtime))
      {
      /* When was the job last modified? */
      job->mtime = atoi(attr->value);

      continue;
      }

#ifdef ATTR_etime
    /*
     * When was the job last eligible to run?  When a user-hold is
     * released, this value is updated to the current time.  This
     * prevents users from gaining higher priority from holding their
     * jobs.
     */
    if (!strcmp(attr->name, ATTR_etime))
      {
      job->eligible = schd_TimeNow - atoi(attr->value);

      continue;
      }

#endif /* ATTR_etime */
    }

  /*
   * If this job is in the "Running" state, compute how many seconds
   * remain until it is completed.
   */
  if (job->state == 'R')
    {
    job->time_left = job->walltime - job->walltime_used;
    }

  /*
   * If this job was enqueued since the last time we ran, set the job
   * flag to indicate that we have not yet seen this job.  This makes it
   * a candidate for additional processing.  There may be some inaccuracy,
   * since the time_t has resolution of 1 second.  Attempt to err on the
   * side of caution.
   */
  if ((job->state == 'Q') && (job->time_queued != UNSPECIFIED))
    {
    if (job->time_queued <= (schd_TimeNow - schd_TimeLast))
      {
      job->flags |= JFLAGS_FIRST_SEEN;
      }
    }

  /*
   * If the 'etime' attribute wasn't found, set it to the time the job has
   * been queued.  Most jobs will be eligible to run their entire lifetime.
   * The exception is a job that has been held - if it was a user hold,
   * the release will reset the etime to the latest value.
   * If not eligible time was given, use the job's creation time.
   */
  if (!job->eligible)
    job->eligible = job->time_queued;

#if defined(sgi)
  /*
   * If the job provided a memory or CPU resource that does not match
   * the resources that will be allocated by the assigned nodes (i.e.
   * a request for 100mb of memory and 16 CPUs - the job will "get" all
   * 4GB of memory anyway), alter the job attributes such that they
   * will align with the assigned nodes later.
   */
  bump_rsrc_requests(job, cpu_req, mem_req);

#endif /* defined(sgi) */

  /*
   * Need to update the  time_until_eligible  and  total_delay fields,
   * probably from a global array of information saved from previous
   * scheduler iteration.
   */

  /*
   * Calculate the job priority weight sort key to be used later in
   * job sorting. This is the "priority" the job should have during
   * sorting based on the size of the job, the length of time queued,
   * and the job type.
   */
  calc_job_weight(job);

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

  for (job = list; job != NULL; job = next)
    {
    numjobs ++;
    next = job->next;

    if (job->jobid)  free(job->jobid);

    if (job->owner)  free(job->owner);

    if (job->host)  free(job->host);

    if (job->exechost) free(job->exechost);

    if (job->group)  free(job->group);

    if (job->comment) free(job->comment);

    if (job->oqueue) free(job->oqueue);

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

static int
bump_rsrc_requests(Job *job, int cpu_req, size_t mem_req)
  {
  char   *id = "bump_rsrc_requests";
  char   *val;
  char    buf[64];
  int     bumped = 0;

  /*
   * If a job gives the "wrong" value for the memory request (for the
   * number of nodes required to fulfill the request), then bump the
   * memory request to the amount of memory the assigned nodes would
   * consume.
   */

  if ((mem_req == 0) || (mem_req != (job->nodes * MB_PER_NODE)))
    {

    /* Make a printable version of the requested memory. */
    strcpy(buf, schd_byte2val(mem_req));

    /* Compute the "right" memory request, based on the nodes. */
    val = schd_byte2val(job->nodes * MB_PER_NODE);

    if (val == NULL)
      return (1);

    if (schd_alterjob(connector, job, ATTR_l, val, "mem"))
      {
      DBPRT(("%s: Failed to set job %s \"mem\" attr to %s\n", id,
             job->jobid, val));
      return (1);
      }

    bumped++;
    }

  /*
   * If a job gives the "wrong" value for the CPU request (for the
   * number of nodes required to fulfill the request), then bump the
   * CPU request to the number of CPUs the assigned nodes would
   * consume.
   */
  if ((cpu_req == 0) || (cpu_req != (job->nodes * PE_PER_NODE)))
    {

    /* Compute the "right" memory request, based on the nodes. */
    sprintf(buf, "%d", (job->nodes * PE_PER_NODE));

    if (schd_alterjob(connector, job, ATTR_l, buf, "ncpus"))
      {
      DBPRT(("%s: Failed to set job %s \"ncpus\" attr to %s\n", id,
             job->jobid, buf));
      return (1);
      }

    bumped++;
    }

  if (bumped)
    {
    strncpy(buf, schd_byte2val(job->nodes * MB_PER_NODE), sizeof(buf) - 1);
    sprintf(log_buffer, "%s cpu/mem (%d/%s) bumped to %d/%s (%d nodes)",
            job->jobid, cpu_req, schd_byte2val(mem_req),
            job->nodes * PE_PER_NODE, buf, job->nodes);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    }

  return (0);
  }

/*
 * The job->priority field correspondes to the priority of
 * the queue to which the job was submitted. This value
 * is then used in the following calculation to determine
 * a new priority that takes into consideration the recent
 * past usage of the user, the number of nodes requested,
 * the priority of the queue to which the job was submitted,
 * and the lenght of time the job has been queued.
 *
 * Jobs are initially assigned a priority value based on:
 *
 *     Queue     Initial          Max
 *     Name      Priority Priority
 *     --------  --------       --------
 *     Special     1000      1024
 *     Challenge    900     999
 *     Normal         0            999
 *     Background    <0          -1024
 *
 */

#define SECS_PER_HOUR 3600

void calc_job_weight(Job *job)
  {
  int    timeQd;
  float  usage, weight, A, B, C, D, E, F;
  double priority;

  timeQd = schd_TimeNow - job->eligible;
  usage  = get_resource_usage(job->owner);

  if (!strcmp(job->oqueue, schd_SpecialQueue))
    job->priority = 1000;

  else if (!strcmp(job->oqueue, schd_ChallengeQueue))
    job->priority = 900;

  else if (!strcmp(job->oqueue, schd_BackgroundQueue))
    job->priority = -100;

  else
    job->priority = 0;


  /* these should be set by parameters in the config file     */
  /* These are used to scale the impact of various attributes */
  /*  of the job on the job weight, used in the priority calc */

  A = 4.0;  /* Adjust recent usage                  */

  B = 2.0;  /* Adjust job size (nodes)              */

  C = 1.0 / 7.0; /* Adjust time queued                   */

  /* Use user supplied values for weight of each factor:      */
  /* For simplicty, the user tunable parameters are in the    */
  /* range of 1-10, or 0 (no weight). We scale these into     */
  /* new ranges such that the values proportionately affect   */
  /* the calculated weight of the job.                        */
  /*                                                          */
  /* Recommended input values for a large system are:         */
  /*                                                          */
  /*      schd_USAGE_WEIGHT = 10                              */
  /*      schd_NODES_WEIGHT =  7                              */
  /*      schd_TIMEQ_WEIGHT =  1                              */
  /*                                                          */

  A = schd_USAGE_WEIGHT / 2.50000;      /* scale to range 1-4 */

  B = schd_NODES_WEIGHT / 3.33333;      /* scale to range 1-3 */

  C = schd_TIMEQ_WEIGHT / 1.42857;      /* scale to range 1-7 */

  D = 0.001;   /* Bring baseline into desired range    */

  E = 0.3;  /* Adjust slope of tanh() curve         */

  F = 1.0;  /* Adjust slope of tanh() curve         */


  /* scale the priority (set by the originating queue) such   */
  /* that the tanh() on the baseline produces appropriate     */
  /* values  (eg. with the ranges stated above).              */

  if (job->priority >= 1000) job->priority *=  10;
  else if (job->priority >=  900) job->priority *= 7.8;
  else if (job->priority >=    0) job->priority  = 100;
  else if (job->priority <     0) job->priority *=  -1;

  weight   = -A * usage / schd_MAX_NCPUS      +
             B * job->nodes / schd_MAX_NCPUS +
             C * timeQd / SECS_PER_HOUR      +
             D * job->priority             ;

  priority = 500 * (1 + tanh(E * weight - F));

  if (job->priority < 0)
    job->priority = -1 * (1024 - priority);

  }
