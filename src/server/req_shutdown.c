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
 * req_shutdown.c - contains the functions to shutdown the server
 */
#include <pbs_config.h>   /* the master config generated by configure */

#include "libpbs.h"
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "server_limits.h"
#include "list_link.h"
#include "work_task.h"
#include "log.h"
#include "../lib/Liblog/pbs_log.h"
#include "../lib/Liblog/log_event.h"
#include "attribute.h"
#include "server.h"
#include "credential.h"
#include "batch_request.h"
#include "pbs_job.h"
#include "queue.h"
#include "pbs_error.h"
#include "svrfunc.h"
#include "csv.h"
#include "threadpool.h"
#include "svr_func.h" /* get_svr_attr_* */
#include "ji_mutex.h"



int          issue_signal(job **, char *, void(*)(batch_request *), void *);

/* Private Fuctions Local to this File */

static int shutdown_checkpoint(job **);
static void post_checkpoint(batch_request *preq);
void  rerun_or_kill(job **, char *text);

/* Private Data Items */

static struct batch_request *pshutdown_request = 0;

/* Global Data Items: */

extern char *msg_abort_on_shutdown;
extern char *msg_daemonname;
extern char *msg_init_queued;
extern char *msg_shutdown_op;
extern char *msg_shutdown_start;
extern char *msg_leftrunning;
extern char *msg_stillrunning;
extern char *msg_on_shutdown;
extern char *msg_job_abort;
extern int   lockfds;

extern struct server server;
extern attribute_def svr_attr_def[];
extern int    LOGLEVEL;
extern struct all_jobs alljobs;



/*
 * saves the queues to file
 */

void save_queues()

  {
  struct pbs_queue *pque;
  int               iter = -1;

  while ((pque = next_queue(&svr_queues, &iter)) != NULL)
    {
    que_save(pque);
    unlock_queue(pque, __func__, (char *)NULL, 0);
    }
  } /* END save_queues() */




/*
 * svr_shutdown() - Perform (or start of) the shutdown of the server
 */

void svr_shutdown(

  int type) /* I */

  {
  pbs_attribute *pattr;
  job           *pjob;
  long           state = SV_STATE_DOWN;
  int            iter;
  char           log_buf[LOCAL_LOG_BUF_SIZE];

  close(lockfds);

  save_queues();

  /* Lets start by logging shutdown and saving everything */
  get_svr_attr_l(SRV_ATR_State, &state);

  strcpy(log_buf, msg_shutdown_start);

  if (state == SV_STATE_SHUTIMM)
    {
    /* if already shuting down, another Immed/sig will force it */
    if ((type == SHUT_IMMEDIATE) || (type == SHUT_SIG))
      {
      state = SV_STATE_DOWN;
      set_svr_attr(SRV_ATR_State, &state);

      strcat(log_buf, "Forced");

      log_event(
        PBSEVENT_SYSTEM | PBSEVENT_ADMIN | PBSEVENT_DEBUG,
        PBS_EVENTCLASS_SERVER,
        msg_daemonname,
        log_buf);

      return;
      }
    }

  if (type == SHUT_IMMEDIATE)
    {
    state = SV_STATE_SHUTIMM;
    set_svr_attr(SRV_ATR_State, &state);

    strcat(log_buf, "Immediate");
    }
  else if (type == SHUT_DELAY)
    {
    state = SV_STATE_SHUTDEL;
    set_svr_attr(SRV_ATR_State, &state);

    strcat(log_buf, "Delayed");
    }
  else if (type == SHUT_QUICK)
    {
    state = SV_STATE_DOWN; /* set to down to brk pbsd_main loop */
    set_svr_attr(SRV_ATR_State, &state);

    strcat(log_buf, "Quick");
    }
  else
    {
    state = SV_STATE_SHUTIMM;
    set_svr_attr(SRV_ATR_State, &state);

    strcat(log_buf, "By Signal");
    }

  log_event(
    PBSEVENT_SYSTEM | PBSEVENT_ADMIN | PBSEVENT_DEBUG,
    PBS_EVENTCLASS_SERVER,
    msg_daemonname,
    log_buf);

  if ((type == SHUT_QUICK) || (type == SHUT_SIG)) /* quick, leave jobs as are */
    {
    return;
    }

  svr_save(&server, SVR_SAVE_QUICK);

  iter = -1;

  while ((pjob = next_job(&alljobs,&iter)) != NULL)
    {
    if (pjob->ji_qs.ji_state == JOB_STATE_RUNNING)
      {
      pjob->ji_qs.ji_svrflags |= JOB_SVFLG_HOTSTART | JOB_SVFLG_HASRUN;

      pattr = &pjob->ji_wattr[JOB_ATR_checkpoint];

      if ((pattr->at_flags & ATR_VFLAG_SET) &&
          ((csv_find_string(pattr->at_val.at_str, "s") != NULL) ||
           (csv_find_string(pattr->at_val.at_str, "c") != NULL) ||
           (csv_find_string(pattr->at_val.at_str, "shutdown") != NULL)))
        {
        /* do checkpoint of job */

        if (shutdown_checkpoint(&pjob) == 0)
          {
          if (pjob != NULL)
            unlock_ji_mutex(pjob, __func__, (char *)"1", LOGLEVEL);

          continue;
          }
        }

      /* if no checkpoint (not supported, not allowed, or fails */
      /* rerun if possible, else kill job */

      rerun_or_kill(&pjob, msg_on_shutdown);
      }

    if (pjob != NULL)
      unlock_ji_mutex(pjob, __func__, (char *)"2", LOGLEVEL);
    }

  return;
  }  /* END svr_shutdown() */






/*
 * shutdown_ack - acknowledge the shutdown (terminate) request
 *  if there is one.  This is about the last thing the server does
 * before going away.
 */

void shutdown_ack(void)

  {
  if (pshutdown_request)
    {
    reply_ack(pshutdown_request);

    pshutdown_request = 0;
    }

  return;
  }




/*
 * req_shutdown - process request to shutdown the server.
 *
 * Must have operator or administrator privilege.
 */

void req_shutdown(

  struct batch_request *preq)

  {
  char  log_buf[LOCAL_LOG_BUF_SIZE];

  if ((preq->rq_perm &
       (ATR_DFLAG_MGWR | ATR_DFLAG_MGRD | ATR_DFLAG_OPRD | ATR_DFLAG_OPWR)) == 0)
    {
    req_reject(PBSE_PERM, 0, preq, NULL, NULL);

    return;
    }

  sprintf(log_buf, msg_shutdown_op, preq->rq_user, preq->rq_host);

  log_event(
    PBSEVENT_SYSTEM | PBSEVENT_ADMIN | PBSEVENT_DEBUG,
    PBS_EVENTCLASS_SERVER,
    msg_daemonname,
    log_buf);

  pshutdown_request = preq;    /* save for reply from main() when done */

  svr_shutdown(preq->rq_ind.rq_shutdown);

  return;
  }  /* END req_shutdown() */





/*
 * shutdown_checkpoint - perform checkpoint of job by issuing a hold request to mom
 */

static int shutdown_checkpoint(

  job **pjob_ptr)

  {
  job                  *pjob = *pjob_ptr;
  struct batch_request *phold;
  pbs_attribute         temp;
  char                  jobid[PBS_MAXSVRJOBID + 1];
  int                   rc = PBSE_NONE;

  phold = alloc_br(PBS_BATCH_HoldJob);

  if (phold == NULL)
    {
    return(PBSE_SYSTEM);
    }

  temp.at_flags = ATR_VFLAG_SET;

  temp.at_type  = job_attr_def[JOB_ATR_hold].at_type;
  temp.at_val.at_long = HOLD_s;

  phold->rq_perm = ATR_DFLAG_MGRD | ATR_DFLAG_MGWR;

  strcpy(phold->rq_ind.rq_hold.rq_orig.rq_objname, pjob->ji_qs.ji_jobid);

  CLEAR_HEAD(phold->rq_ind.rq_hold.rq_orig.rq_attr);

  if (job_attr_def[JOB_ATR_hold].at_encode(
        &temp,
        &phold->rq_ind.rq_hold.rq_orig.rq_attr,
        job_attr_def[JOB_ATR_hold].at_name,
        NULL,
        ATR_ENCODE_CLIENT,
        0) < 0)
    {
    free_br(phold);
    return(PBSE_SYSTEM);
    }

  /* The phold is freed in relay_to_mom (failure)
   * or in issue_Drequest (success) */
  if ((rc = relay_to_mom(&pjob, phold, NULL)) != PBSE_NONE)
    {
    /* FAILURE */
    free_br(phold);

    return(-1);
    }
    
  jobid[0] = '\0';

  if (pjob != NULL)
    {
    pjob->ji_qs.ji_substate = JOB_SUBSTATE_RERUN;
    pjob->ji_qs.ji_svrflags |= JOB_SVFLG_HASRUN | JOB_SVFLG_CHECKPOINT_FILE;
    
    if (LOGLEVEL >= 1)
      {
      log_event(
        PBSEVENT_SYSTEM | PBSEVENT_JOB | PBSEVENT_DEBUG,
        PBS_EVENTCLASS_JOB,
        pjob->ji_qs.ji_jobid,
        "shutting down with active checkpointable job");
      }
  
    job_save(pjob, SAVEJOB_QUICK, 0);
    strcpy(jobid, pjob->ji_qs.ji_jobid);
    unlock_ji_mutex(pjob, __func__, (char *)"1", LOGLEVEL);
    }
  
  if (rc == PBSE_NONE)
    {
    post_checkpoint(phold);

    if (jobid[0] != '\0')
     *pjob_ptr = svr_find_job(jobid, TRUE);
    }

  return(PBSE_NONE);
  }  /* END shutdown_checkpoint() */




/*
 * post-checkpoint - clean up after shutdown_checkpoint
 * This is called on the reply from MOM to a Hold request made in
 * shutdown_checkpoint().  If the request succeeded, then record in job.
 * If the request failed, then we fall back to rerunning or aborting
 * the job.
 */

void post_checkpoint(

  batch_request *preq)

  {
  job *pjob;

  if (preq == NULL)
    return;

  pjob = svr_find_job(preq->rq_ind.rq_hold.rq_orig.rq_objname, FALSE);

  if (preq->rq_reply.brp_code == 0)
    {
    /* checkpointed ok */
    if ((preq->rq_reply.brp_auxcode) && (pjob != NULL)) /* checkpoint can be moved */
      {
      pjob->ji_qs.ji_svrflags =
        (pjob->ji_qs.ji_svrflags & ~JOB_SVFLG_CHECKPOINT_FILE) |
        JOB_SVFLG_HASRUN | JOB_SVFLG_CHECKPOINT_MIGRATEABLE;

      }
    }
  else
    {
    /* need to try rerun if possible or just abort the job */

    if (pjob)
      {
      pjob->ji_qs.ji_svrflags &= ~JOB_SVFLG_CHECKPOINT_FILE;
      pjob->ji_qs.ji_substate = JOB_SUBSTATE_RUNNING;

      if (pjob->ji_qs.ji_state == JOB_STATE_RUNNING)
        rerun_or_kill(&pjob, msg_on_shutdown);
      }
    }

  free_br(preq);

  if (pjob != NULL)
    unlock_ji_mutex(pjob, __func__, (char *)"1", LOGLEVEL);
  }  /* END post_checkpoint() */





/* NOTE:  pjob may be free with dangling pointer */

void rerun_or_kill(

  job  **pjob_ptr, /* I (modified/freed) */
  char  *text)     /* I */

  {
  long       server_state = SV_STATE_DOWN;
  char       log_buf[LOCAL_LOG_BUF_SIZE];
  pbs_queue *pque;
  job       *pjob = *pjob_ptr;

  get_svr_attr_l(SRV_ATR_State, &server_state);
  if (pjob->ji_wattr[JOB_ATR_rerunable].at_val.at_long)
    {
    /* job is rerunable, mark it to be requeued */

    issue_signal(&pjob, "SIGKILL", free_br, NULL);

    if (pjob != NULL)
      {
      pjob->ji_qs.ji_substate  = JOB_SUBSTATE_RERUN;
      if ((pque = get_jobs_queue(&pjob)) != NULL)
        {
        snprintf(log_buf, sizeof(log_buf), "%s%s%s", msg_init_queued, pque->qu_qs.qu_name, text);

        unlock_queue(pque, __func__, (char *)NULL, LOGLEVEL);
        }
      }
    }
  else if (server_state != SV_STATE_SHUTDEL)
    {
    /* job not rerunable, immediate shutdown - kill it off */
    snprintf(log_buf, sizeof(log_buf), "%s%s", msg_job_abort, text);

    /* need to record log message before purging job */

    log_event(
      PBSEVENT_SYSTEM | PBSEVENT_JOB | PBSEVENT_DEBUG,
      PBS_EVENTCLASS_JOB,
      pjob->ji_qs.ji_jobid,
      log_buf);

    job_abt(pjob_ptr, log_buf);

    return;
    }
  else
    {
    /* delayed shutdown, leave job running */
    snprintf(log_buf, sizeof(log_buf), "%s%s", msg_leftrunning, text);
    }

  if (pjob != NULL)
    {
    log_event(
      PBSEVENT_SYSTEM | PBSEVENT_JOB | PBSEVENT_DEBUG,
      PBS_EVENTCLASS_JOB,
      pjob->ji_qs.ji_jobid,
      log_buf);
    }

  return;
  }  /* END rerun_or_kill() */

