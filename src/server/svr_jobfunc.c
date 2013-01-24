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
/*
 * svr_jobfunc.c - contains server functions dealing with jobs
 *
 * Included public functions are:
 * svr_enquejob()     - place job in a queue
 * svr_dequejob()     - remove job from queue
 * svr_setjobstate()  - set the state/substate of a job
 * svr_evaljobstate() - evaluate the state of a job based on attributes
 * chk_resc_limits()  - check job resources vs queue/server limits
 * svr_chkque()       - check if job can enter queue
 * job_set_wait()     - set event for when job's wait time ends
 * get_variable()     - get value of a single environ variable of a job
 * prefix_std_file()  - build the fully prefixed default name for std e/o
 * get_jobowner()     - get job owner name without @host suffix
 * set_resc_deflt()   - set unspecified resource_limit to default values
 * set_statechar()    - set the job state pbs_attribute character value
 *
 * Private functions
 * chk_svr_resc_limit() - check job requirements againt queue/server limits
 * default_std()      - make the default name for standard out/error
 * eval_checkpoint()  - insure job checkpoint .ge. queues min. time
 * set_deflt_resc()   - set unspecified resource_limit to default values
 * job_wait_over()    - event handler for job_set_wait()
 *
 * NOTE: for multi-threaded TORQUE, all functions in here assume that
 * the caller holds any relevant mutexes
 */
#include <pbs_config.h>   /* the master config generated by configure */

#include "pbs_ifl.h"
#include <sys/types.h>
#include <grp.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>

#include "server_limits.h"
#include "list_link.h"
#include "attribute.h"
#include "resource.h"
#include "server.h"
#include "queue.h"
#include "dynamic_string.h"
#include "pbs_job.h"
#include "work_task.h"
#include "pbs_error.h"
#include "log.h"
#include "../lib/Liblog/pbs_log.h"
#include "../lib/Liblog/log_event.h"
#include "acct.h"
#include "svrfunc.h"
#include "sched_cmds.h"
#include "pbs_proto.h"
#include "csv.h"
#include "array.h"
#include "queue_func.h" /* find_queuebyname */
#include "../lib/Libutils/u_lock_ctl.h" /* *startup */
#include "ji_mutex.h"
#include "user_info.h"
#include "svr_jobfunc.h"
#include "svr_task.h"
#include "mutex_mgr.hpp"

#define MSG_LEN_LONG 160

/* Private Functions */

static void default_std(job *, int key, dynamic_string *ds);
static void eval_checkpoint(pbs_attribute *j, pbs_attribute *q);
static int count_queued_jobs(pbs_queue *pque, char *user);

/* Global Data Items: */

extern struct server server;
extern struct all_jobs alljobs;
extern struct all_jobs array_summary;

extern char  *msg_badwait;  /* error message */
extern char  *msg_daemonname;
extern char  *pbs_o_host;
extern char   server_name[];
extern int    comp_resc_lt;
extern int    comp_resc_gt;
extern int    svr_do_schedule;
extern pthread_mutex_t *svr_do_schedule_mutex;
extern pthread_mutex_t *listener_command_mutex;
extern int    listener_command;
extern int    LOGLEVEL;

extern int    DEBUGMODE;

int           SvrNodeCt = 0;  /* cfg nodes or num nodes specified via resources_available */

extern int  svr_clnodes;

/* External Functions */

extern int node_avail_complex(char *, int *, int *, int *, int *);
extern int procs_available(int proc_ct);
extern int procs_requested(char *spec);

/* Private Functions */

#ifndef NDEBUG
static void correct_ct();
#endif  /* NDEBUG */

/* sync w/#define JOB_STATE_XXX */

const char *PJobState[] =
  {
  "TRANSIT",
  "QUEUED",
  "HELD",
  "WAITING",
  "RUNNING",
  "EXITING",
  "COMPLETE",
  NULL
  };


/* sync w/#define JOB_SUBSTATE_XXX, PJobSubState in mom_main.c */

const char *PJobSubState[] =
  {
  "TRANSIN",                /* Transit in, wait for commit */
  "TRANSICM",               /* Transit in, wait for commit */
  "TRNOUT",                 /* transiting job outbound */
  "TRNOUTCM",               /* transiting outbound, rdy to commit */
  "SUBSTATE04",
  "SUBSTATE05",
  "SUBSTATE06",
  "SUBSTATE07",
  "SUBSTATE08",
  "SUBSTATE09",
  "QUEUED",                 /* job queued and ready for selection */
  "PRESTAGEIN",             /* job queued, has files to stage in */
  "SUBSTATE12",
  "SYNCRES",                /* job waiting on sync start ready */
  "STAGEIN",                /* job staging in files then wait */
  "STAGEGO",                /* job staging in files and then run */
  "STAGECMP",               /* job stage in complete */
  "SUBSTATE17",
  "SUBSTATE18",
  "SUBSTATE19",
  "HELD",                   /* job held - user or operator */
  "SYNCHOLD",               /* job held - waiting on sync regist */
  "DEPNHOLD",               /* job held - waiting on dependency */
  "SUBSTATE23",
  "SUBSTATE24",
  "SUBSTATE25",
  "SUBSTATE26",
  "SUBSTATE27",
  "SUBSTATE28",
  "SUBSTATE29",
  "WAITING",                /* job waiting on execution time */
  "SUBSTATE31",
  "SUBSTATE32",
  "SUBSTATE33",
  "SUBSTATE34",
  "SUBSTATE35",
  "SUBSTATE36",
  "STAGEFAIL",              /* job held - file stage in failed */
  "SUBSTATE38",
  "SUBSTATE39",
  "PRERUN",                 /* job sent to MOM to run */
  "STARTING",
  "RUNNING",                /* job running */
  "SUSPEND",                /* job suspended, CRAY only */
  "SUBSTATE44",
  "SUBSTATE45",
  "SUBSTATE46",
  "SUBSTATE47",
  "SUBSTATE48",
  "SUBSTATE49",
  "EXITING",                /* start of job exiting processing */
  "STAGEOUT",               /* job staging out (other) files   */
  "STAGEDEL",               /* job deleting staged out files  */
  "EXITED",                 /* job exit processing completed   */
  "ABORT",                  /* job is being aborted by server  */
  "SUBSTATE55",
  "SUBSTATE56",
  "SUBSTATE57",
  "OBIT",                   /* (MOM) job obit notice sent */
  "COMPLETE",
  "RERUN",                  /* job is rerun, recover output stage */
  "RERUN1",                 /* job is rerun, stageout phase */
  "RERUN2",                 /* job is rerun, delete files stage */
  "RERUN3",                 /* job is rerun, mom delete job */
  "SUBSTATE64",
  "SUBSTATE65",
  "SUBSTATE66",
  "SUBSTATE67",
  "SUBSTATE68",
  "SUBSTATE69",
  "RETURNSTD",             /* returning stderr/stdout files to server spool */
  "SUBSTATE71",
  "SUBSTATE72",
  "SUBSTATE73",
  "SUBSTATE74",
  "ARRAY",
  NULL
  };




int insert_into_alljobs_by_rank(

  struct all_jobs *aj,
  job             *pjob,
  char            *jobid)

  {
  job  *pjcur;
  int   iter = -1;
  long  job_qrank = pjob->ji_wattr[JOB_ATR_qrank].at_val.at_long;
  int   index = -1;
  
  unlock_ji_mutex(pjob, __func__, "4", LOGLEVEL);

  while ((pjcur = next_job_from_back(aj, &iter)) != NULL)
    {
    if (job_qrank > pjcur->ji_wattr[JOB_ATR_qrank].at_val.at_long)
      {
      break;
      }
    
    if (strcmp(jobid, pjcur->ji_qs.ji_jobid) == 0)
      {
      unlock_ji_mutex(pjcur, __func__, "6", LOGLEVEL);
      return(ALREADY_IN_LIST);
      }

    unlock_ji_mutex(pjcur, __func__, "7", LOGLEVEL);
    }

  if (pjcur != NULL)
    {
    index = get_jobs_index(aj, pjcur);
    unlock_ji_mutex(pjcur, __func__, "8", LOGLEVEL);
    pjcur = NULL;
    }

  if ((pjob = svr_find_job(jobid, FALSE)) == NULL)
    {
    return(PBSE_JOBNOTFOUND);
    }
  
  if (index == -1)
    {
    /* link first in list */
    insert_job_first(aj, pjob);
    }
  else
    {
    /* link after 'current' job in list */
    insert_job_after_index(aj, index, pjob);
    }

  return(PBSE_NONE);
  } /* END insert_into_alljobs_by_rank() */




/*
 * svr_enquejob() - enqueue job into specified queue
 */
int svr_enquejob(

  job *pjob,            /* I */
  int  has_sv_qs_mutex, /* I */
  int  prev_job_index)  /* I */

  {
  pbs_attribute *pattrjb;
  attribute_def *pdef;
  pbs_queue     *pque;
  int            rc = -1;
  char           log_buf[LOCAL_LOG_BUF_SIZE];
  time_t         time_now = time(NULL);
  char           job_id[PBS_MAXSVRJOBID+1];
  char           queue_name[PBS_MAXQUEUENAME+1];
  int            total_jobs = 0;
  int            user_jobs = 0;
  int            array_jobs = 0;

  if (pjob == NULL)
    {
    log_err(rc, __func__, "NULL job pointer input");
    return(rc);
    }

  /* make sure queue is still there, there exists a small window ... */
  strcpy(job_id, pjob->ji_qs.ji_jobid);
  /* ji_qs.ji_queue now holds the name of the destination queue */
  strcpy(queue_name, pjob->ji_qs.ji_queue);
  unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
  pque = find_queuebyname(queue_name);

  if (pque == NULL)
    {
    return(PBSE_UNKQUE);
    }

  mutex_mgr que_mgr(pque->qu_mutex, true);

  /* This is called when a job is not yet in the queue,
   * so svr_find_job can not be used.... */
  lock_ji_mutex(pjob, __func__, NULL, LOGLEVEL);

  if (pjob->ji_being_recycled == TRUE)
    {
    que_mgr.unlock();
    unlock_ji_mutex(pjob, __func__, "2", LOGLEVEL);
    return(PBSE_JOB_RECYCLED);
    }

  /* Check again if there are any maximum queuable limits */
  if (pjob->ji_is_array_template)
    {
    array_jobs = num_array_jobs(pjob->ji_wattr[JOB_ATR_job_array_request].at_val.at_str);
    if (array_jobs > 0)
      array_jobs--;
    }
      
  if ((pque->qu_attr[QA_ATR_MaxJobs].at_flags & ATR_VFLAG_SET))
    {
    total_jobs = count_queued_jobs(pque, NULL);
    if (total_jobs + array_jobs >= pque->qu_attr[QA_ATR_MaxJobs].at_val.at_long)
      {
      que_mgr.unlock();
      unlock_ji_mutex(pjob, __func__, "3", LOGLEVEL);
      return(PBSE_MAXQUED);
      }
    }


  if ((pque->qu_attr[QA_ATR_MaxUserJobs].at_flags & ATR_VFLAG_SET))
    {
    user_jobs = count_queued_jobs(pque, pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str);
    if (user_jobs >= pque->qu_attr[QA_ATR_MaxUserJobs].at_val.at_long)
      {
      que_mgr.unlock();
      unlock_ji_mutex(pjob, __func__, "3", LOGLEVEL);
      return(PBSE_MAXUSERQUED);
      }
    }

  /* add job to server's 'all job' list and update server counts */

#ifndef NDEBUG
  sprintf(log_buf, "enqueuing into %s, state %x hop %ld",
    pque->qu_qs.qu_name,
    pjob->ji_qs.ji_state,
    pjob->ji_wattr[JOB_ATR_hopcount].at_val.at_long);

  log_event(PBSEVENT_DEBUG2,PBS_EVENTCLASS_JOB,pjob->ji_qs.ji_jobid,log_buf);

#endif /* NDEBUG */

  if (!pjob->ji_is_array_template)
    {
    if (prev_job_index < 0)
      insert_job(&alljobs, pjob);
    else
      insert_job_after_index(&alljobs, prev_job_index, pjob);

    if (has_sv_qs_mutex == FALSE)
      {
      lock_sv_qs_mutex(server.sv_qs_mutex, __func__);
      }

    server.sv_qs.sv_numjobs++;
    
    if (has_sv_qs_mutex == FALSE)
      unlock_sv_qs_mutex(server.sv_qs_mutex, __func__);

    pthread_mutex_lock(server.sv_jobstates_mutex);
    server.sv_jobstates[pjob->ji_qs.ji_state]++;
    pthread_mutex_unlock(server.sv_jobstates_mutex);
    }
  
  /* place into array_summary if necessary */
  if ((pjob->ji_is_array_template) ||
      (pjob->ji_arraystructid[0] == '\0'))
    {
    if ((rc = has_job(&array_summary, pjob)) == FALSE)
      {
      insert_job(&array_summary, pjob);
      }
    else if (rc == PBSE_JOB_RECYCLED)
      return(rc);
    } /* END if (pjob->is_array_template) */

  /* place into queue in order of queue rank starting at end */
  pjob->ji_qhdr = pque;

  if (!pjob->ji_is_array_template)
    {
    rc = insert_into_alljobs_by_rank(pque->qu_jobs, pjob, job_id);

    if ((rc == ALREADY_IN_LIST) ||
        (rc == PBSE_JOBNOTFOUND))
      {
      if (rc == ALREADY_IN_LIST)
        {
        rc = PBSE_NONE;
        }

      return(rc);
      }

    /* update counts: queue and queue by state */
    pque->qu_numjobs++;
    pque->qu_njstate[pjob->ji_qs.ji_state]++;
    
    /* increment this user's job count for this queue */
    increment_queued_jobs(pque->qu_uih, pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str, pjob);
    }

  if ((pjob->ji_is_array_template) ||
      (pjob->ji_arraystructid[0] == '\0'))
    {
    rc = insert_into_alljobs_by_rank(pque->qu_jobs_array_sum, pjob, job_id);

    if ((rc == ALREADY_IN_LIST) ||
        (rc == PBSE_JOBNOTFOUND))
      {
      if (rc == ALREADY_IN_LIST)
        rc = PBSE_NONE;

      return(rc);
      }
    }

  /* update the current location and type pbs_attribute */
  pdef    = &job_attr_def[JOB_ATR_in_queue];

  pattrjb = &pjob->ji_wattr[JOB_ATR_in_queue];

  pdef->at_free(pattrjb);

  pdef->at_decode(pattrjb, NULL, NULL, pque->qu_qs.qu_name, 0);

  pjob->ji_wattr[JOB_ATR_queuetype].at_val.at_char =
    *pque->qu_attr[QA_ATR_QType].at_val.at_str;

  pjob->ji_wattr[JOB_ATR_queuetype].at_flags |= ATR_VFLAG_SET;

  if ((pjob->ji_wattr[JOB_ATR_qtime].at_flags & ATR_VFLAG_SET) == 0)
    {
    pjob->ji_wattr[JOB_ATR_qtime].at_val.at_long = time_now;
    pjob->ji_wattr[JOB_ATR_qtime].at_flags |= ATR_VFLAG_SET;

    /* issue enqueued accounting record */

    sprintf(log_buf, "queue=%s", pque->qu_qs.qu_name);

    account_record(PBS_ACCT_QUEUE, pjob, log_buf);
    }

  /*
   * set any "unspecified" resources which have default values,
   * first with queue defaults, then with server defaults
   */
  set_resc_deflt(pjob, NULL, TRUE);

  /* set any "unspecified" checkpoint with queue default values, if any */
  set_chkpt_deflt(pjob, pque);

  /* See if we need to do anything special based on type of queue */
  if (pque->qu_qs.qu_type == QTYPE_Execution)
    {
    /* set union to "EXEC" and clear mom's address */

    if (pjob->ji_qs.ji_un_type != JOB_UNION_TYPE_EXEC)
      {
      pjob->ji_qs.ji_un_type = JOB_UNION_TYPE_EXEC;
      pjob->ji_qs.ji_un.ji_exect.ji_momaddr = 0;
      pjob->ji_qs.ji_un.ji_exect.ji_exitstat = 0;
      }

    /* check the job checkpoint against the queue's min */
    eval_checkpoint(
      &pjob->ji_wattr[JOB_ATR_checkpoint],
      &pque->qu_attr[QE_ATR_checkpoint_min]);

    /* do anything needed doing regarding job dependencies */
    que_mgr.unlock();

    if ((pjob->ji_qs.ji_state != JOB_STATE_COMPLETE) && 
        (pjob->ji_qs.ji_substate != JOB_SUBSTATE_COMPLETE) && 
        (pjob->ji_wattr[JOB_ATR_depend].at_flags & ATR_VFLAG_SET))
      {
      if ((rc = depend_on_que(&
                              pjob->ji_wattr[JOB_ATR_depend],
                              pjob,
                              ATR_ACTION_NOOP)) != 0)
        {
        return(PBSE_BADDEPEND);
        }
      }

    /* set eligible time */
    if (((pjob->ji_wattr[JOB_ATR_etime].at_flags & ATR_VFLAG_SET) == 0) &&
        (pjob->ji_qs.ji_state == JOB_STATE_QUEUED))
      {
      pjob->ji_wattr[JOB_ATR_etime].at_val.at_long = time_now;
      pjob->ji_wattr[JOB_ATR_etime].at_flags |= ATR_VFLAG_SET;
      }
      
    /* notify the scheduler we have a new job */
    pthread_mutex_lock(svr_do_schedule_mutex);
    svr_do_schedule = SCH_SCHEDULE_NEW;
    pthread_mutex_unlock(svr_do_schedule_mutex);
    pthread_mutex_lock(listener_command_mutex);
    listener_command = SCH_SCHEDULE_NEW;
    pthread_mutex_unlock(listener_command_mutex);
    }
  else if (pque->qu_qs.qu_type == QTYPE_RoutePush)
    {
    /* start attempts to route job */
    pjob->ji_qs.ji_un_type = JOB_UNION_TYPE_ROUTE;
    pjob->ji_qs.ji_un.ji_routet.ji_quetime = time_now;
    
    }

  return(PBSE_NONE);
  }  /* END svr_enquejob() */





/*
 * svr_dequejob() - remove job from whatever queue its in and reduce counts
 * Make sure job mutex is unlocked before exiting
 */

int svr_dequejob(

  job *pjob,                    /* I, M */
  int  parent_queue_mutex_held) /* I */

  {
  int            bad_ct = 0;
  int            rc = PBSE_NONE;
  pbs_attribute *pattr;
  pbs_queue     *pque;
  resource      *presc;
  char           log_buf[LOCAL_LOG_BUF_SIZE];

  /* remove job from server's all job list and reduce server counts */
  if (LOGLEVEL >= 7)
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, pjob->ji_qs.ji_jobid);

  if (parent_queue_mutex_held == FALSE)
    {
    pque = get_jobs_queue(&pjob);

    if (pjob == NULL)
      {
      log_err(PBSE_JOBNOTFOUND, __func__, "Job lost while acquiring queue 10");
      return(PBSE_JOBNOTFOUND);
      }
    if (pque == NULL)
      {
      unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
      log_err(PBSE_JOBNOTFOUND, __func__, "Job has no queue");
      return(PBSE_JOBNOTFOUND);
      }
    }
  else
    pque = pjob->ji_qhdr;

  if (pque != NULL)
    {
    if ((rc = remove_job(pque->qu_jobs, pjob)) == PBSE_NONE)
      {
      if (--pque->qu_numjobs < 0)
        {
        bad_ct = 1;
        log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_JOB, __func__, "qu_numjobs < 0. Recount required.");
        }

      if (--pque->qu_njstate[pjob->ji_qs.ji_state] < 0)
        {
        bad_ct = 1;
        log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_JOB, __func__, "qu_njstate < 0. Recount required.");
        }

      if ((pjob->ji_qs.ji_state == JOB_STATE_COMPLETE) &&
          (--pque->qu_numcompleted < 0))
        {
        log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_JOB, __func__, "qu_numcompleted < 0. Recount required.");
        bad_ct = 1;
        }

      if (pjob->ji_is_array_template == FALSE)
        {
        decrement_queued_jobs(pque->qu_uih, pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str);
        }
      }
    else if (rc == PBSE_JOB_RECYCLED)
      {
      /* calling functions know this return code means the job is gone */
      return(PBSE_JOBNOTFOUND);
      }

    /* the only reason to care about the error is if the job is gone */
    if (remove_job(pque->qu_jobs_array_sum, pjob) == PBSE_JOB_RECYCLED)
      return(PBSE_JOBNOTFOUND);

    pjob->ji_qhdr = NULL;

    if (parent_queue_mutex_held == FALSE)
      unlock_queue(pque, __func__, NULL, LOGLEVEL);
    }

#ifndef NDEBUG

  snprintf(log_buf, sizeof(log_buf), "dequeuing from %s, state %s",
    pque ? pque->qu_qs.qu_name : "unknown queue",
    PJobState[pjob->ji_qs.ji_state]);

  log_event(PBSEVENT_DEBUG2, PBS_EVENTCLASS_JOB, pjob->ji_qs.ji_jobid, log_buf);

  if (bad_ct)   /* state counts are all messed up */
    {
    char queue_name[PBS_MAXQUEUENAME+1];
    char           job_id[PBS_MAXSVRJOBID+1];

    strcpy(job_id, pjob->ji_qs.ji_jobid);

    /* this function will lock queues and jobs */
    unlock_ji_mutex(pjob, __func__, NULL, LOGLEVEL);

    if (parent_queue_mutex_held == TRUE)
      {
      strcpy(queue_name, pque->qu_qs.qu_name);
      unlock_queue(pque, __func__, NULL, LOGLEVEL);
      }

    correct_ct();

    /* lock queue then job */
    if (parent_queue_mutex_held == TRUE)
      {
      pque = find_queuebyname(queue_name);
      }

    if ((pjob = svr_find_job(job_id, FALSE)) == NULL)
      return(PBSE_JOBNOTFOUND);
    }

#endif /* NDEBUG */

  pjob->ji_wattr[JOB_ATR_qtime].at_flags &= ~ATR_VFLAG_SET;

  /* clear any default resource values.  */

  pattr = &pjob->ji_wattr[JOB_ATR_resource];

  if (pattr->at_flags & ATR_VFLAG_SET)
    {
    presc = (resource *)GET_NEXT(pattr->at_val.at_list);

    while (presc != NULL)
      {
      if (presc->rs_value.at_flags & ATR_VFLAG_DEFLT)
        presc->rs_defin->rs_free(&presc->rs_value);

      presc = (resource *)GET_NEXT(presc->rs_link);
      }
    }

  /* the only error is if the job isn't present */
  if ((rc = remove_job(&alljobs, pjob)) == PBSE_NONE)
    {
    if (!pjob->ji_is_array_template)
      {
      lock_sv_qs_mutex(server.sv_qs_mutex, __func__);

      if (--server.sv_qs.sv_numjobs < 0)
        {
        bad_ct = 1;
        log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_JOB, __func__, "sv_numjobs < 0. Recount required.");
        }
      
      unlock_sv_qs_mutex(server.sv_qs_mutex, __func__);
      pthread_mutex_lock(server.sv_jobstates_mutex);

      if (--server.sv_jobstates[pjob->ji_qs.ji_state] < 0)
        {
        bad_ct = 1;
        log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_JOB, __func__, "sv_jobstates < 0. Recount required.");
        }
      
      pthread_mutex_unlock(server.sv_jobstates_mutex);
      }
    }
  else if (rc == PBSE_JOB_RECYCLED)
    {
    /* calling functions know this return code means the job is gone */
    return(PBSE_JOBNOTFOUND);
    }

  /* notify scheduler a job has been removed */

  pthread_mutex_lock(svr_do_schedule_mutex);
  svr_do_schedule = SCH_SCHEDULE_TERM;
  pthread_mutex_unlock(svr_do_schedule_mutex);
  pthread_mutex_lock(listener_command_mutex);
  listener_command = SCH_SCHEDULE_TERM;
  pthread_mutex_unlock(listener_command_mutex);

  return(PBSE_NONE);
  }  /* END svr_dequejob() */





/*
 * svr_setjobstate - set the job state, update the server/queue state counts,
 * and save the job
 */

int svr_setjobstate(

  job *pjob,            /* I (modified) */
  int  newstate,        /* I */
  int  newsubstate,     /* I */
  int  has_queue_mutex) /* I */

  {
  int          changed = 0;
  int          oldstate;

  pbs_queue   *pque;
  char         log_buf[LOCAL_LOG_BUF_SIZE];
  time_t       time_now = time(NULL);

  if (pjob == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input job pointer");
    return(PBSE_BAD_PARAMETER);
    }

  if (LOGLEVEL >= 2)
    {
    sprintf(log_buf, "%s: setting job %s state from %s-%s to %s-%s (%d-%d)\n",
      __func__,
      pjob->ji_qs.ji_jobid,
      PJobState[pjob->ji_qs.ji_state],
      PJobSubState[pjob->ji_qs.ji_substate],
      PJobState[newstate],
      PJobSubState[newsubstate],
      newstate,
      newsubstate);

    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
    }

  /*
   * if its is a new job, then don't update counts, svr_enquejob() will
   * take care of that, also req_commit() will see that the job is saved.
   */

  if (pjob->ji_qs.ji_substate != JOB_SUBSTATE_TRANSICM)
    {
    /* Not a new job, update the counts and save if needed */

    if (pjob->ji_qs.ji_substate != newsubstate)
      changed = 1;

    /* if the state is changing, also update the state counts */

    oldstate = pjob->ji_qs.ji_state;

    if (oldstate != newstate)
      {
      changed = 1;

      /* the array job isn't actually a job so don't count it here */
      if (pjob->ji_is_array_template == FALSE)
        {
        pthread_mutex_lock(server.sv_jobstates_mutex);
        server.sv_jobstates[oldstate]--;
        server.sv_jobstates[newstate]++;
        pthread_mutex_unlock(server.sv_jobstates_mutex);
        }

      if (has_queue_mutex == FALSE)
        {
        pque = get_jobs_queue(&pjob);

        if (pjob == NULL)
          {
          log_err(PBSE_JOBNOTFOUND, __func__, "Job lost while acquiring queue 11");
          return(PBSE_JOBNOTFOUND);
          }
        }
      else
        pque = pjob->ji_qhdr;

      if (pque != NULL)
        {
        /* the array job isn't actually a job so don't count it here */
        if (pjob->ji_is_array_template == FALSE)
          {
          pque->qu_njstate[oldstate]--;
          pque->qu_njstate[newstate]++;
          }

        /* if execution queue, and eligibility to run has improved, */
        /* notify the scheduler */

        if ((pque->qu_qs.qu_type == QTYPE_Execution) &&
            (newstate == JOB_STATE_QUEUED))
          {
          pthread_mutex_lock(svr_do_schedule_mutex);
          svr_do_schedule = SCH_SCHEDULE_NEW;
          pthread_mutex_unlock(svr_do_schedule_mutex);
          pthread_mutex_lock(listener_command_mutex);
          listener_command = SCH_SCHEDULE_NEW;
          pthread_mutex_unlock(listener_command_mutex);

          if ((pjob->ji_wattr[JOB_ATR_etime].at_flags & ATR_VFLAG_SET) == 0)
            {
            pjob->ji_wattr[JOB_ATR_etime].at_val.at_long = time_now;
            pjob->ji_wattr[JOB_ATR_etime].at_flags |= ATR_VFLAG_SET;
            }
          }
        else if ((newstate == JOB_STATE_HELD) ||
                 (newstate == JOB_STATE_WAITING))
          {
          /* on hold or wait, clear etime */

          job_attr_def[JOB_ATR_etime].at_free(
            &pjob->ji_wattr[JOB_ATR_etime]);
          }

        if (has_queue_mutex == FALSE)
          unlock_queue(pque, __func__, NULL, LOGLEVEL);
        }
      }
    }    /* END if (pjob->ji_qs.ji_substate != JOB_SUBSTATE_TRANSICM) */

  /* set the states accordingly */

  pjob->ji_qs.ji_state = newstate;

  pjob->ji_qs.ji_substate = newsubstate;

  pjob->ji_wattr[JOB_ATR_substate].at_val.at_long = newsubstate;

  set_statechar(pjob);

  /* update the job file */

  if (pjob->ji_modified)
    {
    return(job_save(pjob, SAVEJOB_FULL,0));
    }

  if (changed)
    {
    return(job_save(pjob, SAVEJOB_QUICK,0));
    }

  return(0);
  }  /* END svr_setjobstate() */




/*
 * 
 - evaluate and return the job state and substate
 * according to the the values of the hold, execution time, and
 * dependency attributes.  This is typically called after the job has been
 * enqueued or the (hold, execution-time) attributes have been modified.
 */

void svr_evaljobstate(

  job *pjob,
  int *newstate,  /* O recommended new state for job    */
  int *newsub,    /* O recommended new substate for job */
  int  forceeval)

  {
  if (pjob == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input job pointer");
    return;
    }
  if (newstate == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input newstate pointer");
    return;
    }
  if (newsub == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input newsub pointer");
    return;
    }

  if ((forceeval == 0) &&
      ((pjob->ji_qs.ji_state == JOB_STATE_RUNNING) ||
       (pjob->ji_qs.ji_state == JOB_STATE_TRANSIT)))
    {
    *newstate = pjob->ji_qs.ji_state; /* leave as is */
    *newsub   = pjob->ji_qs.ji_substate;
    }
  else if (pjob->ji_wattr[JOB_ATR_hold].at_val.at_long)
    {
    *newstate = JOB_STATE_HELD;

    /* is the hold due to a dependency? */

    if ((pjob->ji_qs.ji_substate == JOB_SUBSTATE_SYNCHOLD) ||
        (pjob->ji_qs.ji_substate == JOB_SUBSTATE_DEPNHOLD))
      *newsub = pjob->ji_qs.ji_substate;  /* keep it */
    else
      *newsub = JOB_SUBSTATE_HELD;
    }
  else if (pjob->ji_wattr[JOB_ATR_exectime].at_val.at_long > (long)time(NULL))
    {
    *newstate = JOB_STATE_WAITING;
    *newsub   = JOB_SUBSTATE_WAITING;
    }
  else if (pjob->ji_wattr[JOB_ATR_stagein].at_flags & ATR_VFLAG_SET)
    {
    *newstate = JOB_STATE_QUEUED;

    if (pjob->ji_qs.ji_svrflags & JOB_SVFLG_StagedIn)
      {
      *newsub = JOB_SUBSTATE_STAGECMP;
      }
    else
      {
      *newsub = JOB_SUBSTATE_PRESTAGEIN;
      }
    }
  else
    {
    *newstate = JOB_STATE_QUEUED;
    *newsub   = JOB_SUBSTATE_QUEUED;
    }

  return;
  }  /* END svr_evaljobstate() */




/*
 * get_variable - get the value associated with a specified environment
 * variable of a job
 *
 * Returns a pointer to the start of the value or NULL if the variable
 * is not found in the variable_list pbs_attribute.
 */

char *get_variable(

  job           *pjob,     /* I */
  const char   *variable) /* I */

  {
  char *pc;

  pc = arst_string(
         variable,
         &pjob->ji_wattr[JOB_ATR_variables]);

  if (pc)
    {
    if ((pc = strchr(pc, (int)'=')) != NULL)
      pc++;
    }

  return(pc);
  }  /* END get_variable() */




/*
 * get_resource - find a resource (value) entry in the queue or server list
 *
 * Returns: pointer to struct resource or NULL
 */

static resource *get_resource(

  pbs_attribute *p_queattr,  /* I */
  pbs_attribute *p_svrattr,  /* I */
  resource_def  *rscdf,      /* I */
  int           *fromQueue)  /* O */

  {
  resource *pr;

  pr = find_resc_entry(p_queattr, rscdf);

  if ((pr == NULL) || ((pr->rs_value.at_flags & ATR_VFLAG_SET) == 0))
    {
    /* queue limit not set, check server's */

    pr = find_resc_entry(p_svrattr, rscdf);

    if ((pr != NULL) && (pr->rs_value.at_flags & ATR_VFLAG_SET))
      {
      *fromQueue = 0;
      }
    }
  else
    {
    /* queue limit is set, use it */

    *fromQueue = 1;
    }

  return(pr);
  }  /* END get_resource() */




/*
 * compare the job resource limit against the system limit
 * unless a queue limit exists, it takes priority
 *
 * returns number of .gt. and .lt. comparison in comp_resc_gt and comp_resc_lt
 * does not make use of comp_resc_eq or comp_resc_nc
 */

int chk_svr_resc_limit(

  pbs_attribute *jobatr, /* I */
  pbs_queue     *pque,   /* I */
  int            qtype,   /* I */
  char          *EMsg)   /* O (optional,minsize=1024) */

  {
  int           dummy;
  int           rc;
  int           req_procs = 0; /* must start at 0 */
  resource     *jbrc;
  resource     *jbrc_nodes = NULL;
  resource     *qurc;
  resource     *svrc;
  resource     *cmpwith;

  int           LimitIsFromQueue = FALSE;
  const char  *LimitName;

  /* NOTE:  support Cray-specific evaluation */

  int           MPPWidth = 0;
  int           PPN = 0;
  int           comp_resc_lt = 0;

  long          mpp_nppn = 0;
  long          mpp_width = 0;
  long          mpp_nodect = 0;
  resource     *mppnodect_resource = NULL;
  long          proc_ct = 0;

  resource_def *noderesc     = NULL;
  resource_def *needresc     = NULL;
  resource_def *nodectresc   = NULL;
  resource_def *mppwidthresc = NULL;
  resource_def *mppnppn      = NULL;
  resource_def *procresc     = NULL;
  char          log_buf[LOCAL_LOG_BUF_SIZE];

  /* NOTE:  server limits are specified with server.resources_available */

  if (EMsg != NULL)
    EMsg[0] = '\0';

  noderesc     = find_resc_def(svr_resc_def, "nodes",     svr_resc_size);
  needresc     = find_resc_def(svr_resc_def, "neednodes", svr_resc_size);
  nodectresc   = find_resc_def(svr_resc_def, "nodect",    svr_resc_size);
  mppwidthresc = find_resc_def(svr_resc_def, "mppwidth",  svr_resc_size);
  mppnppn      = find_resc_def(svr_resc_def, "mppnppn",   svr_resc_size);
  procresc     = find_resc_def(svr_resc_def, "procs",   svr_resc_size);
  
  SvrNodeCt = 0;
  
  if (nodectresc != NULL)
    {
    pthread_mutex_lock(server.sv_attr_mutex);
    svrc = (resource *)GET_NEXT(
        server.sv_attr[SRV_ATR_resource_avail].at_val.at_list);
    
    while (svrc != NULL)
      {
      if (svrc->rs_defin == nodectresc)
        {
        svrc = find_resc_entry(
            &server.sv_attr[SRV_ATR_resource_avail],
            svrc->rs_defin);
        
        if ((svrc != NULL) && (svrc->rs_value.at_flags & ATR_VFLAG_SET))
          {
          SvrNodeCt = svrc->rs_value.at_val.at_long;
          }
        }
      
      svrc = (resource *)GET_NEXT(svrc->rs_link);
      } /* END while (svrc != NULL) */
    
    pthread_mutex_unlock(server.sv_attr_mutex);
    }   /* END if (nodectresc != NULL) */

  /* return values via global comp_resc_gt and comp_resc_lt */
  comp_resc_gt = 0;


  jbrc = (resource *)GET_NEXT(jobatr->at_val.at_list);

  while (jbrc != NULL)
    {
    cmpwith = NULL;

    if ((jbrc->rs_value.at_flags & (ATR_VFLAG_SET | ATR_VFLAG_DEFLT)) == ATR_VFLAG_SET)
      {
      qurc = find_resc_entry(&pque->qu_attr[QA_ATR_ResourceMax], jbrc->rs_defin);

      LimitName = (jbrc->rs_defin->rs_name == NULL)?"resource":jbrc->rs_defin->rs_name;

      pthread_mutex_lock(server.sv_attr_mutex);
      cmpwith = get_resource(&pque->qu_attr[QA_ATR_ResourceMax],
          &server.sv_attr[SRV_ATR_ResourceMax],
          jbrc->rs_defin,
          &LimitIsFromQueue);
      pthread_mutex_unlock(server.sv_attr_mutex);

      if (strcmp(LimitName,"mppnppn") == 0)
        {
        mpp_nppn = jbrc->rs_value.at_val.at_long;
        }
      else if (strcmp(LimitName,"mppwidth") == 0)
        {
        mpp_width = jbrc->rs_value.at_val.at_long;
        }

      if ((jbrc->rs_defin == noderesc) && 
          (qtype == QTYPE_Execution))
        {
        /* NOTE:  process after loop so SvrNodeCt is loaded */
        /* can check pure nodes violation right here */
        int job_nodes;
        int queue_nodes;

        jbrc_nodes = jbrc;
        if ((jbrc_nodes != NULL) && 
            (qurc != NULL))
          {
          if ((isdigit(*(jbrc_nodes->rs_value.at_val.at_str))) &&
              (isdigit(*(qurc->rs_value.at_val.at_str))))
            {
            job_nodes   = atoi(jbrc_nodes->rs_value.at_val.at_str);
            queue_nodes = atoi(qurc->rs_value.at_val.at_str);

            if (queue_nodes < job_nodes)
              comp_resc_lt++;
            }
          }
        }

      /* Added 6/14/2010 Ken Nielson for ability to parse the procs resource */
      else if ((jbrc->rs_defin == procresc) &&
               (qtype == QTYPE_Execution))
        {
        proc_ct = jbrc->rs_value.at_val.at_long;
        }

#ifdef NERSCDEV
      else if (jbrc->rs_defin == mppwidthresc)
        {
        if (jbrc->rs_value.at_flags & ATR_VFLAG_SET)
          {
          MPPWidth = jbrc->rs_value.at_val.at_long;
          }
        }

#endif /* NERSCDEV */
      else if ((strcmp(LimitName,"mppnodect") == 0) &&
               (jbrc->rs_value.at_val.at_long == -1))
        {
        /*
         * mppnodect is a special attrtibute,  It gets set based upon the
         * values of mppwidth and mppnppn. -1 signifies the case where mppwidth
         * and mppnppn were not both specified for the job. We will need to
         * check mppnodect limits against queue/server defaults, if any.
         */
         
        mppnodect_resource = jbrc;
        }
      else if ((cmpwith != NULL) && 
               (jbrc->rs_defin != needresc))
        {
        /* don't check neednodes */

        rc = jbrc->rs_defin->rs_comp(
               &cmpwith->rs_value,
               &jbrc->rs_value);

        if (rc > 0)
          {
          comp_resc_gt++;
          }
        else if (rc < 0)
          {
          /* only record if:
           *     is_transit flag is not set
           * or  is_transit is set, but not to true
           * or  the value comes from queue limit
           */
          if ((!(pque->qu_attr[QE_ATR_is_transit].at_flags & ATR_VFLAG_SET)) ||
              (!pque->qu_attr[QE_ATR_is_transit].at_val.at_long) ||
              (LimitIsFromQueue))
            {
            if ((EMsg != NULL) && (EMsg[0] == '\0'))
              {
              sprintf(EMsg, "cannot satisfy %s max %s requirement",
                      (LimitIsFromQueue == 1) ? "queue" : "server",LimitName);
              }

              comp_resc_lt++;
            }
          }
        }
      }    /* END if () */

    jbrc = (resource *)GET_NEXT(jbrc->rs_link);
    }  /* END while (jbrc != NULL) */

  if (mppnodect_resource != NULL)
    {
    /*
     * special case where mppnodect was not specified for the job, we need to
     * check max against recalculated value using queue/server resources_defaults
     */
     
    if (mpp_nppn == 0)
      {     
      /* get queue/server default value */
      
      if (mppnppn != NULL)
        {
        pthread_mutex_lock(server.sv_attr_mutex);
        cmpwith = get_resource(&pque->qu_attr[QA_ATR_ResourceDefault],
            &server.sv_attr[SRV_ATR_resource_deflt],
            mppnppn,
            &LimitIsFromQueue);
        pthread_mutex_unlock(server.sv_attr_mutex);
          
        if (cmpwith != NULL)
          {
          mpp_nppn = cmpwith->rs_value.at_val.at_long;
          }
        }
      }

    if (mpp_width == 0)
      {
      /* get queue/server default value */

      if (mppwidthresc != NULL)
        {
        pthread_mutex_lock(server.sv_attr_mutex);
        cmpwith = get_resource(&pque->qu_attr[QA_ATR_ResourceDefault],
            &server.sv_attr[SRV_ATR_resource_deflt],
            mppwidthresc,
            &LimitIsFromQueue);
        pthread_mutex_unlock(server.sv_attr_mutex);
          
        if (cmpwith != NULL)
          {
          mpp_width = cmpwith->rs_value.at_val.at_long;
          }
        }
      }

    /* Uses same way of calculating as set_mppnodect */
    /* Check for width less than a node */

    if ((mpp_width) && (mpp_width < mpp_nppn))
      {
      mpp_nppn = mpp_width;
      }

    /* Compute an estimate for the number of nodes needed */

    mpp_nodect = mpp_width;
    if (mpp_nppn > 1)
      {
      mpp_nodect = (mpp_nodect + mpp_nppn - 1) / mpp_nppn;
      }

    LimitIsFromQueue = FALSE;
    LimitName = mppnodect_resource->rs_defin->rs_name;
    
    pthread_mutex_lock(server.sv_attr_mutex);
    cmpwith = get_resource(&pque->qu_attr[QA_ATR_ResourceMax],
        &server.sv_attr[SRV_ATR_ResourceMax],
        mppnodect_resource->rs_defin,
        &LimitIsFromQueue);
    pthread_mutex_unlock(server.sv_attr_mutex);

    if (cmpwith != NULL)
      {
      long nodect_orig;

      if (LOGLEVEL >= 7)
        {
        sprintf(log_buf,
          "chk_svr_resc_limit: comparing calculated mppnodect %ld, %s limit %s %ld\n",
          mpp_nodect,
          (LimitIsFromQueue == 1) ? "queue" : "server",
          LimitName,
          cmpwith->rs_value.at_val.at_long);
        
        log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_SERVER, msg_daemonname, log_buf);
        }

      nodect_orig = mppnodect_resource->rs_value.at_val.at_long;
      mppnodect_resource->rs_value.at_val.at_long = mpp_nodect;
      
      rc = mppnodect_resource->rs_defin->rs_comp(
              &cmpwith->rs_value,
              &mppnodect_resource->rs_value);

      mppnodect_resource->rs_value.at_val.at_long = nodect_orig;

      if (rc > 0)
        {
        comp_resc_gt++;
        }
      else if (rc < 0)
        {
        /* only record if:
         *     is_transit flag is not set
         * or  is_transit is set, but not to true
         * or  the value comes from queue limit
         */
        if ((!(pque->qu_attr[QE_ATR_is_transit].at_flags & ATR_VFLAG_SET)) ||
            (!pque->qu_attr[QE_ATR_is_transit].at_val.at_long) ||
            (LimitIsFromQueue))
          {
          if ((EMsg != NULL) && (EMsg[0] == '\0'))
            {
            sprintf(EMsg, "cannot satisfy %s max %s requirement",
                    (LimitIsFromQueue == 1) ? "queue" : "server",
                    (LimitName != NULL) ? LimitName : "resource");
            }

          comp_resc_lt++;
          }
        }
      }
    }  /* END if (mppnodect_resource != NULL) */

  if (jbrc_nodes != NULL)
    {
    int tmpI;

    char *ptr;

    int IgnTest = 0;

    /* the DREADED special case ...         */
    /* check nodes using special function   */

    /* NOTE:  if 'nodes' is simple nodect specification AND server nodect *
              is set, and requested nodes < server nodect, ignore special *
              case test */

    ptr = jbrc_nodes->rs_value.at_val.at_str;

    if (isdigit(*ptr) && !strchr(ptr, ':') && !strchr(ptr, '+'))
      {
      tmpI = (int)strtol(ptr, NULL, 10);

      if ((SvrNodeCt > 0) && (tmpI <= SvrNodeCt))
        IgnTest = 1;
      }

    if (IgnTest == 0)
      {
      /* how many processors does this spec want */
      req_procs += procs_requested(jbrc_nodes->rs_value.at_val.at_str);

      if (node_avail_complex(
            jbrc_nodes->rs_value.at_val.at_str,
            &dummy,
            &dummy,
            &dummy,
            &dummy) == -1)
        {
        /* only record if:
         *     is_transit flag is not set
         * or  is_transit is set, but not to true
         */
        if ((!(pque->qu_attr[QE_ATR_is_transit].at_flags & ATR_VFLAG_SET)) ||
            (!pque->qu_attr[QE_ATR_is_transit].at_val.at_long))
          {
          if ((EMsg != NULL) && (EMsg[0] == '\0'))
            strcpy(EMsg, "cannot locate feasible nodes (nodes file is empty or all systems are busy)");
        
          comp_resc_lt++;
          }
        }
      }
    }    /* END if (jbrc_nodes != NULL) */

  if (proc_ct > 0)
    {
    if (procs_available(proc_ct) == -1)
      {
      /* only record if:
       * is_transit flag is not set
       * or  is_transit is set, but not to true
       */
      if ((!(pque->qu_attr[QE_ATR_is_transit].at_flags & ATR_VFLAG_SET)) ||
          (!pque->qu_attr[QE_ATR_is_transit].at_val.at_long))
        {
        if ((EMsg != NULL) && (EMsg[0] == '\0'))
          strcpy(EMsg, "cannot locate feasible nodes (nodes file is empty or requested nodes exceed system)");
        
        comp_resc_lt++;
        }
      }    
    }

#ifndef CRAY_MOAB_PASSTHRU
  if ((proc_ct + req_procs) > svr_clnodes) 
    {
    if ((!(pque->qu_attr[QE_ATR_is_transit].at_flags & ATR_VFLAG_SET)) ||
        (!pque->qu_attr[QE_ATR_is_transit].at_val.at_long))
      {
      if ((EMsg != NULL) && (EMsg[0] == '\0'))
        strcpy(EMsg, "cannot locate feasible nodes (nodes file is empty or requested nodes exceed all systems)");
      
      comp_resc_lt++;
      }
    }
#endif

  if (MPPWidth > 0)
    {
    /* NYI */

    if (PPN == 0)
      {
      if (EMsg != NULL)
        strcpy(EMsg, "MPP Width Detected");
      }
    }

  return(comp_resc_lt);
  }  /* END chk_svr_resc_limit() */





/*
 * count_queued_jobs
 *
 * @param pque - the queue we're counting in
 * @param user - the user who we're counting for, or NULL if all users
 *
 * @return the number of jobs, or -1 on error
 */

static int count_queued_jobs(

  pbs_queue *pque, /* I */
  char      *user) /* I */

  {
  int num_jobs = 0;

  if (user == NULL)
    num_jobs = pque->qu_numjobs;
  else
    num_jobs = get_num_queued(pque->qu_uih, user);

  return(num_jobs);
  } /* END count_queued_jobs */





/*
 * chk_resc_limits - check job Resource_Limits pbs_attribute against the queue
 * and server maximum and mininum values.
 *
 * return 0 if within limits, or an pbs_error number if not.
 */

int chk_resc_limits(

  pbs_attribute *pattr,  /* I */
  pbs_queue     *pque,   /* I */
  char          *EMsg)   /* O (optional,minsize=1024) */

  {
  int resc_lt;
  int resc_gt;

  if (pattr == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input pbs_attribute pointer");
    return(PBSE_BAD_PARAMETER);
    }
  if (pque == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input pbs_queue pointer");
    return(PBSE_BAD_PARAMETER);
    }

  if (EMsg != NULL)
    EMsg[0] = '\0';

  /* first check against queue minimum */
  pthread_mutex_lock(server.sv_attr_mutex);
  resc_gt = comp_resc2(&pque->qu_attr[QA_ATR_ResourceMin],
         pattr,
         server.sv_attr[SRV_ATR_QCQLimits].at_val.at_long,
         EMsg,
         GREATER);
  pthread_mutex_unlock(server.sv_attr_mutex);

  if (resc_gt != PBSE_NONE)
    {
    if ((EMsg != NULL) && (EMsg[0] == '\0'))
      strcpy(EMsg, "job violates queue min resource limits");

    return(PBSE_EXCQRESC);
    }

  /* now check against queue or server maximum */
  resc_lt = chk_svr_resc_limit(pattr,pque,pque->qu_qs.qu_type,EMsg);

  if (resc_lt > 0)
    {
    if ((EMsg != NULL) && (EMsg[0] == '\0'))
      strcpy(EMsg, "job violates queue/server max resource limits");

    return(PBSE_EXCQRESC);
    }

  /* SUCCESS */
  return(PBSE_NONE);
  }  /* END chk_resc_limits() */



static int check_execution_uid_and_gid(

    struct job *const pjob,
    char       *const EMsg)
  {
  int return_code = PBSE_NONE; /* Optimistic assumption */

  if (!(pjob->ji_wattr[JOB_ATR_euser].at_flags & ATR_VFLAG_SET) ||
      !(pjob->ji_wattr[JOB_ATR_egroup].at_flags & ATR_VFLAG_SET))
    {
    return_code = set_jobexid(pjob, pjob->ji_wattr, EMsg); /* PBSE_BADUSER or GRP */
    }

  return(return_code);
  }




static int check_queue_disallowed_types(

    struct job       *const pjob,
    struct pbs_queue *const pque,
    char             *const EMsg)
  {
  int return_code = PBSE_NONE; /* Optimistic assumption */
  int i = 0;

  if (pque->qu_attr[QA_ATR_DisallowedTypes].at_flags & ATR_VFLAG_SET)
    {
    for (i = 0;
         i < (pque->qu_attr[QA_ATR_DisallowedTypes]).at_val.at_arst->as_usedptr;
         i++)
      {
      /* if job is interactive...*/

      if ((pjob->ji_wattr[JOB_ATR_interactive].at_flags & ATR_VFLAG_SET) &&
          (pjob->ji_wattr[JOB_ATR_interactive].at_val.at_long > 0))
        {
        if (pque->qu_attr[QA_ATR_DisallowedTypes].at_val.at_arst != NULL)
          {
          if (strcmp(Q_DT_interactive,
                     pque->qu_attr[QA_ATR_DisallowedTypes].at_val.at_arst->as_string[i]) == 0)
            {
            if (EMsg)
              snprintf(EMsg, 1024,
                "interactive job is not allowed for queue: user %s, queue %s",
                pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
                pque->qu_qs.qu_name);

            return(PBSE_NOINTERACTIVE);
            }
          }
        }
      else /* else job is batch... */
        {
        if (strcmp(Q_DT_batch,
                   pque->qu_attr[QA_ATR_DisallowedTypes].at_val.at_arst->as_string[i]) == 0)
          {
          if (EMsg)
            snprintf(EMsg, 1024,
              "batch job is not allowed for queue: user %s, queue %s",
              pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
              pque->qu_qs.qu_name);

          return(PBSE_NOBATCH);
          }
        }

      if (strcmp(Q_DT_rerunable,
                 pque->qu_attr[QA_ATR_DisallowedTypes].at_val.at_arst->as_string[i]) == 0
          && (pjob->ji_wattr[JOB_ATR_rerunable].at_flags & ATR_VFLAG_SET &&
              pjob->ji_wattr[JOB_ATR_rerunable].at_val.at_long > 0))
        {
        if (EMsg)
          snprintf(EMsg, 1024,
            "rerunable job is not allowed for queue: user %s, queue %s",
            pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
            pque->qu_qs.qu_name);

        return(PBSE_NORERUNABLE);
        }

      if (strcmp(Q_DT_nonrerunable,
                 pque->qu_attr[QA_ATR_DisallowedTypes].at_val.at_arst->as_string[i]) == 0
          && (!(pjob->ji_wattr[JOB_ATR_rerunable].at_flags & ATR_VFLAG_SET) ||
              pjob->ji_wattr[JOB_ATR_rerunable].at_val.at_long == 0))
        {
        if (EMsg)
          snprintf(EMsg, 1024,
            "only rerunable jobs are allowed for queue: user %s, queue %s",
            pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
            pque->qu_qs.qu_name);

        return(PBSE_NONONRERUNABLE);
        }
      if (strcmp(Q_DT_fault_tolerant,
                 pque->qu_attr[QA_ATR_DisallowedTypes].at_val.at_arst->as_string[i]) == 0
          && ((pjob->ji_wattr[JOB_ATR_fault_tolerant].at_flags & ATR_VFLAG_SET) &&
              pjob->ji_wattr[JOB_ATR_fault_tolerant].at_val.at_long != 0))
        {
        if (EMsg)
          snprintf(EMsg, 1024,
            "fault_tolerant jobs are not allowed for queue: user %s, queue %s",
            pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
            pque->qu_qs.qu_name);

        return(PBSE_NOFAULTTOLERANT);
        }

      if (strcmp(Q_DT_fault_intolerant,
                 pque->qu_attr[QA_ATR_DisallowedTypes].at_val.at_arst->as_string[i]) == 0
          && (!(pjob->ji_wattr[JOB_ATR_fault_tolerant].at_flags & ATR_VFLAG_SET) ||
              pjob->ji_wattr[JOB_ATR_fault_tolerant].at_val.at_long == 0))
        {
        if (EMsg)
          snprintf(EMsg, 1024,
            "only fault_tolerant jobs are allowed for queue: user %s, queue %s",
            pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
            pque->qu_qs.qu_name);

        return(PBSE_NOFAULTINTOLERANT);
        }
      if (strcmp(Q_DT_job_array,
                 pque->qu_attr[QA_ATR_DisallowedTypes].at_val.at_arst->as_string[i]) == 0
          && (pjob->ji_wattr[JOB_ATR_job_array_request].at_flags & ATR_VFLAG_SET))
        {
        if (EMsg)
          snprintf(EMsg, 1024,
            "job arrays are not allowed for queue: queue %s",
            pque->qu_qs.qu_name);
        return(PBSE_NOJOBARRAYS);
        }

      }
    }

  return(return_code);
  }

/* Special return constant for check_queue_group_ACL function */
static const int FAILED_GROUP_ACL = -1;
static int check_queue_group_ACL(

    struct job       *const pjob,
    struct pbs_queue *const pque,
    const int               mtype, /* MOVE_TYPE_* type, see server_limits.h */
    char             *const EMsg)
  {
  int return_code = PBSE_NONE; /* Optimistic assumption */

  if (pque->qu_attr[QA_ATR_AclGroupEnabled].at_val.at_long)
    {
    int rc;
    int slpygrp;
    struct array_strings *pas;

    pthread_mutex_lock(server.sv_attr_mutex);
    slpygrp = attr_ifelse_long(
      &pque->qu_attr[QA_ATR_AclGroupSloppy],
      &server.sv_attr[SRV_ATR_AclGroupSloppy],
      0);
    pthread_mutex_unlock(server.sv_attr_mutex);

    rc = acl_check(
           &pque->qu_attr[QA_ATR_AclGroup],
           pjob->ji_wattr[JOB_ATR_egroup].at_val.at_str,
           ACL_Group);

    if ((rc == 0) && slpygrp)
      {
      /* try again looking at the gids */

      rc = acl_check(
             &pque->qu_attr[QA_ATR_AclGroup],
             pjob->ji_wattr[JOB_ATR_egroup].at_val.at_str,
             ACL_Gid);
      }

    if ((rc == 0) && slpygrp &&
        (!(pjob->ji_wattr[JOB_ATR_grouplst].at_flags & ATR_VFLAG_SET)))
      {
      /* check group acl against all accessible groups */

      struct group *grp;
      int i = 0;
      int j = 0;

      char uname[PBS_MAXUSER + 1];

      snprintf(uname, sizeof(uname), "%s", pjob->ji_wattr[JOB_ATR_euser].at_val.at_str);

      /* fetch the groups in the ACL and look for matching user membership */

      pas = pque->qu_attr[QA_ATR_AclGroup].at_val.at_arst;

      for (i = 0; pas != NULL && i < pas->as_usedptr;i++)
        {
        if ((grp = getgrnam(pas->as_string[i])) == NULL)
          continue;

        for (j = 0;grp->gr_mem[j] != NULL;j++)
          {
          if (!strcmp(grp->gr_mem[j], uname))
            {
            rc = 1;

            break;
            }
          }

        if (rc == 1)
          break;
        }
      }    /* END if (rc == 0) && slpygrp && ...) */

    if (rc == 0)
      {
      /* ACL not satisfied */

      if (mtype != MOVE_TYPE_MgrMv) /* ok if mgr */
        {
        int logic_or;

        pthread_mutex_lock(server.sv_attr_mutex);
        logic_or = attr_ifelse_long(&pque->qu_attr[QA_ATR_AclLogic],
                                    &server.sv_attr[SRV_ATR_AclLogic],
                                    0);
        pthread_mutex_unlock(server.sv_attr_mutex);

        if (logic_or && pque->qu_attr[QA_ATR_AclUserEnabled].at_val.at_long)
          {
          /* only fail if neither user nor group acls can be met */

          return(FAILED_GROUP_ACL);
          }
        else
          {
          /* no user acl, fail immediately */
          if (EMsg) snprintf(EMsg, 1024,
                               "group ACL is not satisfied: user %s, queue %s",
                               pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
                               pque->qu_qs.qu_name);

          return(PBSE_PERM);
          }
        }
      }
    }    /* END if (pque->qu_attr[QA_ATR_AclGroupEnabled].at_val.at_long) */

  return(return_code);
  }

static int check_queue_host_ACL(

    struct job       *const pjob,
    struct pbs_queue *const pque,
    char             *const hostname,
    char             *const EMsg)
  {
  int return_code = PBSE_NONE; /* Optimistic assumption */

  if (pque->qu_attr[QA_ATR_AclHostEnabled].at_val.at_long)
    {
    if (acl_check(&pque->qu_attr[QA_ATR_AclHost], hostname, ACL_Host) == 0)
      {
      if (EMsg)
        snprintf(EMsg, 1024,
          "host ACL rejected the submitting host: user %s, queue %s, host %s",
          pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
          pque->qu_qs.qu_name,
          hostname);

      return(PBSE_BADHOST);
      }
    }

  return(return_code);
  }

/* Special return constant for check_queue_group_ACL function */
static const int FAILED_USER_ACL = -2;
static int check_queue_user_ACL(

    struct job       *const pjob,
    struct pbs_queue *const pque,
    char             *const EMsg)
  {
  int return_code = PBSE_NONE; /* Optimistic assumption */

  if (pque->qu_attr[QA_ATR_AclUserEnabled].at_val.at_long)
    {
    if (acl_check(
          &pque->qu_attr[QA_ATR_AclUsers],
          pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
          ACL_User) == 0)
      {
      int logic_or;

      pthread_mutex_lock(server.sv_attr_mutex);
      logic_or = attr_ifelse_long(&pque->qu_attr[QA_ATR_AclLogic],
                                  &server.sv_attr[SRV_ATR_AclLogic],
                                  0);
      pthread_mutex_unlock(server.sv_attr_mutex);

      if (logic_or && pque->qu_attr[QA_ATR_AclGroupEnabled].at_val.at_long)
        {
        /* only fail if neither user nor group acls can be met */

        return(FAILED_USER_ACL);
        }
      else
        {
        /* no group acl, fail immediately */
        if (EMsg)
          snprintf(EMsg, 1024,
            "user ACL rejected the submitting user: user %s, queue %s",
            pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
            pque->qu_qs.qu_name);

        return(PBSE_PERM);
        }
      }
    }

  return(return_code);
  }

static int check_queue_job_limit(

    struct job       *const pjob,
    struct pbs_queue *const pque,
    char             *const EMsg)
  {
  int return_code = PBSE_NONE; /* Optimistic assumption */
  int array_jobs = 0;

  /* set the number of array jobs in this request if applicable */
  if (pjob->ji_is_array_template)
    {
    array_jobs = num_array_jobs(
        pjob->ji_wattr[JOB_ATR_job_array_request].at_val.at_str);

    /* only add if there wasn't an error. if there is, fail elsewhere */
    if (array_jobs > 0)
      {
      /* when not an array, user_jobs is the current number of jobs, not
       * the number of jobs that will be added. For this reason, the
       * comparison below is >= and this needs to be decremented by 1 */
      array_jobs--;
      }
    else
      {
      array_jobs = 0;
      }
    }

  if ((pque->qu_attr[QA_ATR_MaxJobs].at_flags & ATR_VFLAG_SET))
    {
    int total_jobs = 0;
    unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
    total_jobs = count_queued_jobs(pque, NULL);

    lock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
    if (pjob->ji_being_recycled == TRUE)
      {
      unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
      return(PBSE_JOB_RECYCLED);
      }

    if ((total_jobs + array_jobs) >= pque->qu_attr[QA_ATR_MaxJobs].at_val.at_long)
      {
      if (EMsg)
        {
        snprintf(EMsg, 1024,
          "total number of jobs in queue exceeds the queue limit: "
          "user %s, queue %s",
          pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
          pque->qu_qs.qu_name);
        }

      return(PBSE_MAXQUED);
      }
    }

  if ((pque->qu_attr[QA_ATR_MaxUserJobs].at_flags & ATR_VFLAG_SET) &&
      (pque->qu_attr[QA_ATR_MaxUserJobs].at_val.at_long >= 0))
    {
    int user_jobs = 0;

    /* count number of jobs user has in queue */
    unlock_ji_mutex(pjob, __func__, NULL, LOGLEVEL);

    user_jobs = count_queued_jobs(pque,
        pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str);

    lock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
    if (pjob->ji_being_recycled == TRUE)
      {
      unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
      return(PBSE_JOB_RECYCLED);
      }

    if ((user_jobs + array_jobs) >= pque->qu_attr[QA_ATR_MaxUserJobs].at_val.at_long)
      {
      if (EMsg)
        snprintf(EMsg, 1024,
          "total number of current user's jobs exceeds the queue limit: "
          "user %s, queue %s",
          pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
          pque->qu_qs.qu_name);

      return(PBSE_MAXUSERQUED);
      }
    }

  return(return_code);
  }

static int check_queue_enabled(

    struct job       *const pjob,
    struct pbs_queue *const pque,
    char             *const EMsg)
  {
  int return_code = PBSE_NONE; /* Optimistic assumption */

  if (pque->qu_attr[QA_ATR_Enabled].at_val.at_long == 0)
    {
    if (EMsg)
      {
      snprintf(EMsg, 1024,
        "queue is disabled: user %s, queue %s",
        pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
        pque->qu_qs.qu_name);
      }

    return(PBSE_QUNOENB);
    }

  return(return_code);
  }

static int check_local_route(

    struct job       *const pjob,
    struct pbs_queue *const pque,
    const int               mtype, /* MOVE_TYPE_* type, see server_limits.h */
    char             *const EMsg)
  {
  int return_code = PBSE_NONE; /* Optimistic assumption */

  /* if "from_route_only" is true, only local route allowed */
  if ((pque->qu_attr[QA_ATR_FromRouteOnly].at_flags & ATR_VFLAG_SET) &&
      (pque->qu_attr[QA_ATR_FromRouteOnly].at_val.at_long == 1))
    {
    if (mtype == MOVE_TYPE_Move)  /* ok if not plain user */
      {
      if (EMsg)
        snprintf(EMsg, 1024,
          "queue accepts only routed jobs, no direct submission: "
          "user %s, queue %s",
          pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
          pque->qu_qs.qu_name);

      return(PBSE_QACESS);
      }
    }

  return(return_code);
  }

static int are_job_resources_in_limits_of_queue(

    struct job       *const pjob,
    struct pbs_queue *const pque,
    char             *const EMsg)
  {
  int return_code = PBSE_NONE; /* Optimistic assumption */
  int check_limits = chk_resc_limits(&pjob->ji_wattr[JOB_ATR_resource], pque, EMsg);

  if (check_limits != 0)
    {
    /* FAILURE */
    return(check_limits);
    }
  else
    {
    if (pque->qu_attr->at_val.at_str == NULL)
      {
      log_err(PBSE_BAD_PARAMETER, __func__, "pque->qu_attr->at_val.at_str uninitialized");
      return PBSE_BAD_PARAMETER;
      }
    if (strcmp(pque->qu_attr->at_val.at_str, "Execution") == 0)
      {
      /* job routed to Execution queue successfully */
      /* unset job's procct resource */
      resource_def *pctdef;
      resource *pctresc;
      pctdef = find_resc_def(svr_resc_def, "procct", svr_resc_size);
      if ((pctresc = find_resc_entry(&pjob->ji_wattr[JOB_ATR_resource], pctdef)) != NULL)
         pctdef->rs_free(&pctresc->rs_value);
      }
    }

  return(return_code);
  }

/*
 * svr_chkque - check if job can enter a queue
 *
 * returns 0 for yes, 1 for no
 *
 * NOTE: the following fields must be set in the job struture before
 *   calling svr_chkque():  ji_wattr[JOB_ATR_job_owner]
 * NOTE: calls chk_resc_limits() to validate resource request
 *
 * Also if the job is being considered for an execution queue, then
 * set_jobexid() will be called.
 */

int svr_chkque(

  job       *pjob,
  pbs_queue *pque,
  char      *hostname,
  int        mtype,     /* MOVE_TYPE_* type, see server_limits.h */
  char      *EMsg)      /* O (optional,minsize=1024) */

  {
  int failed_group_acl = 0;
  int failed_host_acl = 0;
  int failed_user_acl  = 0;

  if (pjob == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input job pointer");
    return PBSE_BAD_PARAMETER;
    }
  if (pque == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input pbs_queue pointer");
    return PBSE_BAD_PARAMETER;
    }

  if (EMsg != NULL)
    EMsg[0] = '\0';


  /*
   * 1. If the queue is an Execution queue ...
   *    This is checked first because 1a - 1c are more damaging
   *    (see local_move() in svr_movejob.c)
   */
  if (pque->qu_qs.qu_type == QTYPE_Execution)
    {
    /* 1a. if not already set, set up execution uid/gid/name */
    int can_be_established = 0;
    int are_allowed = 0;

    can_be_established = check_execution_uid_and_gid(pjob, EMsg);
    if (can_be_established != PBSE_NONE)
        {
        return(can_be_established);
        }

    /* 1b. check site restrictions -- Currently site_acl_check is a stub */
    if (site_acl_check(pjob, pque))
      {
      if (EMsg) 
        snprintf(EMsg, 1024,
          "site_acl_check() failed: user %s, queue %s",
          pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
          pque->qu_qs.qu_name);

      return(PBSE_PERM);
      }

    /* 1c. cannot have an unknown resource */
    if (find_resc_entry(
          &pjob->ji_wattr[JOB_ATR_resource],
          &svr_resc_def[svr_resc_size - 1]) != 0)
      {
      if (EMsg) 
        {
        snprintf(EMsg, 1024,
          "detected request for an unknown resource");
        }

      return(PBSE_UNKRESC);
      }

    /* 1d. cannot have an unknown pbs_attribute */
    if (pjob->ji_wattr[JOB_ATR_UNKN].at_flags & ATR_VFLAG_SET)
      {
      if (EMsg) 
        snprintf(EMsg, 1024,
          "detected presence of an unknown attribute");

      return(PBSE_NOATTR);
      }

    /* 1e. check queue's disallowed_types */
    are_allowed = check_queue_disallowed_types(pjob, pque, EMsg);
    if (are_allowed != PBSE_NONE)
      {
      return(are_allowed);
      }

    /* 1f. if enabled, check the queue's group ACL */
    failed_group_acl = check_queue_group_ACL(pjob, pque, mtype, EMsg);
    if (failed_group_acl == PBSE_PERM)
      {
      return(failed_group_acl);
      }
    }      /* END if (pque->qu_qs.qu_type == QTYPE_Execution) */

  /* checks 2 and 3 are bypassed for a move by manager or qorder */
  if ((mtype != MOVE_TYPE_MgrMv) && (mtype != MOVE_TYPE_Order))
    {
    int queue_job_limit = 0;
    int queue_enabled = 0;
    int local_route = 0;

    /* 2. the queue must be enabled and the job limit not exceeded */
    queue_enabled = check_queue_enabled(pjob, pque, EMsg);
    if (queue_enabled != PBSE_NONE)
      {
      return(queue_enabled);
      }

    queue_job_limit = check_queue_job_limit(pjob, pque, EMsg);
    if (queue_job_limit != PBSE_NONE)
      {
      return(queue_job_limit);
      }

    /* 3. if "from_route_only" is true, only local route allowed */
    local_route = check_local_route(pjob, pque, mtype, EMsg);
    if (local_route != PBSE_NONE)
      {
      return(local_route);
      }
    }

  /* checks 4, 5, and 6 are bypassed for a move by manager */

  if (mtype != MOVE_TYPE_MgrMv)
    {
    int job_resources_in_limits_of_queue = 0;

    /* 4. if enabled, check the queue's host ACL */
    failed_host_acl = check_queue_host_ACL(pjob, pque, hostname, EMsg);
    if (failed_host_acl != PBSE_NONE)
      {
      return(failed_host_acl);
      }

    /* 5. if enabled, check the queue's user ACL */
    failed_user_acl = check_queue_user_ACL(pjob, pque, EMsg);
    if (failed_user_acl == PBSE_PERM)
      {
      return(failed_user_acl);
      }

    /* 5.5. if failed user and group acls, fail */

    if (failed_group_acl && failed_user_acl)
      {
      if (EMsg) snprintf(EMsg, 1024,
                           "both user and group ACL rejected the submitting user: "
                           "user %s, queue %s",
                           pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str,
                           pque->qu_qs.qu_name);

      return(PBSE_PERM);
      }

    /* 6. resources of the job must be in the limits of the queue */
    job_resources_in_limits_of_queue = are_job_resources_in_limits_of_queue(pjob, pque, EMsg);
    if (job_resources_in_limits_of_queue != PBSE_NONE)
      {
      return(job_resources_in_limits_of_queue);
      }

    }    /* END if (mtype != MOVE_TYPE_MgrMv) */

  /* SUCCESS - job can enter queue */

  return(PBSE_NONE);
  }  /* END svr_chkque() */





/*
 * job_wait_over - The execution wait time of a job has been reached, at
 * least according to a work_task entry via which we were invoked.
 *
 * If indeed the case, re-evaluate and set the job state.
 */

void job_wait_over(

  struct work_task *pwt)

  {
  int          newstate;
  int          newsub;
  job         *pjob;
  char         log_buf[LOCAL_LOG_BUF_SIZE];
  char        *jobid = (char *)pwt->wt_parm1;
 
  free(pwt->wt_mutex);
  free(pwt);

  if (jobid == NULL)
    {
    log_err(ENOMEM, __func__, "Cannot allocate memory");
    return;
    }

  pjob = svr_find_job(jobid, TRUE);
  free(jobid);

  if (pjob != NULL)
    {
    mutex_mgr job_mutex(pjob->ji_mutex, true);

#ifndef NDEBUG
      {
      time_t now = time((time_t *)0);
      time_t when = ((job *)pjob)->ji_wattr[JOB_ATR_exectime].at_val.at_long;
      
      if (when > now)
        {
        sprintf(log_buf, msg_badwait, ((job *)pjob)->ji_qs.ji_jobid);
        
        log_err(-1, __func__, log_buf);
        
        /* recreate the work task entry */
        set_task(WORK_Timed, when, job_wait_over, strdup(pjob->ji_qs.ji_jobid), FALSE);
        
        return;
        }
      }
#endif  /* !NDEBUG */

    pjob->ji_qs.ji_svrflags &= ~JOB_SVFLG_HASWAIT;
    
    /* clear the exectime pbs_attribute */
    job_attr_def[JOB_ATR_exectime].at_free(
      &pjob->ji_wattr[JOB_ATR_exectime]);
    
    pjob->ji_modified = 1;
    
    svr_evaljobstate(pjob, &newstate, &newsub, 0);
    svr_setjobstate(pjob, newstate, newsub, FALSE);
    }

  return;
  } /* END job_wait_over() */




/*
 * job_set_wait - set up a work task entry that will trigger at the execution
 * wait time of the job.
 *
 * This is called as the at_action (see attribute.h) function associated
 * with the execution-time job pbs_attribute.
 */

int job_set_wait(

  pbs_attribute *pattr,
  void          *j,    /* a (job *) cast to void * */
  int            mode) /* unused, do it for all action modes */

  {
  long       when;
  job       *pjob = (job *)j;

  if (pattr == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input pbs_attribute pointer");
    return PBSE_BAD_PARAMETER;
    }
  if (pjob == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input job pointer");
    return PBSE_BAD_PARAMETER;
    }

  if ((pattr->at_flags & ATR_VFLAG_SET) == 0)
    {
    return(PBSE_NONE);
    }

  when  = pattr->at_val.at_long;

  if (set_task(WORK_Timed, when, job_wait_over, strdup(pjob->ji_qs.ji_jobid), FALSE) == NULL)
    {
    return(-1);
    }

  /* set JOB_SVFLG_HASWAIT to show job has work task entry */
  pjob->ji_qs.ji_svrflags |= JOB_SVFLG_HASWAIT;

  return(PBSE_NONE);
  }  /* END job_set_wait() */




/*
 * default_std - make the default name for standard output or error
 * "job_name".[e|o]job_sequence_number
 */

static void default_std(

  job            *pjob,
  int             key,  /* 'e' for stderr, 'o' for stdout */
  dynamic_string *ds)   /* O */

  {
  char          *pd;
  unsigned long  job_id_num;
  /* allow for very large job ids */
  char           job_num_buf[100];

  pd = strrchr(pjob->ji_wattr[JOB_ATR_jobname].at_val.at_str, '/');

  if (pd != NULL)
    ++pd;
  else
    pd = pjob->ji_wattr[JOB_ATR_jobname].at_val.at_str;

  /* start with the job name */
  append_dynamic_string(ds, pd);
  append_char_to_dynamic_string(ds, '.');
  append_char_to_dynamic_string(ds, (char)key);

  job_id_num = strtol(pjob->ji_qs.ji_jobid, NULL, 10);
  sprintf(job_num_buf, "%lu", job_id_num);

  append_dynamic_string(ds, job_num_buf);

  if ((pd = strchr(pjob->ji_qs.ji_jobid, '[')) != NULL)
    {
    pd++;

    if (isdigit(*pd))
      {
      append_char_to_dynamic_string(ds, '-');

      job_id_num = strtol(pd, NULL, 10);
      sprintf(job_num_buf, "%lu", job_id_num);
      append_dynamic_string(ds, job_num_buf);
      }
    }

  return;
  }  /* END default_std() */





/*
 * prefix_std_file - build the fully prefixed default file name for the job's
 * standard output or error:
 *  qsub_host:$PBS_O_WORKDIR/job_name.[e|o]job_sequence_number
 */

char *prefix_std_file(

  job            *pjob,
  dynamic_string *ds,
  int             key)

  {
  int   len;
  char *qsubhost;
  char *wdir;

  if (pjob == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input job pointer");
    return(NULL);
    }
  if (ds == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input dynamic_string pointer");
    return(NULL);
    }

  qsubhost = get_variable(pjob, pbs_o_host);
  wdir     = get_variable(pjob, "PBS_O_WORKDIR");

  if (qsubhost != NULL)
    {

    /* If just the host name portion was specified
     * then we use it instead of qsubhost
     */
    
    if ((key == (int)'e') &&
        (pjob->ji_wattr[JOB_ATR_errpath].at_flags & ATR_VFLAG_SET) &&
        (pjob->ji_wattr[JOB_ATR_errpath].at_val.at_str[strlen(pjob->ji_wattr[JOB_ATR_errpath].at_val.at_str) - 1] == ':'))
      {
      
      pjob->ji_wattr[JOB_ATR_errpath].at_val.at_str[strlen(pjob->ji_wattr[JOB_ATR_errpath].at_val.at_str) - 1] = '\0';
      qsubhost = pjob->ji_wattr[JOB_ATR_errpath].at_val.at_str;
      
      }
    else if ((key == (int)'o') && 
        (pjob->ji_wattr[JOB_ATR_outpath].at_flags & ATR_VFLAG_SET) &&
        (pjob->ji_wattr[JOB_ATR_outpath].at_val.at_str[strlen(pjob->ji_wattr[JOB_ATR_outpath].at_val.at_str) - 1] == ':'))
      {
      
      pjob->ji_wattr[JOB_ATR_outpath].at_val.at_str[strlen(pjob->ji_wattr[JOB_ATR_outpath].at_val.at_str) - 1] = '\0';
      qsubhost = pjob->ji_wattr[JOB_ATR_outpath].at_val.at_str;
      
      }

    len = strlen(qsubhost) +
          strlen(pjob->ji_wattr[JOB_ATR_jobname].at_val.at_str) +
          PBS_MAXSEQNUM +
          5;

    if (wdir)
      len += strlen(wdir);

    append_dynamic_string(ds, qsubhost);
    append_char_to_dynamic_string(ds, ':');

    if (wdir)
      {
      append_dynamic_string(ds, wdir);
      append_char_to_dynamic_string(ds, '/');
      }

    /* now add the rest */
    default_std(pjob, key, ds);
    }

  return(ds->str);
  } /* END prefix_std_file() */




/*
 * add_std_filename - add the default file name for the job's
 * standard output or error:
 *  job_name.[e|o]job_sequence_number
 */

char *add_std_filename(

  job            *pjob,
  char           *path,
  int             key,
  dynamic_string *ds)

  {
  if (pjob == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input job pointer");
    return(NULL);
    }
  if (path == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input path pointer");
    return(NULL);
    }
  if (ds == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input dynamic_string pointer");
    return(NULL);
    }

  append_dynamic_string(ds, path);
  append_char_to_dynamic_string(ds, '/');
  
  /* now add the rest */
  default_std(pjob, key, ds);

  return(ds->str);
  } /* add_std_filename() */




/*
 * get_jobowner - copy the basic job owner's name, without the @host suffix.
 * The "to" buffer must be large enought (PBS_MAXUSER+1).
 */

void get_jobowner(

  char *from,
  char *to)

  {
  int i;

  if (to == NULL)
    return;

  if (from == NULL)
    {
    *to = 0;
    return;
    }


  for (i = 0;i < PBS_MAXUSER;++i)
    {
    if ((*(from + i) == '@') || (*(from + i) == '\0'))
      break;

    *(to + i) = *(from + i);
    }

  *(to + i) = '\0';

  return;
  }





/**
 * @see set_resc_deflt() - parent
 *
 * @param jb
 * @param *dflt
 */

static void set_deflt_resc(

  pbs_attribute *jb,
  pbs_attribute *dflt)

  {
  resource *prescjb;
  resource *prescdt;

  if (dflt->at_flags & ATR_VFLAG_SET)
    {
    /* for each resource in the default value list */

    prescdt = (resource *)GET_NEXT(dflt->at_val.at_list);

    while (prescdt != NULL)
      {
      if (prescdt->rs_value.at_flags & ATR_VFLAG_SET)
        {
        /* see if the job already has that resource */

        prescjb = find_resc_entry(jb, prescdt->rs_defin);

        if ((prescjb == NULL) ||
            ((prescjb->rs_value.at_flags & ATR_VFLAG_SET) == 0))
          {
          /* resource does not exist or value is not set */

          if (prescjb == NULL)
            prescjb = add_resource_entry(jb, prescdt->rs_defin);

          if (prescjb != NULL)
            {
            if (prescdt->rs_defin->rs_set(
                  &prescjb->rs_value,
                  &prescdt->rs_value,
                  SET) == 0)
              {
              prescjb->rs_value.at_flags |= (ATR_VFLAG_SET | ATR_VFLAG_DEFLT | ATR_VFLAG_MODIFY);
              }
            }
          }
        }

      prescdt = (resource *)GET_NEXT(prescdt->rs_link);
      }
    }

  return;
  }  /* END set_deflt_resc() */






/**
 * Set job defaults
 *
 * @see svr_enquejob() - parent
 * @see set_deflt_resc() - child
 *
 * @param pjob (I) [modified]
 * @param ji_wattr
 *
 * NOTE:  if ji_wattr parameter is passed in, update it instead of pjob
 */

void set_resc_deflt(

  job           *pjob,            /* I (modified) */
  pbs_attribute *ji_wattr,        /* I (optional) decoded attributes  */
  int            has_queue_mutex) /* I */

  {
  pbs_attribute *ja;

  pbs_queue     *pque;
  attribute_def *pdef;
  int            i;
  int            rc;

  if (ji_wattr != NULL)
    ja = &ji_wattr[JOB_ATR_resource];
  else
    ja = &pjob->ji_wattr[JOB_ATR_resource];


  if (LOGLEVEL >= 10)
    log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_JOB, __func__, pjob->ji_qs.ji_jobid);

  if (has_queue_mutex == FALSE)
    {
    pque = get_jobs_queue(&pjob);
    if (pjob == NULL)
      {
      log_err(PBSE_JOBNOTFOUND, __func__, "Job lost while acquiring queue 12");
      return;
      }
    }
  else
    pque = pjob->ji_qhdr;

  /* apply queue defaults first since they take precedence */
  if (pque != NULL)
    {
    set_deflt_resc(ja, &pque->qu_attr[QA_ATR_ResourceDefault]);
    
    pthread_mutex_lock(server.sv_attr_mutex);
    /* server defaults will only be applied to attributes which have
       not yet been set */
    set_deflt_resc(ja, &server.sv_attr[SRV_ATR_resource_deflt]);
    
#ifdef RESOURCEMAXDEFAULT
    /* apply queue max limits first since they take precedence */
    set_deflt_resc(ja, &pque->qu_attr[QA_ATR_ResourceMax]);
    
    /* server max limits will only be applied to attributes which have
       not yet been set */
    set_deflt_resc(ja, &server.sv_attr[SRV_ATR_ResourceMax]);
#endif
    pthread_mutex_unlock(server.sv_attr_mutex);

    unlock_queue(pque, __func__, NULL, LOGLEVEL);
    }

  for (i = 0;i < JOB_ATR_LAST;++i)
    {
    pdef = &job_attr_def[i];

    if ((pjob->ji_wattr[i].at_flags & ATR_VFLAG_SET) && (pdef->at_action))
      {
      if ((rc = pdef->at_action(&pjob->ji_wattr[i], pjob, ATR_ACTION_NEW)) != 0)
        {
        /* FIXME: return an actual error */

        return;
        }
      }
    }
  
  if ((pque = get_jobs_queue(&pjob)) != NULL)
    {
    if (has_queue_mutex == FALSE)
      unlock_queue(pque, __func__, NULL, LOGLEVEL);
    }
  else if (pjob == NULL)
    {
    log_err(PBSE_JOBNOTFOUND, __func__, "Job lost while acquiring queue 13");
    }
 
  return;
  }  /* END set_resc_deflt() */




  /**
   * Set job defaults
   *
   * @see svr_enquejob() - parent
   * @see set_chkpt_resc() - child
   *
   * @param pjob (I) [modified]
   * @param pque (I)
   *
   */

void set_chkpt_deflt(

  job       *pjob,     /* I (modified) */
  pbs_queue *pque)     /* Input */

  {
  char log_buf[LOCAL_LOG_BUF_SIZE];
  
  if (pjob == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input job pointer");
    return;
    }

  if (pque == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input pbs_queue pointer");
    return;
    }
  
  /* If execution queue has checkpoint defaults specified, but job does not have
   * checkpoint values, then set defaults on the job.
   */
  
  if ((pque->qu_qs.qu_type == QTYPE_Execution) &&
      (pque->qu_attr[QE_ATR_checkpoint_defaults].at_flags & ATR_VFLAG_SET) &&
      (pque->qu_attr[QE_ATR_checkpoint_defaults].at_val.at_str))
    {
    if ((!(pjob->ji_wattr[JOB_ATR_checkpoint].at_flags & ATR_VFLAG_SET)) ||
        (csv_find_string(pjob->ji_wattr[JOB_ATR_checkpoint].at_val.at_str, "u") != NULL))
      {
      job_attr_def[JOB_ATR_checkpoint].at_set(
          &pjob->ji_wattr[JOB_ATR_checkpoint],
          &pque->qu_attr[QE_ATR_checkpoint_defaults],
          SET);
      
      if (LOGLEVEL >= 7)
        {
        sprintf(log_buf,"Applying queue (%s) checkpoint defaults (%s) to job",
          pque->qu_qs.qu_name,
          pque->qu_attr[QE_ATR_checkpoint_defaults].at_val.at_str);
        
        log_event(PBSEVENT_JOB,PBS_EVENTCLASS_JOB,pjob->ji_qs.ji_jobid,log_buf);
        }
      }
    }
  
  return;
  }  /* END set_chkpt_deflt() */





  /*
   * set_statechar - set the job state pbs_attribute to the letter that correspondes
   * to its current state.
   */

void set_statechar(

  job *pjob) /* *I* (modified) */

  {
  static const char *statechar = "TQHWREC";
  static const char suspend    = 'S';
  
  if (pjob == NULL)
  {
  log_err(PBSE_BAD_PARAMETER, __func__, "NULL input job pointer");
  return;
  }
  
  if ((pjob->ji_qs.ji_state == JOB_STATE_RUNNING) &&
      (pjob->ji_qs.ji_svrflags & JOB_SVFLG_Suspend))
    {
    pjob->ji_wattr[JOB_ATR_state].at_val.at_char = suspend;
    }
  else
    {
    if (pjob->ji_qs.ji_state < (int)strlen(statechar))
      {
      pjob->ji_wattr[JOB_ATR_state].at_val.at_char =
        *(statechar + pjob->ji_qs.ji_state);
      }
    else
      {
      pjob->ji_wattr[JOB_ATR_state].at_val.at_char = 'U'; /* Unknown */
      }
    }
  
  return;
  }  /* END set_statechar() */






/*
 * eval_checkpoint - if the job's checkpoint pbs_attribute is "c=nnnn" and
 *  nnnn is less than the queue' minimum checkpoint time, reset
 * to the queue min time.
 */

static void eval_checkpoint(
    
  pbs_attribute *jobckp, /* job's checkpoint pbs_attribute */
  pbs_attribute *queckp) /* queue's checkpoint pbs_attribute */

  {
  int   jobs;
  char  queues[30];
  char *pv;

  if (((jobckp->at_flags & ATR_VFLAG_SET) == 0) ||
      ((queckp->at_flags & ATR_VFLAG_SET) == 0))
    {
    return;  /* need do nothing */
    }
  
  pv = jobckp->at_val.at_str;
  
  if (*pv++ == 'c')
    {
    if (*pv == '=')
      pv++;
    
    jobs = atoi(pv);
    
    if (jobs < queckp->at_val.at_long)
      {
      sprintf(queues, "c=%ld", queckp->at_val.at_long);
      
      free_str(jobckp);

      decode_str(jobckp, 0, 0, queues, 0);
      }
    }
  
  return;
  }  /* END eval_checkpoint() */





#ifndef NDEBUG

/*
 * correct_ct - This is a work-around for an as yet unfound bug where
 * the counts of jobs in each state sometimes (rarely) become wrong.
 * When this happens, the count for a state can become negative.
 * If this is detected (see above), this routine is called to reset
 * all of the counts and log a message.
 */

static void correct_ct()
  
  {
  int           i;
  char         *pc;
  job          *pjob;
  pbs_queue    *pque;
  int           queue_iter = -1;
  int           job_iter = -1;
  int           num_jobs = 0;
  int           job_counts[PBS_NUMJOBSTATE];
  char          log_buf[LOCAL_LOG_BUF_SIZE];
  
  lock_startup();
  lock_sv_qs_mutex(server.sv_qs_mutex, __func__);
  sprintf(log_buf, "Job state counts incorrect, server %d: ", server.sv_qs.sv_numjobs);
  
  server.sv_qs.sv_numjobs = 0;
  unlock_sv_qs_mutex(server.sv_qs_mutex, __func__);
  
  pthread_mutex_lock(server.sv_jobstates_mutex);
  
  for (i = 0;i < PBS_NUMJOBSTATE;++i)
    {
    pc = log_buf + strlen(log_buf);
    
    sprintf(pc, "%d ", server.sv_jobstates[i]);
    
    job_counts[i] = 0;
    }
  
  pthread_mutex_unlock(server.sv_jobstates_mutex);
  
  log_event(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, msg_daemonname, log_buf);
  
  while ((pque = next_queue(&svr_queues,&queue_iter)) != NULL)
    {
    mutex_mgr pque_mutex = mutex_mgr(pque->qu_mutex, true);
    snprintf(log_buf, LOCAL_LOG_BUF_SIZE, "checking queue %s", pque->qu_qs.qu_name);
    log_event(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, msg_daemonname, log_buf);
    pque->qu_numjobs = 0;
    pque->qu_numcompleted = 0;
    
    for (i = 0;i < PBS_NUMJOBSTATE;++i)
      pque->qu_njstate[i] = 0;
    
    /* iterate over each job in the queue and count it.
     * This code used to iterate over each job, find the queue, and then count the job.
     * This had to be changed because the mutex order needs to lock the queue before the 
     * job, so in order to acquire a queue's mutex the job's mutex had to be released on 
     * occasion, meaning the job could be deleted by the time that thread had the job's 
     * lock again, since the mutex is released before being destroyed. This caused crashes 
     * along with other mayhem. Thus, keep the code here and only get jobs that really 
     * exist */
    job_iter = -1;
    
    while ((pjob = next_job(pque->qu_jobs, &job_iter)) != NULL)
      {
      num_jobs++;
      
      job_counts[pjob->ji_qs.ji_state]++;
      
      pque->qu_numjobs++;
      pque->qu_njstate[pjob->ji_qs.ji_state]++;
      
      if (pjob->ji_qs.ji_state == JOB_STATE_COMPLETE)
        pque->qu_numcompleted++;
      
      unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
      }
    
    } /* END for each queue */
  
  sprintf(log_buf, "%s:2", __func__);
  lock_sv_qs_mutex(server.sv_qs_mutex, log_buf);
  server.sv_qs.sv_numjobs = num_jobs;
  unlock_sv_qs_mutex(server.sv_qs_mutex, log_buf);
  
  pthread_mutex_lock(server.sv_jobstates_mutex);
  for (i = 0; i < PBS_NUMJOBSTATE; i++)
    server.sv_jobstates[i] = job_counts[i];
  pthread_mutex_unlock(server.sv_jobstates_mutex);
  
  unlock_startup();
  return;
  }  /* END correct_ct() */


int lock_ji_mutex(

  job        *pjob,
  const char *id,
  const char *msg,
  int        logging)

  {
  int rc = PBSE_NONE;
  char *err_msg = NULL;
  char stub_msg[] = "no pos";

  if (logging >= 10)
    {
    err_msg = (char *)calloc(1, MSG_LEN_LONG);
    if (msg == NULL)
      msg = stub_msg;
    snprintf(err_msg, MSG_LEN_LONG, "locking %s in method %s-%s", pjob->ji_qs.ji_jobid, id, msg);
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, id, err_msg);
    }

  if (pjob->ji_mutex != NULL)
    {
    if (pthread_mutex_lock(pjob->ji_mutex) != 0)
      {
      if (logging >= 20)
        {
        snprintf(err_msg, MSG_LEN_LONG, "ALERT: cannot lock job %s mutex in method %s",
                                     pjob->ji_qs.ji_jobid, id);
        log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, id, err_msg);
        }
      rc = PBSE_MUTEX;
      }
    }
  else
    {
    rc = -1;
    log_err(rc, __func__, "Uninitialized mutex pass to pthread_mutex_lock!");
    }

  if (err_msg != NULL)
  free(err_msg);

  return rc;
  }



int unlock_ji_mutex(

  job        *pjob,
  const char *id,
  const char *msg,
  int        logging)
  {
  int rc = PBSE_NONE;
  char *err_msg = NULL;
  char stub_msg[] = "no pos";

  if (logging >= 10)
    {
    err_msg = (char *)calloc(1, MSG_LEN_LONG);
    if (msg == NULL)
      msg = stub_msg;
    snprintf(err_msg, MSG_LEN_LONG, "unlocking %s in method %s-%s", pjob->ji_qs.ji_jobid, id, msg);
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, id, err_msg);
    }

  if (pjob->ji_mutex != NULL)
    {
    if (pthread_mutex_unlock(pjob->ji_mutex) != 0)
      {
    if (logging >= 20)
        {
        snprintf(err_msg, MSG_LEN_LONG, "ALERT: cannot unlock job %s mutex in method %s",
                                            pjob->ji_qs.ji_jobid, id);
        log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, id, err_msg);
        }
      rc = PBSE_MUTEX;
      }
    }
  else
    {
    rc = -1;
    log_err(rc, __func__, "Uninitialized mutex pass to pthread_mutex_unlock!");
    }

   if (err_msg != NULL)
     free(err_msg);

   return rc;
   }


int lock_ai_mutex(

  job_array  *pa,
  const char *id,
  const char *msg,
  int        logging)

  {
  int rc = PBSE_NONE;
  char *err_msg = NULL;
  char stub_msg[] = "no pos";

  if (logging >= 10)
    {
    err_msg = (char *)calloc(1, MSG_LEN_LONG);
    if (msg == NULL)
      msg = stub_msg;
    snprintf(err_msg, MSG_LEN_LONG, "locking %s in method %s-%s", pa->ai_qs.parent_id, id, msg);
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, id, err_msg);
    }

  if (pthread_mutex_lock(pa->ai_mutex) != 0)
    {
    if (logging >= 20)
      {
      snprintf(err_msg, MSG_LEN_LONG, "ALERT: cannot lock job array %s mutex in method %s",
                                   pa->ai_qs.parent_id, id);
      log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, id, err_msg);
      }
    rc = PBSE_MUTEX;
    }

  if (err_msg != NULL)
  free(err_msg);

  return rc;
  }


int unlock_ai_mutex(

  job_array  *pa,
  const char *id,
  const char *msg,
  int        logging)

  {
  int rc = PBSE_NONE;
  char *err_msg = NULL;
  char stub_msg[] = "no pos";

  if (logging >= 10)
    {
    err_msg = (char *)calloc(1, MSG_LEN_LONG);
    if (msg == NULL)
      msg = stub_msg;
    snprintf(err_msg, MSG_LEN_LONG, "unlocking %s in method %s-%s", pa->ai_qs.parent_id, id, msg);
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, id, err_msg);
    }

  if (pthread_mutex_unlock(pa->ai_mutex) != 0)
    {
    if (logging >= 20)
      {
      snprintf(err_msg, MSG_LEN_LONG, "ALERT: cannot unlock job array %s mutex in method %s",
                                   pa->ai_qs.parent_id, id);
      log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, id, err_msg);
      }
    rc = PBSE_MUTEX;
    }

  if (err_msg != NULL)
  free(err_msg);

  return rc;
  }

  
#endif  /* NDEBUG */
