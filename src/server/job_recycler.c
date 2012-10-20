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


#include <stdlib.h>

#include "pbs_job.h"
#include "utils.h"
#include "threadpool.h"
#include "ji_mutex.h"

extern job_recycler recycler;
extern int          LOGLEVEL;


void initialize_recycler()

  {
  recycler.rc_next_id = 0;
  initialize_all_jobs_array(&recycler.rc_jobs);
  recycler.rc_iter = -1;

  recycler.rc_mutex = calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(recycler.rc_mutex,NULL);
  } /* END initialize_recycler() */




job *next_job_from_recycler(

  struct all_jobs *aj,
  int             *iter)

  {
  job *pjob;

  lock_alljobs_mutex(aj, __func__, NULL, LOGLEVEL);
  pjob = next_thing(aj->ra, iter);
  unlock_alljobs_mutex(aj, __func__, NULL, LOGLEVEL);

  if (pjob != NULL)
    lock_ji_mutex(pjob, __func__, NULL, LOGLEVEL);

  return(pjob);
  } /* END next_job_from_recycler() */




void *remove_some_recycle_jobs(
    
  void *vp)

  {
  int  i;
  int  iter = -1;
  job *pjob = NULL;

  pthread_mutex_lock(recycler.rc_mutex);

  for (i = 0; i < JOBS_TO_REMOVE; i++)
    {
    pjob = next_job_from_recycler(&recycler.rc_jobs,&iter);
    
    if (pjob == NULL)
      break;

    if (LOGLEVEL >= 10)
      log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, pjob->ji_qs.ji_jobid);

    remove_job(&recycler.rc_jobs, pjob);
    unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
    free(pjob->ji_mutex);
    memset(pjob, 255, sizeof(job));
    free(pjob);
    }

  pthread_mutex_unlock(recycler.rc_mutex);

  return(NULL);
  } /* END remove_some_recycle_jobs() */




int insert_into_recycler(

  job *pjob)

  {
  int              rc;
  pthread_mutex_t *tmp = pjob->ji_mutex;
  char             log_buf[LOCAL_LOG_BUF_SIZE];

  if (LOGLEVEL >= 7)
    {
    snprintf(log_buf, sizeof(log_buf),
      "Adding job %s to the recycler", pjob->ji_qs.ji_jobid);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
    }

  memset(pjob, 0, sizeof(job));
  pjob->ji_mutex = tmp;

  pthread_mutex_lock(recycler.rc_mutex);

  sprintf(pjob->ji_qs.ji_jobid,"%d",recycler.rc_next_id);
  pjob->ji_being_recycled = TRUE;

  if (recycler.rc_jobs.ra->num >= MAX_RECYCLE_JOBS)
    {
    enqueue_threadpool_request(remove_some_recycle_jobs,NULL);
    }
    
  rc = insert_job(&recycler.rc_jobs, pjob);
    
  update_recycler_next_id();

  pthread_mutex_unlock(recycler.rc_mutex);

  return(rc);
  } /* END insert_into_recycler() */




job *get_recycled_job()

  {
  job *pjob;

  pthread_mutex_lock(recycler.rc_mutex);
  pjob = next_job_from_recycler(&recycler.rc_jobs,&recycler.rc_iter);

  if (pjob == NULL)
    recycler.rc_iter = -1;
  pthread_mutex_unlock(recycler.rc_mutex);

  if (pjob != NULL)
    pjob->ji_being_recycled = FALSE;

  return(pjob);
  } /* END get_recycled_job() */



void update_recycler_next_id() 

  {
  recycler.rc_next_id++;
  } /* END update_recycler_next_id() */






/* END job_recycler.c */

