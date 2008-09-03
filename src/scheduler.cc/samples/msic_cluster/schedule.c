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
 * This is the main loop for the scheduler.  The scheduler receives a
 * command and then calls the routine schedule().  Appropriate actions
 * are taken based on the command specified.  All actions taken by the
 * scheduler must be initiated from here.
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

/* Scheduler header files */
#include "toolkit.h"
#include "gblxvars.h"
#include "msgs.h"

/* PBS header files */
#include "pbs_error.h"
#include "log.h"
#include "sched_cmds.h"

extern int connector;
extern char *schd_CmdStr[16];

/*
 * How many seconds to wait before attempting to re-request the job and queue
 * information from the server.
 */
#define WAIT_FOR_QUEUE_SANITY 10

Resources schd_Rsrcs;  /* system resources */

struct tm schd_TmNow;
time_t  schd_TimeNow;
time_t  schd_TimeLast  = 0;
int last_run_in_pt = 0;
int schd_MAX_NCPUS = 0;
Job    *schd_AllJobs = NULL;
char    schd_EXPRESS_Q_NAME[50] = "\0";

static int schd_req(int cmd);
static int schedule_restart(Job *joblist);
static int make_job_dump(char *dumpfile);
static int dump_sorted_jobs(FILE *file, Job *joblist);


/*
 * This routine gets the scheduling command and takes appropriate action.
 * C.f. PBS Administrator's Guide, Section 4.1.
 *
 * Valid commands are:
 *    SCH_ERROR            ERROR.
 *    SCH_SCHEDULE_NULL    Place holder.
 *    SCH_SCHEDULE_NEW     New job arrived or a non-running job changed state.
 *    SCH_SCHEDULE_TERM    A running job terminated.
 *    SCH_SCHEDULE_TIME    The schedulers time interval expired.
 *    SCH_SCHEDULE_RECYC   One job was started in the last scheduling cycle.
 *    SCH_SCHEDULE_CMD     The server attribute "scheduling" was set to true.
 *    SCH_CONFIGURE        Perform any scheduler [re]configuration.
 *    SCH_QUIT             QUIT
 *    SCH_RULESET          Reread the scheduler rules.
 *    SCH_SCHEDULE_FIRST   First schedule run after server starts.
 */
int
schedule(int cmd)
  {
  char   *id = "schedule";
  char   *cmdstr = "Error";

  /*
   * Try to find a text string that describes the command request.
   */

  if (cmd >= 0)
    cmdstr = (schd_CmdStr[cmd] != NULL) ? schd_CmdStr[cmd] : "Unknown";

  (void)sprintf(log_buffer, "Schedule: received cmd %d (%s)", cmd, cmdstr);

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

  DBPRT(("--------\n%s\n", log_buffer));

  /*
   * See if a reconfiguration is pending.  If so, do it before anything
   * starts to look at the structures.
   */
  if (schd_sigflags & SCHD_SIGFLAGS_RECONFIG)
    {
    (void)sprintf(log_buffer, "Reconfiguring due to delivery of SIGHUP.");
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));

    if (schedinit(0, NULL))
      {
      (void)sprintf(log_buffer,
                    "%s: FATAL ERROR!!!  Configuration failed!!!", id);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));

      exit(1);
      }

    /* Turn off the pending reconfiguration flag. */
    schd_sigflags &= ~SCHD_SIGFLAGS_RECONFIG;
    }

  /*
   * Assume the scheduler is about to use the current configuration.
   * Set the BUSY flag so that reconfiguration will be postponed until
   * the scheduler is idle if a SIGHUP is delivered.
   */
  schd_sigflags |= SCHD_SIGFLAGS_BUSY;

  switch (cmd)
    {
      /*
       * "Null" commands.  These cases are quietly ignored.
       */

    case SCH_SCHEDULE_NULL: /* Placeholder only.                        */

    case SCH_ERROR:  /* Error.                                   */

    case SCH_RULESET:  /* Re-read scheduler rules.                 */

    case SCH_SCHEDULE_RECYC: /* Recycle scheduler after 1 run.           */
      (void)sprintf(log_buffer, "command %d ignored.", cmd);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s\n", log_buffer));

      break;

      /*
       * These commands cause the scheduler to perform its scheduling cycle.
       */

    case SCH_SCHEDULE_FIRST: /* 1st schedule run after server starts.    */

    case SCH_SCHEDULE_CMD: /* Perform a schedule run now, on command.  */

    case SCH_SCHEDULE_TERM: /* A running job terminated.                */

    case SCH_SCHEDULE_NEW: /* A new job has arrived.                   */

    case SCH_SCHEDULE_TIME: /* Scheduler sleep interval reached.        */

      if (!schd_req(cmd))
        {
        schd_cleanup();
        DBPRT(("schd_req(%d) returned 0.  Calling it again.\n", cmd));
        schd_req(cmd);
        }

      schd_cleanup();

      break;

    case SCH_CONFIGURE:  /* Re-initialize the scheduler.             */
      (void)sprintf(log_buffer, "[re]configure scheduler");
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s\n", log_buffer));

      schedinit(0, NULL);  /* XXX args not used... for now! */
      break;

    case SCH_QUIT:  /* Exit gracefully.                         */
      (void)sprintf(log_buffer, "Server requested Scheduler to quit.  Exit.");
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s\n", log_buffer));
      exit(0);

    default:
      (void)sprintf(log_buffer, "Schedule command %d unrecognized.", cmd);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s\n", log_buffer));
    }

  /*
   * The scheduling part of the scheduler is now idle.  Unset the BUSY
   * flag.
   */
  schd_sigflags &= ~SCHD_SIGFLAGS_BUSY;

  schd_FirstRun = 0;

  return (0);
  }

/*
 * Service a request to schedule a job.
 */
/* ARGSUSED */
int
schd_req(int cmd)
  {
  char   *id = "schd_req";
  Job    *jobs = NULL;
  int     error, total_ran = 0;

  struct tm *tm_ptr;
  char    reason[MAX_TXT + 1];

  /* Save "last" run time (in global 'schd_TimeNow') for later use. */
  schd_TimeLast = schd_TimeNow;

  /*
   * Get the number of seconds since the Epoch, and break it down into
   * the various day, month, and year fields in a struct tm.
   */
  time(&schd_TimeNow);

  if ((tm_ptr = localtime(&schd_TimeNow)) != NULL)
    memcpy((void *) & schd_TmNow, (void *)tm_ptr, sizeof(struct tm));
  else
    memset((void *)&schd_TmNow, 0, sizeof(struct tm));

  DBPRT(("[time_t %d] %s", schd_TimeNow, ctime(&schd_TimeNow)));

  /*
   * If the configuration file has been changed since the last time the
   * scheduler was run, than note that in the logs.  Don't re-read it
   * automatically, just note the fact.  Don't reset the timestamp - it
   * will be done when someone finally HUP's the scheduler.
   */
  if (schd_CfgFilename && schd_file_has_changed(schd_CfgFilename, 0))
    {
    (void)sprintf(log_buffer,
                  "WARNING!!!  Scheduler config file %s has changed!",
                  schd_CfgFilename);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    (void)sprintf(log_buffer, "Run 'kill -HUP %ld' to reconfigure.",
                  (long)getpid());
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    }

  /* Query the server-defined maximum NCPUS limit -- used later for
   * enforcement of Shares, etc. Only need to do this once per scheduler
   * lifetime, since the shares are tracked across multiple days
   */
  if (schd_MAX_NCPUS == 0)
    schd_MAX_NCPUS = schd_get_max_ncpus();

  /* if we haven't read the Share Usage data yet, then do so */
  if (schd_NeedToGetShareInfo)
    schd_share_info("r");

  /* Pause for a second after getting 'new job arrived' to allow any exiting
   * job to complete -- i.e. jobs that resubmit just prior to exiting will
   * cause us to get "woke up", yet those resources will not have been freed
   * yet.
   */
  sleep(1);

  /*
   * If the site has enabled enforcement of a distiction of prime vs.
   * non-prime time, preform various tasks to setup and support this
   * feature.
   */
  if (schd_ENFORCE_PRIME_TIME)
    schd_check_primetime();

  /* Get the current list of all jobs known to our server. */
  jobs = schd_get_jobs(NULL, NULL);

  if (jobs == NULL)
    {
    (void)sprintf(log_buffer, ">>> No Jobs - Cancel Scheduling Cycle <<<");
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s\n", log_buffer));
    return (1);
    }

  /*
   * At the first run of the scheduler, check for queued jobs on any of
   * the run queues. This may happen if there is some glitch and the
   * POSIX jobs are checkpointed. schedule_restart() will return non-zero
   * if it finds and restarts any jobs. Recycle if this is the case.
   */
  if (cmd == SCH_SCHEDULE_FIRST &&
      schd_SCHED_RESTART_ACTION != SCHD_RESTART_NONE)
    {
    if (schedule_restart(jobs))
      {
      schd_free_jobs(jobs);
      return (0);
      }
    }

  /*
   * Get the queue limits for each queue about which the scheduler
   * knows, from the PBS server. If PBS fails to provide us any
   * information about a queue, treat it as a fatal error. If a
   * queue has failed the sanity checks, wait before retrying.
   */
  error = schd_get_queues();

  if (error < 0)
    {
    DBPRT(("get_queue() failed\n"));
    schd_free_jobs(jobs);
    return (1); /* Bogus queue - don't retry */
    }

  /*
   * Sort the list of jobs based on several criteria. The sorting routine
   * returns a pointer to the new head of the list created by relinking
   * the elements of the linked list, or NULL if an error occurs. Zero
   * the original list pointer to reduce confusion -- the same list, in
   * different order, now lives on schd_AllJobs.
   */
  schd_AllJobs = schd_sort_jobs(jobs);

  jobs = NULL;

  if (schd_AllJobs == NULL)
    {
#ifdef DEBUG
    (void)sprintf(log_buffer,
                  ">>> Sorting Failed - Cancel Scheduling Cycle <<<");
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
#endif
    DBPRT(("%s\n", log_buffer));
    return (1);
    }

  /*
   * Get the queue utilization for each queue. Any jobs on schd_AllJobs
   * (set by sort_jobs() above) that belong to the queue will be placed
   * on the queue->jobs list.
   */
  error = schd_get_queue_util();

  if (error < 0)
    {
    DBPRT(("get_all_queue_info() failed\n"));
    return (1); /* Bogus queue - don't recycle. */
    }
  else if (error > 0)
    {
    DBPRT(("queue failed sanity check - wait and recycle.\n"));
    sleep(WAIT_FOR_QUEUE_SANITY);
    return (0); /* Attempt to recycle scheduler. */
    }

  /* Dump the list of jobs being scheduled from submit queue. */
  if (schd_JOB_DUMPFILE)
    {
    make_job_dump(schd_JOB_DUMPFILE);
    }

  if (schd_SubmitQueue->queue->jobs &&
      !(schd_SubmitQueue->queue->flags & (QFLAGS_DISABLED | QFLAGS_STOPPED)))
    {
    total_ran = schd_pack_queues(schd_SubmitQueue->queue->jobs,
                                 schd_BatchQueues, reason);
    }

  /* DEBUG: for Pat Wright at MSIC: print the resouces used each iteration
   *
      if (total_ran > 0) {
   *
   */
  (void)sprintf(log_buffer, "System resources after scheduling:");

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

  schd_dump_rsrclist();

  /*
   * DEBUG: continuation of above test for Pat Wright
     }
   *
   */

  /* We need to save the Share Usage data peridoically, so that a restart
   * of pbs_sched doesn't loose it.
   */
  if (schd_save_shares())  /* is it time yet? */
    schd_share_info("w");  /* yes, so do it   */

  (void)sprintf(log_buffer, ">>>  End Scheduling Cycle (ran %d jobs)  <<<",
                total_ran);

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

  DBPRT(("%s\n", log_buffer));

  return (1);
  }

static int
schedule_restart(Job *joblist)
  {
  char   *id = "schedule_restart";
  Job    *job, *nextjob;
  QueueList *qptr;
  int     found, changed;

  changed = found = 0;

  for (job = joblist; job != NULL; job = nextjob)
    {
    nextjob = job->next;

    if (job->state != 'Q')
      continue;

    /*
     * See if the job is queued on one of the batch queues.  If not,
     * go on to the next job.
     */
    for (qptr = schd_BatchQueues; qptr != NULL; qptr = qptr->next)
      if (strcmp(qptr->queue->qname, job->qname) == 0)
        break;

    if (qptr == NULL)
      continue;

    found++;

    if (schd_SCHED_RESTART_ACTION == SCHD_RESTART_RERUN)
      {
      (void)sprintf(log_buffer, "Restart job '%s' on queue '%s'.",
                    job->jobid, job->qname);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                 id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));

      schd_comment_job(job, schd_JobMsg[JOB_RESTARTED],
                       JOB_COMMENT_REQUIRED);

      if (schd_run_job_on(job, job->queue, schd_SCHED_HOST,
                          LEAVE_JOB_COMMENT))
        {
        (void)sprintf(log_buffer,
                      "Unable to run job '%s' on queue '%s'.", job->jobid,
                      job->qname);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   log_buffer);
        }
      else
        changed ++;

      }
    else /* (SCHED_RESTART_ACTION == SCHD_RESTART_RESUBMIT) */
      {
      /* Move the job back to its originating queue. */
      if (pbs_movejob(connector, job->jobid, job->oqueue, NULL) != 0)
        {
        (void)sprintf(log_buffer,
                      "failed to move %s to queue %s, %d", job->jobid,
                      job->oqueue, pbs_errno);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   log_buffer);
        DBPRT(("%s: %s\n", id, log_buffer));
        }
      else
        {
        (void)sprintf(log_buffer,
                      "Requeued job '%s' on queue '%s'.", job->jobid,
                      job->oqueue);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,
                   id, log_buffer);
        DBPRT(("%s: %s\n", id, log_buffer));
        schd_comment_job(job, schd_JobMsg[JOB_RESUBMITTED],
                         JOB_COMMENT_REQUIRED);
        changed ++;
        }
      }
    }

  if (found)
    {
    if (schd_SCHED_RESTART_ACTION == SCHD_RESTART_RERUN)
      {
      (void)sprintf(log_buffer,
                    "Re-ran %d jobs (of %d) found queued on run queues.\n",
                    changed, found);
      }
    else
      {
      (void)sprintf(log_buffer,
                    "Moved %d queued jobs (of %d) from run queues back to '%s'.\n",
                    changed, found, schd_SubmitQueue->queue->qname);
      }

    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    DBPRT(("%s: %s\n", id, log_buffer));
    }

  return (changed);
  }

static int
make_job_dump(char *dumpfile)
  {
  char    *id = "make_job_dump";
  FILE    *dump;
  QueueList *qptr;


  (void)sprintf(log_buffer, "Sorted jobs: %s", schd_JOB_DUMPFILE);
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

  /*
   * Attempt to open the dump file, creating it if necessary.  It should
   * be truncated each time this runs, so don't open with append mode.
   */

  if ((dump = fopen(dumpfile, "w")) == NULL)
    {
    (void)sprintf(log_buffer, "Cannot write to %s: %s\n", dumpfile,
                  strerror(errno));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    return (-1);
    }

  /* Head the file with a timestamp. */
  fprintf(dump, "%s\n", ctime(&schd_TimeNow));

  /* Include the version string compiled into the scheduler binary. */
  fprintf(dump, "%s\n", schd_VersionString);

  /* And some more useful information about the state of the world. */
  fprintf(dump, "Scheduler running on '%s'\n", schd_ThisHost);

  fprintf(dump, "Prime-time is ");

  if (schd_ENFORCE_PRIME_TIME)
    {
    fprintf(dump, "from %s ", schd_sec2val(schd_PRIME_TIME_START));
    fprintf(dump, "to %s.\n", schd_sec2val(schd_PRIME_TIME_END));
    }
  else
    fprintf(dump, "not enforced.\n");

  fprintf(dump, "\nJOBS LISTED IN ORDER FROM HIGHEST TO LOWEST PRIORITY\n\n");

  /* Now dump the jobs queued on the various queues, in order of sort_order. */
  qptr = schd_SubmitQueue;

  if (qptr->queue->jobs)
    {
    fprintf(dump, "Jobs on submit queue '%s':\n", qptr->queue->qname);
    dump_sorted_jobs(dump, qptr->queue->jobs);
    }

  if (fclose(dump))
    {
    (void)sprintf(log_buffer, "close(%s): %s\n", dumpfile, strerror(errno));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    return (-1);
    }

  return (0);
  }

static int
dump_sorted_jobs(FILE *dump, Job *joblist)
  {
  Job     *job;
  int      njobs, elig_mesg = 0;

#define DUMP_PRIORITY  6
#define DUMP_JID_LEN  10
#define DUMP_STATE_LEN  1
#define DUMP_QUEUE_LEN  8
#define DUMP_OWNER_LEN  8
#define DUMP_NODES_LEN  3
#define DUMP_WALLT_LEN  8
#define DUMP_ELIGI_LEN  9 /* time plus '*' if wait != eligible */
#define DUMP_FLAGS_LEN  10

  char     jid[DUMP_JID_LEN + 1];
  char     queue[DUMP_QUEUE_LEN + 1];
  char     owner[DUMP_OWNER_LEN + 1];
  char     wallt[DUMP_WALLT_LEN + 1];
  char     eligi[DUMP_ELIGI_LEN + 1];
  char     flags[DUMP_FLAGS_LEN + 1];

  fprintf(dump, " %*s %*s %*s %*s %*s %*s %*s %*s %*s\n",
          -DUMP_PRIORITY,  "Pri",
          -DUMP_JID_LEN,  "Job ID",
          -DUMP_STATE_LEN, "S",
          -DUMP_OWNER_LEN, "Owner",
          -DUMP_QUEUE_LEN, "Queue",
          -DUMP_NODES_LEN, "Nds",
          -DUMP_WALLT_LEN, "Walltime",
          -DUMP_ELIGI_LEN, "Eligible",
          -DUMP_FLAGS_LEN, "Flags");

  fprintf(dump, " %*s %*s %c %*s %*s %*s %*s %*s %*s\n",
          -DUMP_PRIORITY,  "-----",
          -DUMP_JID_LEN,  "---------",
          '-',
          -DUMP_QUEUE_LEN, "--------",
          -DUMP_OWNER_LEN, "--------",
          -DUMP_NODES_LEN, "---",
          -DUMP_WALLT_LEN, "--------",
          -DUMP_ELIGI_LEN, "---------",
          -DUMP_FLAGS_LEN, "------");

  for (njobs = 0, job = joblist; job != NULL; job = job->next)
    {

    njobs++;
    strncpy(jid, job->jobid, DUMP_JID_LEN);
    strncpy(owner, job->owner, DUMP_OWNER_LEN);
    strncpy(queue, job->oqueue, DUMP_QUEUE_LEN);
    strcpy(wallt, schd_sec2val(job->walltime));
    strcpy(eligi, schd_sec2val(job->eligible));

    if (job->time_queued != job->eligible)
      {
      strcat(eligi, "*");
      elig_mesg ++;
      }

    flags[0] = '\0';

    /* Watch length of 'flags[]' array! */

    if (job->flags & JFLAGS_PRIORITY)
      strcat(flags, "High ");

    if (job->flags & JFLAGS_PRIORITY)
      strcat(flags, "Wait ");

    if (job->flags & JFLAGS_RUNLIMIT)
      strcat(flags, "RUNLIM");

    if (job->flags & JFLAGS_CHKPT_OK)
      strcat(flags, "CKPTOK");

    if (job->flags & JFLAGS_CHKPTD)
      strcat(flags, "CKPTD");

    if (job->flags & JFLAGS_SUSPENDED)
      strcat(flags, "SUSP");

    /* Trim off the trailing space if any flags were listed. */
    if (flags[0] != '\0')
      flags[strlen(flags) - 1] = '\0';

    fprintf(dump, " %*d %*s %c %*s %*s %*d %*s %*s %*s\n",
            -DUMP_PRIORITY, job->sort_order,
            -DUMP_JID_LEN, jid,
            job->state,
            -DUMP_OWNER_LEN, owner,
            -DUMP_QUEUE_LEN, queue,
            -DUMP_NODES_LEN, job->ncpus,
            -DUMP_WALLT_LEN, wallt,
            -DUMP_ELIGI_LEN, eligi,
            -DUMP_FLAGS_LEN, flags);
    }

  fprintf(dump, "    Total: %d job%s\n\n", njobs, (njobs == 1) ? "" : "s");

  if (elig_mesg)
    {
    fprintf(dump, "Jobs marked with a ``*'' have an etime different "
            "from their ctime.\n\n");
    }

  return (njobs);
  }

int schd_get_max_ncpus(void)
  {

  char *id = "get_max_ncpus";
  Batch_Status *bs, *bsp;
  AttrList *attr;
  static AttrList alist[] = {{NULL, ATTR_rescmax, "", ""}};
  int ret = 0;


  /* Query the server for status of the max ncpus attribute */

  if ((bs = pbs_statserver(connector, alist, NULL)) == NULL)
    {
    sprintf(log_buffer, "pbs_statserver failed: %d", pbs_errno);
    log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, id, log_buffer);
    DBPRT(("%s: %s\n", id, log_buffer));
    return (ret);
    }

  /* Process the list of attributes returned by the server. */
  for (bsp = bs; bsp != NULL; bsp = bsp->next)
    {
    for (attr = bsp->attribs; attr != NULL; attr = attr->next)
      {
      if (!strcmp(attr->name, "resources_max"))
        {
        if (!strcmp(attr->resource, "ncpus"))
          {
          ret = atoi(attr->value);
          break;
          }
        }
      }
    }

  pbs_statfree(bs);

  return (ret);
  }
