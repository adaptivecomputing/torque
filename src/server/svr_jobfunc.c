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
 *	svr_enquejob()     - place job in a queue
 *	svr_dequejob()     - remove job from queue
 *	svr_setjobstate()  - set the state/substate of a job
 *	svr_evaljobstate() - evaluate the state of a job based on attributes
 *	chk_resc_limits()  - check job resources vs queue/server limits
 *	svr_chkque()	   - check if job can enter queue
 *	job_set_wait()	   - set event for when job's wait time ends
 *	get_variable()	   - get value of a single environ variable of a job 
 *	prefix_std_file()  - build the fully prefixed default name for std e/o
 *	get_jobowner()	   - get job owner name without @host suffix
 *	set_resc_deflt()   - set unspecified resource_limit to default values
 *	set_statechar()	   - set the job state attribute character value
 *
 * Private functions
 *	chk_svr_resc_limit() - check job requirements againt queue/server limits
 *	default_std()	   - make the default name for standard out/error
 *	eval_chkpnt()	   - insure job checkpoint .ge. queues min. time
 *	set_deflt_resc()   - set unspecified resource_limit to default values
 *	job_wait_over()	   - event handler for job_set_wait()
 */
#include <pbs_config.h>   /* the master config generated by configure */

#include "pbs_ifl.h"
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "server_limits.h"
#include "list_link.h"
#include "attribute.h"
#include "resource.h"
#include "server.h"
#include "queue.h"
#include "job.h"
#include "work_task.h"
#include "pbs_error.h"
#include "log.h"
#include "acct.h"
#include "svrfunc.h"
#include "sched_cmds.h"
#include "pbs_proto.h"


/* Private Functions */

static void default_std A_((job *, int key, char * to));
static void eval_chkpnt A_((attribute *j, attribute *q));

/* Global Data Items: */

extern struct server server;

extern list_head svr_alljobs;
extern char  *msg_badwait;		/* error message */
extern char  *msg_daemonname;
extern char  *pbs_o_host;
extern char   server_name[];
extern list_head svr_queues;
extern int    comp_resc_lt;
extern int    comp_resc_gt;
extern int    svr_do_schedule;
extern int    LOGLEVEL;

extern time_t time_now;

/* Externl Functions */

extern int node_avail_complex(char *,int *,int *,int *,int *);


/* Private Functions */

#ifndef NDEBUG
static void correct_ct A_((pbs_queue *));
#endif 	/* NDEBUG */

/* sync w/#define JOB_STATE_XXX */

const char *PJobState[] = {
  "TRANSIT",
  "QUEUED",
  "HELD",
  "WAITING",
  "RUNNING",
  "EXITING",
  NULL };


/* sync w/#define JOB_SUBSTATE_XXX, PJobSubState in mom_main.c */

const char *PJobSubState[] = {
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
  "SUBSTATE29"
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
  "COMPLETED",
  "RERUN",                  /* job is rerun, recover output stage */
  "RERUN1",                 /* job is rerun, stageout phase */
  "RERUN2",                 /* job is rerun, delete files stage */
  "RERUN3",                 /* job is rerun, mom delete job */
  NULL };






/*
 * svr_enquejob() - enqueue job into specified queue
 */

int svr_enquejob(

  job *pjob)  /* I */

  {
  attribute     *pattrjb;
  attribute_def *pdef;
  job	        *pjcur;
  pbs_queue     *pque;
  int            rc;

  /* make sure queue is still there, there exists a small window ... */

  pque = find_queuebyname(pjob->ji_qs.ji_queue);

  if (pque == NULL)
    {
    return(PBSE_UNKQUE);
    }

  /* add job to server's all job list and update server counts */

#ifndef NDEBUG
  sprintf(log_buffer, "enqueuing into %s, state %x hop %ld",
    pque->qu_qs.qu_name, 
    pjob->ji_qs.ji_state,
    pjob->ji_wattr[(int)JOB_ATR_hopcount].at_val.at_long);

  log_event(
    PBSEVENT_DEBUG2,
    PBS_EVENTCLASS_JOB,
    pjob->ji_qs.ji_jobid,
    log_buffer);
#endif	/* NDEBUG */

  pjcur = (job *)GET_PRIOR(svr_alljobs);

  while (pjcur != NULL) 
    {
    if ((unsigned long)pjob->ji_wattr[(int)JOB_ATR_qrank].at_val.at_long >= 
        (unsigned long)pjcur->ji_wattr[(int)JOB_ATR_qrank].at_val.at_long)
      break;

    pjcur = (job *)GET_PRIOR(pjcur->ji_alljobs);
    }  /* END while (pjcur != NULL) */

  if (pjcur == 0) 
    {
    /* link first in server's list */

    insert_link(&svr_alljobs, &pjob->ji_alljobs, pjob,
      LINK_INSET_AFTER);
    } 
  else 
    {
    /* link after 'current' job in server's list */

    insert_link(&pjcur->ji_alljobs, &pjob->ji_alljobs, pjob,
      LINK_INSET_AFTER);
    }

  server.sv_qs.sv_numjobs++;
  server.sv_jobstates[pjob->ji_qs.ji_state]++;

  /* place into queue in order of queue rank starting at end */

  pjob->ji_qhdr = pque;

	pjcur = (job *)GET_PRIOR(pque->qu_jobs);
	while (pjcur) {
	    if ((unsigned long)pjob->ji_wattr[(int)JOB_ATR_qrank].at_val.at_long >= (unsigned long)pjcur->ji_wattr[(int)JOB_ATR_qrank].at_val.at_long)
			break;
		pjcur = (job *)GET_PRIOR(pjcur->ji_jobque);
	}
	if (pjcur == 0) {
		/* link first in list */
		insert_link(&pque->qu_jobs, &pjob->ji_jobque, pjob,
			    LINK_INSET_AFTER);
	} else {
		/* link after 'current' job in list */
		insert_link(&pjcur->ji_jobque, &pjob->ji_jobque, pjob,
			    LINK_INSET_AFTER);
	}

	/* update counts: queue and queue by state */

	pque->qu_numjobs++;
	pque->qu_njstate[pjob->ji_qs.ji_state]++;

	/* update the current location and type attribute */

	pdef    = &job_attr_def[(int)JOB_ATR_in_queue];
	pattrjb = &pjob->ji_wattr[(int)JOB_ATR_in_queue];
	pdef->at_free(pattrjb);
	pdef->at_decode(pattrjb, (char *)0, (char *)0, pque->qu_qs.qu_name);

	pjob->ji_wattr[(int)JOB_ATR_queuetype].at_val.at_char = 
			*pque->qu_attr[(int)QA_ATR_QType].at_val.at_str;
	pjob->ji_wattr[(int)JOB_ATR_queuetype].at_flags |= ATR_VFLAG_SET;

	if ((pjob->ji_wattr[(int)JOB_ATR_qtime].at_flags & ATR_VFLAG_SET) ==0) {
		pjob->ji_wattr[(int)JOB_ATR_qtime].at_val.at_long = time_now;
		pjob->ji_wattr[(int)JOB_ATR_qtime].at_flags |= ATR_VFLAG_SET;

		/* issue enqueued accounting record */

		(void)sprintf(log_buffer, "queue=%s", pque->qu_qs.qu_name);
		account_record(PBS_ACCT_QUEUE, pjob, log_buffer);
	}
	
	/*
	 * set any "unspecified" resources which have default values,
	 * first with queue defaults, then with server defaults
	 */

	set_resc_deflt(pjob);

	/*
	 * See if we need to do anything special based on type of queue
	 */

	if (pque->qu_qs.qu_type == QTYPE_Execution) {

		/* set union to "EXEC" and clear mom's address */

		if (pjob->ji_qs.ji_un_type != JOB_UNION_TYPE_EXEC) {
			pjob->ji_qs.ji_un_type = JOB_UNION_TYPE_EXEC;
			pjob->ji_qs.ji_un.ji_exect.ji_momaddr = 0;
			pjob->ji_qs.ji_un.ji_exect.ji_exitstat = 0;
		}

    /* check the job checkpoint against the queue's  min */

    eval_chkpnt(
      &pjob->ji_wattr[(int)JOB_ATR_chkpnt],
      &pque->qu_attr[(int)QE_ATR_ChkptMim]);
		
    /* do anything needed doing regarding job dependencies */

    if (pjob->ji_wattr[(int)JOB_ATR_depend].at_flags & ATR_VFLAG_SET)
      {
      if ((rc = depend_on_que(&
             pjob->ji_wattr[(int)JOB_ATR_depend],
             pjob,
             ATR_ACTION_NOOP)) != 0)
        {
        return (rc);
        }
      }

    /* set eligible time */

    if (((pjob->ji_wattr[(int)JOB_ATR_etime].at_flags & ATR_VFLAG_SET) == 0) && 
         (pjob->ji_qs.ji_state == JOB_STATE_QUEUED)) 
      {
      pjob->ji_wattr[(int)JOB_ATR_etime].at_val.at_long =time_now;
      pjob->ji_wattr[(int)JOB_ATR_etime].at_flags |=ATR_VFLAG_SET;

      /* notify the scheduler we have a new job */

      svr_do_schedule = SCH_SCHEDULE_NEW;
      }
    } 
  else if (pque->qu_qs.qu_type == QTYPE_RoutePush) 
    {
    /* start attempts to route job */

    pjob->ji_qs.ji_un_type = JOB_UNION_TYPE_ROUTE;
    pjob->ji_qs.ji_un.ji_routet.ji_quetime = time_now;
    pjob->ji_qs.ji_un.ji_routet.ji_rteretry = 0;
    }

  return(0);
  }  /* END svr_enquejob() */





/*
 * svr_dequejob() - remove job from whatever queue its in and reduce counts
 */

void svr_dequejob(

  job *pjob)

  {
  int	   bad_ct = 0;
  attribute *pattr;
  pbs_queue *pque;
  resource  *presc;

  /* remove job from server's all job list and reduce server counts */

  if (is_linked(&svr_alljobs, &pjob->ji_alljobs)) 
    {
    delete_link(&pjob->ji_alljobs);

    if (--server.sv_qs.sv_numjobs < 0)
      bad_ct = 1;
			
    if (--server.sv_jobstates[pjob->ji_qs.ji_state] < 0)
      bad_ct = 1;
    }

  if ((pque = pjob->ji_qhdr) != (pbs_queue *)0) 
    {
    if (is_linked(&pque->qu_jobs, &pjob->ji_jobque)) 
      {
      delete_link(&pjob->ji_jobque);

      if (--pque->qu_numjobs < 0)
        bad_ct = 1;

      if (--pque->qu_njstate[pjob->ji_qs.ji_state] < 0)
        bad_ct = 1;
      }

    pjob->ji_qhdr = (pbs_queue *)0;
    }

#ifndef NDEBUG

  sprintf(log_buffer, "dequeuing from %s, state %s",
    pque ? pque->qu_qs.qu_name : "unknown queue", 
    PJobState[pjob->ji_qs.ji_state]);

  log_event(
    PBSEVENT_DEBUG2, 
    PBS_EVENTCLASS_JOB, 
    pjob->ji_qs.ji_jobid,
    log_buffer);

  if (bad_ct) 		/* state counts are all messed up */
    correct_ct(pque);

#endif	/* NDEBUG */

  pjob->ji_wattr[(int)JOB_ATR_qtime].at_flags &= ~ATR_VFLAG_SET;

  /* clear any default resource values.		*/

  pattr = &pjob->ji_wattr[(int)JOB_ATR_resource];

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

  /* notify scheduler a job has been removed */
                                                                                                      
  svr_do_schedule = SCH_SCHEDULE_TERM;

  return;
  }  /* END svr_dequeuejob() */





/*
 * svr_setjobstate - set the job state, update the server/queue state counts,
 *	and save the job
 */

int svr_setjobstate(

  job *pjob,        /* I (modified) */
  int  newstate,    /* I */
  int  newsubstate) /* I */

  {
  int    changed = 0;
  int    oldstate;

  pbs_queue *pque = pjob->ji_qhdr;

  if (LOGLEVEL >= 2)
    {
    sprintf(log_buffer,"svr_setjobstate: setting job %s state from %s to %s-%s (%d-%d)\n",
      (pjob->ji_qs.ji_jobid != NULL) ? pjob->ji_qs.ji_jobid : "",
      PJobState[pjob->ji_qs.ji_state],
      PJobState[newstate],
      PJobSubState[newsubstate],
      newstate,
      newsubstate);

    log_event(PBSEVENT_ERROR,PBS_EVENTCLASS_SERVER,msg_daemonname,
      log_buffer);
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

      server.sv_jobstates[oldstate]--;
      server.sv_jobstates[newstate]++;

      if (pque != (pbs_queue *)0) 
        {
        pque->qu_njstate[oldstate]--;
        pque->qu_njstate[newstate]++;

        /* if execution queue, and eligibility to run has improved, */
        /* notify the scheduler */

        if ((pque->qu_qs.qu_type == QTYPE_Execution) &&
            (newstate == JOB_STATE_QUEUED)) 
          {
          svr_do_schedule = SCH_SCHEDULE_NEW;

          if ((pjob->ji_wattr[(int)JOB_ATR_etime].at_flags & ATR_VFLAG_SET) == 0) 
            {
            pjob->ji_wattr[(int)JOB_ATR_etime].at_val.at_long =time_now;
            pjob->ji_wattr[(int)JOB_ATR_etime].at_flags |=ATR_VFLAG_SET;
	    }
          } 
        else if ((newstate == JOB_STATE_HELD) ||
                 (newstate == JOB_STATE_WAITING)) 
          {
          /* on hold or wait, clear etime */

          job_attr_def[(int)JOB_ATR_etime].at_free(
            &pjob->ji_wattr[(int)JOB_ATR_etime]);
          }
        }
      }   
    }    /* END if (pjob->ji_qs.ji_substate != JOB_SUBSTATE_TRANSICM) */

  /* set the states accordingly */

  pjob->ji_qs.ji_state = newstate;
  pjob->ji_qs.ji_substate = newsubstate;
  pjob->ji_wattr[(int)JOB_ATR_substate].at_val.at_long = newsubstate;
 
  set_statechar(pjob);

  /* update the job file */
	
  if (pjob->ji_modified)
    {
    return(job_save(pjob,SAVEJOB_FULL));
    }

  if (changed)
    {
    return(job_save(pjob,SAVEJOB_QUICK));
    }

  return(0);
  }  /* END svr_setjobstate() */




/*
 * svr_evaljobstate - evaluate and return the job state and substate
 *	according to the the values of the hold, execution time, and
 *	dependency attributes.  This is typically called after the job has been 
 *	enqueued or the (hold, execution-time) attributes have been modified.
 */

void svr_evaljobstate(

  job *pjob,
  int *newstate,  /* O recommended new state for job    */
  int *newsub,    /* O recommended new substate for job */
  int  forceeval)

  {
  if ((forceeval == 0) &&
     ((pjob->ji_qs.ji_state == JOB_STATE_RUNNING) ||
      (pjob->ji_qs.ji_state == JOB_STATE_TRANSIT))) 
    {
    *newstate = pjob->ji_qs.ji_state;	/* leave as is */
    *newsub   = pjob->ji_qs.ji_substate;
    } 
  else if (pjob->ji_wattr[(int)JOB_ATR_hold].at_val.at_long) 
    {
    *newstate = JOB_STATE_HELD;

    /* is the hold due to a dependency? */

    if ((pjob->ji_qs.ji_substate == JOB_SUBSTATE_SYNCHOLD) ||
        (pjob->ji_qs.ji_substate == JOB_SUBSTATE_DEPNHOLD))
      *newsub = pjob->ji_qs.ji_substate;  /* keep it */
    else
      *newsub = JOB_SUBSTATE_HELD;
    } 
  else if (pjob->ji_wattr[(int)JOB_ATR_exectime].at_val.at_long > (long)time_now) 
    {
    *newstate = JOB_STATE_WAITING;
    *newsub   = JOB_SUBSTATE_WAITING;
    } 
  else if (pjob->ji_wattr[(int)JOB_ATR_stagein].at_flags & ATR_VFLAG_SET) 
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
 *	variable of a job
 *
 *	Returns a pointer to the start of the value or NULL if the variable
 *	is not found in the variable_list attribute.
 */

char *get_variable(

  job  *pjob,     /* I */
  char *variable) /* I */

  {
  char *pc;

  pc = arst_string(
         variable, 
         &pjob->ji_wattr[(int)JOB_ATR_variables]);

  if (pc) 
    {
    if ((pc = strchr(pc,(int)'=')) != NULL)
      pc++;
    }

  return(pc);
  }  /* END get_variable() */




/*
 * compare the job resource limit against the system limit
 * unless a queue limit exists, it takes priority
 *
 * returns number of .gt. and .lt. comparision in comp_resc_gt and comp_resc_lt
 *	does not make use of comp_resc_eq or comp_resc_nc
 */

static void chk_svr_resc_limit(

  attribute *jobatr,
  attribute *queatr,
  attribute *svratr,
  int	     qtype)  /* I */

  {
  int       dummy;
  int       rc;
  resource *jbrc;

  resource *jbrc_nodes = NULL;

  resource *qurc;
  resource *svrc;
  resource *cmpwith;

  static int SvrNodeCt = 0;

  static resource_def *noderesc = NULL;
  static resource_def *needresc = NULL;
  static resource_def *nodectresc = NULL;

  /* NOTE:  server limits are specified with server.resources_available */

  if (noderesc == NULL) 
    {
    noderesc = find_resc_def(svr_resc_def,"nodes",svr_resc_size);
    needresc = find_resc_def(svr_resc_def,"neednodes",svr_resc_size);
    nodectresc = find_resc_def(svr_resc_def,"nodect",svr_resc_size);

    if (nodectresc != NULL)
      {
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
      }   /* END if (nodectresc != NULL) */
    }     /* END if (noderesc == NULL) */

  /* return values via global comp_resc_gt and comp_resc_lt */

  comp_resc_gt = 0;
  comp_resc_lt = 0;

  jbrc = (resource *)GET_NEXT(jobatr->at_val.at_list);

  while (jbrc != NULL) 
    {
    cmpwith = NULL;

    if ((jbrc->rs_value.at_flags & (ATR_VFLAG_SET|ATR_VFLAG_DEFLT)) == ATR_VFLAG_SET) 
      {
      qurc = find_resc_entry(queatr,jbrc->rs_defin);

      if ((qurc == NULL) || ((qurc->rs_value.at_flags & ATR_VFLAG_SET) == 0)) 
        {
        /* queue limit not set, check server's */
			
        svrc = find_resc_entry(svratr,jbrc->rs_defin);

        if ((svrc != NULL) && (svrc->rs_value.at_flags & ATR_VFLAG_SET)) 
          {
          cmpwith = svrc;
          }
        } 
      else 
        {
        /* queue limit is set, use it */

        cmpwith = qurc;
        }

      if ((jbrc->rs_defin == noderesc) && (qtype == QTYPE_Execution)) 
        {
        /* NOTE:  process after loop so SvrNodeCt is loaded */

        jbrc_nodes = jbrc;
        }
      else if ((cmpwith != NULL) && (jbrc->rs_defin != needresc)) 
        {
        /* don't check neednodes */

        rc = jbrc->rs_defin->rs_comp(
               &cmpwith->rs_value,
               &jbrc->rs_value);

        if (rc > 0)
          comp_resc_gt++;
        else if (rc < 0)
          comp_resc_lt++;
        }
      }    /* END if () */

    jbrc = (resource *)GET_NEXT(jbrc->rs_link);
    }  /* END while (jbrc != NULL) */

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

    if (isdigit(*ptr) && !strchr(ptr,':') && !strchr(ptr,'+'))
      {
      tmpI = (int)strtol(ptr,NULL,10);

      if ((SvrNodeCt > 0) && (tmpI <= SvrNodeCt))
        IgnTest = 1;
      }

    if (IgnTest == 0)
      {
      if (node_avail_complex(
            jbrc_nodes->rs_value.at_val.at_str,
            &dummy,
            &dummy,
            &dummy,
            &dummy) == -1)
        {
        comp_resc_lt++;
        }
      }
    }

  return;
  }  /* END chk_svr_resc_limit() */




/*
 * chk_resc_limits - check job Resource_Limits attribute against the queue 
 *	and server maximum and mininum values.
 *
 *	return 0 if within limits, or an pbs_error number if not.
 */

int chk_resc_limits(

  attribute *pattr,  /* I */
  pbs_queue *pque)   /* I */

  {
  /* NOTE:  comp_resc_gt and comp_resc_lt are global ints */

  /* first check against queue minimum */

  if ((comp_resc(&pque->qu_attr[QA_ATR_ResourceMin],pattr) == -1) ||
      (comp_resc_gt > 0))
    {
    return(PBSE_EXCQRESC);
    }

  /* now check against queue or server maximum */

  chk_svr_resc_limit(
    pattr,
    &pque->qu_attr[QA_ATR_ResourceMax],
    &server.sv_attr[SRV_ATR_ResourceMax],
    pque->qu_qs.qu_type);

  if (comp_resc_lt > 0)
    {
    return(PBSE_EXCQRESC);
    }

  return(0);
  }  /* END chk_resc_limits() */




/*
 * svr_chkque - check if job can enter a queue
 *
 *	returns 0 for yes, 1 for no
 *
 * Note: the following fields must be set in the job struture before
 *	 calling svr_chkque(): 	ji_wattr[JOB_ATR_job_owner]
 * Also if the job is being considered for an execution queue, then
 * set_jobexid() will be called.
 */

int svr_chkque(

  job	    *pjob,
  pbs_queue *pque,
  char	    *hostname,
  int	     mtype)	/* MOVE_TYPE_* type, see server_limits.h */

  {
  int i;
  int failed_group_acl = 0;
  int failed_user_acl  = 0;

  /*
   * 1. If the queue is an Execution queue ...
   *    These is checked first because 1a - 1c are more damaging
   *    (see local_move() in svr_movejob.c)
   */

  if (pque->qu_qs.qu_type == QTYPE_Execution) 
    {
    /* 1a. if not already set, set up execution uid/gid/name */

    if (!(pjob->ji_wattr[(int)JOB_ATR_euser].at_flags & ATR_VFLAG_SET))
      {
      if ((i = set_jobexid(pjob,pjob->ji_wattr)) != 0)
        {
        return(i);  /* PBSE_BADUSER or GRP */
        }
      }

    /* 1b. check site restrictions */

    if (site_acl_check(pjob,pque))
      {
      return(PBSE_PERM);
      }

    /* 1c. cannot have an unknown resource */

    if (find_resc_entry(
         &pjob->ji_wattr[(int)JOB_ATR_resource],
         &svr_resc_def[svr_resc_size - 1]) != 0)
      {
      return(PBSE_UNKRESC);
      }

    /* 1d. cannot have an unknown attribute */

    if (pjob->ji_wattr[(int)JOB_ATR_UNKN].at_flags & ATR_VFLAG_SET)
      {
      return(PBSE_NOATTR);
      }
			
    /* 1e. if enabled, check the queue's group ACL */

    if (pque->qu_attr[QA_ATR_AclGroupEnabled].at_val.at_long)
      {
      struct array_strings *parst;

      int rc;

      if ((pattr->at_flags & ATR_VFLAG_SET) &&
          (parst = pattr->at_val.at_arst) &&
          (getenv("PBSACLUSEGRPLIST") != NULL))
        {
        char  *pn;
        char  *ptr;
        static char groupname[PBS_MAXUSER + 1];

        int    i;

        /* search the group-list attribute */

        if ((pattr->at_flags & ATR_VFLAG_SET) &&
            (parst = pattr->at_val.at_arst))
          {
          for (i = 0;i < parst->as_usedptr;i++)
            {
            pn = parst->as_string[i];
            ptr = strchr(pn,'@');

            /* NOTE:  match against both local and remote groups (FIXME) */

            if (ptr != NULL)
              {
              strncpy(groupname,pn,MIN(PBS_MAXUSER,ptr - pn));
              groupname[MIN(PBS_MAXUSER,ptr - pn)] = '\0';

              rc = acl_check(
                &pque->qu_attr[QA_ATR_AclGroup],
                groupname,
                ACL_Group);
              }

            if (rc != 0)
              {
              /* match found or failure encountered */

              break;
              }
            }  /* END for (i) */
          }
        }
      else
        {
        rc = acl_check(
           &pque->qu_attr[QA_ATR_AclGroup],
           pjob->ji_wattr[(int)JOB_ATR_egroup].at_val.at_str,
           ACL_Group);
        }

      if (rc == 0)
        {
        /* ACL not satisfied */

        if (mtype != MOVE_TYPE_MgrMv) /* ok if mgr */
          {
          if (pque->qu_attr[QA_ATR_AclLogic].at_val.at_long &&
              pque->qu_attr[QA_ATR_AclUserEnabled].at_val.at_long)
            {
            /* only fail if neither user nor group acls can be met */

            failed_group_acl = 1;
            }
          else
            {
            /* no user acl, fail immediately */

            return(PBSE_PERM);
            }
          }
        }
      }
    }

  /* checks 2 and 3 are bypassed for a move by manager or qorder */

  if ((mtype != MOVE_TYPE_MgrMv) && (mtype != MOVE_TYPE_Order)) 
    {
    /* 2. the queue must be enabled and the job limit not exceeded */

    if (pque->qu_attr[QA_ATR_Enabled].at_val.at_long == 0)
      {
      return(PBSE_QUNOENB);
      }

    if ((pque->qu_attr[QA_ATR_MaxJobs].at_flags & ATR_VFLAG_SET) &&
        (pque->qu_numjobs>=pque->qu_attr[QA_ATR_MaxJobs].at_val.at_long))
      {
      return(PBSE_MAXQUED);
      }
    
    /* 3. if "from_route_only" is true, only local route allowed */

    if ((pque->qu_attr[QA_ATR_FromRouteOnly].at_flags&ATR_VFLAG_SET) &&
        (pque->qu_attr[QA_ATR_FromRouteOnly].at_val.at_long == 1))
      {
      if (mtype == MOVE_TYPE_Move)  /* ok if not plain user */
        {
        return(PBSE_QACESS);
        }
      }
    }

  /* checks 4, 5, and 6 are bypassed for a move by manager */

  if (mtype != MOVE_TYPE_MgrMv)
    {
    /* 4. if enabled, check the queue's host ACL */

    if (pque->qu_attr[QA_ATR_AclHostEnabled].at_val.at_long)
      {
      if (acl_check(&pque->qu_attr[QA_ATR_AclHost],hostname,ACL_Host) == 0)
        {
        return(PBSE_BADHOST);
        }
      }

    /* 5. if enabled, check the queue's user ACL */

    if (pque->qu_attr[QA_ATR_AclUserEnabled].at_val.at_long) 
      {
      if (acl_check(
           &pque->qu_attr[QA_ATR_AclUsers],
           pjob->ji_wattr[(int)JOB_ATR_job_owner].at_val.at_str,
           ACL_User) == 0)
        {
        if (pque->qu_attr[QA_ATR_AclLogic].at_val.at_long &&
            pque->qu_attr[QA_ATR_AclGroupEnabled].at_val.at_long)
          {
          /* only fail if neither user nor group acls can be met */

          failed_user_acl = 1;
          }
        else
          {
          /* no group acl, fail immediately */

          return(PBSE_PERM);
          }
        }
      }

    /* 5.5. If failed user and group acls, fail */

    if (failed_group_acl && failed_user_acl)
      {
      return(PBSE_PERM);
      }

    /* 6. resources of the job must be in the limits of the queue */

    if ((i = chk_resc_limits(&pjob->ji_wattr[(int)JOB_ATR_resource],pque)) != 0)
      {
      return(i);
      }
    }    /* END if (mtype != MOVE_TYPE_MgrMv) */

  return(0);	/* all ok, job can enter queue */
  }  /* END svr_chkque() */





/*
 * job_wait_over - The execution wait time of a job has been reached, at 
 *	least according to a work_task entry via which we were invoked.
 *
 *	If indeed the case, re-evaluate and set the job state.
 */

static void job_wait_over(

  struct work_task *pwt)

  {
  int  newstate;
  int  newsub;
  job *pjob;

  pjob = (job *)pwt->wt_parm1;

#ifndef NDEBUG
  {
  time_t now = time((time_t *)0);
  time_t when = ((job *)pjob)->ji_wattr[(int)JOB_ATR_exectime].at_val.at_long;
  struct work_task *ptask;

  if (when > now) 
    {
    sprintf(log_buffer,msg_badwait,((job *)pjob)->ji_qs.ji_jobid);

    log_err(-1,"job_wait_over",log_buffer);

    /* recreate the work task entry */

    ptask = set_task(WORK_Timed,when,job_wait_over,pjob);

    if (ptask != NULL)
      append_link(&pjob->ji_svrtask,&ptask->wt_linkobj,ptask);

    return;
    }
  }
#endif  /* !NDEBUG */

  pjob->ji_qs.ji_svrflags &= ~JOB_SVFLG_HASWAIT;

  /* clear the exectime attribute */

  job_attr_def[(int)JOB_ATR_exectime].at_free(
    &pjob->ji_wattr[(int)JOB_ATR_exectime]);	

  pjob->ji_modified = 1;

  svr_evaljobstate(pjob,&newstate,&newsub,0);

  svr_setjobstate(pjob,newstate,newsub);

  return;
  }




/*
 * job_set_wait - set up a work task entry that will trigger at the execution
 *	wait time of the job.
 *
 *	This is called as the at_action (see attribute.h) function associated
 *	with the execution-time job attribute.
 */

int job_set_wait(

  attribute *pattr,
  void      *pjob,	/* a (job *) cast to void * */
  int        mode)	/* unused, do it for all action modes */

  {
  struct work_task *ptask;
  long              when;

  if ((pattr->at_flags & ATR_VFLAG_SET) == 0)
    {
    return(0);
    }

  when  = pattr->at_val.at_long;

  ptask = (struct work_task *)GET_NEXT(((job *)pjob)->ji_svrtask);

  /* Is there already an entry for this job?  Then reuse it */

  if (((job *)pjob)->ji_qs.ji_svrflags & JOB_SVFLG_HASWAIT) 
    {
    while (ptask != NULL) 
      {
      if ((ptask->wt_type == WORK_Timed) && 
          (ptask->wt_func == job_wait_over) &&
          (ptask->wt_parm1 == pjob)) 
        {
        ptask->wt_event = when;

        return(0);
        }

      ptask = (struct work_task *)GET_NEXT(ptask->wt_linkobj);
      }
    }

  ptask = set_task(WORK_Timed,when,job_wait_over,pjob);

  if (ptask == NULL)
    {
    return(-1);
    }

  append_link(&((job *)pjob)->ji_svrtask,&ptask->wt_linkobj,ptask);

  /* set JOB_SVFLG_HASWAIT to show job has work task entry */

  ((job *)pjob)->ji_qs.ji_svrflags |= JOB_SVFLG_HASWAIT;

  return(0);
  }  /* END job_set_wait() */




/*
 * default_std - make the default name for standard output or error
 *	"job_name".[e|o]job_sequence_number
 */

static void default_std(

  job  *pjob,
  int   key,		/* 'e' for stderr, 'o' for stdout */
  char *to)		/* ptr to buffer in which to return name */
			/* had better be big enough!	  */
  {
  int   len;
  char *pd;

  pd = strrchr(pjob->ji_wattr[(int)JOB_ATR_jobname].at_val.at_str, '/');

  if (pd != NULL)
    ++pd;
  else
    pd = pjob->ji_wattr[(int)JOB_ATR_jobname].at_val.at_str;

  len = strlen(pd);

  strcpy(to, pd);		/* start with the job name */

  *(to + len++) = '.';            /* the dot        */
  *(to + len++) = (char)key;	/* the letter     */

  pd = pjob->ji_qs.ji_jobid;      /* the seq_number */

  while (isdigit((int)*pd))       
    *(to + len++) = *pd++;

  *(to + len) = '\0';

  return;
  }  /* END default_std() */





/*
 * prefix_std_file - build the fully prefixed default file name for the job's
 *	standard output or error: 
 *		qsub_host:$PBS_O_WORKDIR/job_name.[e|o]job_sequence_number
 */

char *prefix_std_file(

  job *pjob,
  int  key)

  {
  int	 len;
  char	*name = (char *)0;
  char	*qsubhost;
  char	*wdir;

  qsubhost = get_variable(pjob,pbs_o_host);
  wdir     = get_variable(pjob,"PBS_O_WORKDIR");

  if (qsubhost != NULL) 
    {
    len = strlen(qsubhost) + 
          strlen(pjob->ji_wattr[(int)JOB_ATR_jobname].at_val.at_str) + 
          PBS_MAXSEQNUM + 
          5;

    if (wdir)
      len += strlen(wdir);

    name = malloc(len);
      
    if (name) 
      {
      strcpy(name,qsubhost);	/* the qsub host name	*/
      strcat(name,":");	        /* the :		*/

      if (wdir) 
        {
        strcat(name,wdir);	/* the qsub cwd		*/
        strcat(name,"/");	/* the final /		*/
        }
						/* now add the rest	*/
      default_std(pjob,key,name + strlen(name));
      }
    }

  return(name);
  }




/*
 * get_jobowner - copy the basic job owner's name, without the @host suffix.
 *	The "to" buffer must be large enought (PBS_MAXUSER+1).
 */

void get_jobowner(from, to)
	char *from;
	char *to;
{
	int i;

	for (i=0; i<PBS_MAXUSER; ++i) {
		if ( (*(from+i) == '@') || (*(from+i) == '\0') )
			break;
		*(to+i) = *(from+i);
	}
	*(to+i) = '\0';
}





static void set_deflt_resc(

  attribute *jb,
  attribute *dflt)

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

        prescjb = find_resc_entry(jb,prescdt->rs_defin);

        if ((prescjb == NULL) ||
           ((prescjb->rs_value.at_flags & ATR_VFLAG_SET) == 0)) 
          {
          /* resource does not exist or value is not set */

          if (prescjb == NULL)
            prescjb = add_resource_entry(jb,prescdt->rs_defin);

          if (prescjb != NULL) 
            {
            if (prescdt->rs_defin->rs_set(
                  &prescjb->rs_value,
                  &prescdt->rs_value,
                  SET) == 0)
              {
              prescjb->rs_value.at_flags |= (ATR_VFLAG_SET|ATR_VFLAG_DEFLT);
              }
            }
          }
        }

      prescdt = (resource *)GET_NEXT(prescdt->rs_link);
      }
    } 

  return;
  }  /* END set_deflt_resc() */




void set_resc_deflt(

  job *pjob)

  {
  pbs_queue *pque;

  pque = pjob->ji_qhdr;

  assert(pque != NULL);

  set_deflt_resc(
    &pjob->ji_wattr[(int)JOB_ATR_resource],
    &pque->qu_attr[(int)QA_ATR_ResourceDefault]);

  set_deflt_resc(
    &pjob->ji_wattr[(int)JOB_ATR_resource],
    &server.sv_attr[(int)SRV_ATR_resource_deflt]);

  set_deflt_resc(
    &pjob->ji_wattr[(int)JOB_ATR_resource],
    &pque->qu_attr[(int)QA_ATR_ResourceMax]);

  set_deflt_resc(
    &pjob->ji_wattr[(int)JOB_ATR_resource],
    &server.sv_attr[(int)SRV_ATR_ResourceMax]);

  return;
  }




/*
 * set_statechar - set the job state attribute to the letter that correspondes
 *	to its current state.
 */

void set_statechar(pjob)
	job *pjob;
{
	static char *statechar = "TQHWRE";
	static char suspend    = 'S';

	if ( (pjob->ji_qs.ji_state == JOB_STATE_RUNNING) &&
	     (pjob->ji_qs.ji_svrflags & JOB_SVFLG_Suspend) )
		pjob->ji_wattr[JOB_ATR_state].at_val.at_char = suspend;
	else
		pjob->ji_wattr[JOB_ATR_state].at_val.at_char = 
					*(statechar + pjob->ji_qs.ji_state);
}






/*
 * eval_chkpnt - if the job's checkpoint attribute is "c=nnnn" and 
 * 	nnnn is less than the queue' minimum checkpoint time, reset
 *	to the queue min time. 
 */

static void eval_chkpnt(jobckp, queckp)
	attribute *jobckp;	/* job's checkpoint attribute */
	attribute *queckp;	/* queue's checkpoint attribute */
{
	int jobs;
	char queues[30];
	char *pv;
	
	if ( ((jobckp->at_flags & ATR_VFLAG_SET) == 0)  ||
	     ((queckp->at_flags & ATR_VFLAG_SET) == 0) )
		return;		/* need do nothing */

	pv = jobckp->at_val.at_str;
	if (*pv++ == 'c') {
		if (*pv == '=')
			pv++;
		jobs = atoi(pv);
		if (jobs < queckp->at_val.at_long) {
			(void)sprintf(queues, "c=%ld", queckp->at_val.at_long);
			free_str(jobckp);
			(void)decode_str(jobckp, 0, 0, queues);
		}
	}
}





#ifndef NDEBUG

/*
 * correct_ct - This is a work-around for an as yet unfound bug where
 *	the counts of jobs in each state sometimes (rarely) become wrong.
 *	When this happens, the count for a state can become negative.
 *	If this is detected (see above), this routine is called to reset
 *	all of the counts and log a message.
 */

static void correct_ct(

  pbs_queue *pqj)

  {
  int	   i;
  char	  *pc;
  job	  *pjob;
  pbs_queue *pque;

  sprintf(log_buffer,"Job state counts incorrect, server %d: ",
    server.sv_qs.sv_numjobs);

  server.sv_qs.sv_numjobs = 0;

  for (i = 0;i < PBS_NUMJOBSTATE;++i)  
    {
    pc = log_buffer + strlen(log_buffer);

    sprintf(pc,"%d ", 
      server.sv_jobstates[i]);

    server.sv_jobstates[i] = 0;
    }

  if (pqj) 
    {
    pc = log_buffer + strlen(log_buffer);

    sprintf(pc,"; queue %s %d: ", 
      pqj->qu_qs.qu_name,
      pqj->qu_numjobs);

    for (i = 0;i < PBS_NUMJOBSTATE;++i) 
      {
      pc = log_buffer + strlen(log_buffer);

      sprintf(pc,"%d ", 
        pqj->qu_njstate[i]);
      }
    }

  log_event(PBSEVENT_ERROR,PBS_EVENTCLASS_SERVER,msg_daemonname,
    log_buffer);

  for (pque = (pbs_queue *)GET_NEXT(svr_queues);pque; 
       pque = (pbs_queue *)GET_NEXT(pque->qu_link)) 
    {
    pque->qu_numjobs = 0;

    for (i = 0;i < PBS_NUMJOBSTATE;++i)
      pque->qu_njstate[i] = 0;
    }

  for (pjob = (job *)GET_NEXT(svr_alljobs);pjob != NULL;
       pjob = (job *)GET_NEXT(pjob->ji_alljobs))  
    {
    server.sv_qs.sv_numjobs++;

    server.sv_jobstates[pjob->ji_qs.ji_state]++;

    (pjob->ji_qhdr)->qu_numjobs++;
    (pjob->ji_qhdr)->qu_njstate[pjob->ji_qs.ji_state]++;
    }  /* END for (pjob) */

  return;
  }  /* END correct_ct() */

#endif 	/* NDEBUG */
