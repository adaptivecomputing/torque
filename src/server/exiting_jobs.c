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





#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string>

#include "exiting_jobs.h"
#include "log.h"
#include "batch_request.h"
#include "ji_mutex.h"
#include "mutex_mgr.hpp"




void on_job_exit(batch_request *preq, char *jobid);
void force_purge_work(job *pjob);




int record_job_as_exiting(

  job *pjob)

  {
  job_exiting_retry_info *jeri = (job_exiting_retry_info*)calloc(1, sizeof(job_exiting_retry_info));

  if (jeri == NULL)
    return(ENOMEM);

  strcpy(jeri->jobid, pjob->ji_qs.ji_jobid);
  jeri->last_attempt = time(NULL);

  return(add_to_hash_map(exiting_jobs_info, jeri, jeri->jobid));
  } /* END record_job_as_exiting() */



int remove_from_exiting_list_by_jobid(

  const char *jobid)

  {
  pthread_mutex_lock(exiting_jobs_info->hm_mutex);
  
  job_exiting_retry_info *jeri = (job_exiting_retry_info*)get_remove_from_hash_map(exiting_jobs_info, jobid, true);

  if (jeri != NULL)
    free(jeri);
  
  pthread_mutex_unlock(exiting_jobs_info->hm_mutex);

  return(PBSE_NONE);
  } /* END remove_from_exiting_list_by_jobid() */



int remove_job_from_exiting_list(

  job **pjob)

  {
  std::string jobid((*pjob)->ji_qs.ji_jobid);

  unlock_ji_mutex(*pjob,__func__, NULL, LOGLEVEL);
  int rc = remove_from_exiting_list_by_jobid(jobid.c_str());
  *pjob = svr_find_job((char *)jobid.c_str(),FALSE);
  return rc;
  } /* END remove_job_from_exiting_list() */


int retry_job_exit(

  char *jobid)

  {
  char  log_buf[LOCAL_LOG_BUF_SIZE];

  snprintf(log_buf, sizeof(log_buf), "Retrying job exiting for job %s",
    jobid);
  log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
  
  /* jobid will be freed in on_job_exit() */
  on_job_exit(NULL, jobid);

  return(PBSE_NONE);
  } /* END retry_job_exit() */



char *get_next_retryable_jobid(

  int *iter)

  {
  job_exiting_retry_info *jeri;
  job                    *pjob;
  time_t                  time_now = time(NULL);
  char                    log_buf[LOCAL_LOG_BUF_SIZE];

  pthread_mutex_lock(exiting_jobs_info->hm_mutex);
  mutex_mgr exit_mgr(exiting_jobs_info->hm_mutex, true);

  while ((jeri = (job_exiting_retry_info *)next_from_hash_map(exiting_jobs_info, iter, true)) != NULL)
    {
    if (time_now - jeri->last_attempt > EXITING_RETRY_TIME)
      {
      if (jeri->attempts >= MAX_EXITING_RETRY_ATTEMPTS)
        {
        std::string jid(jeri->jobid);
        remove_from_hash_map(exiting_jobs_info, jeri->jobid, true);
        free(jeri);
        exit_mgr.unlock(); //Don't hold on to a mutex when trying to lock another.
        if ((pjob = svr_find_job((char *)jid.c_str(), TRUE)) != NULL)
          {
          snprintf(log_buf, sizeof(log_buf), "Job %s has had its exiting re-tried %d times, purging.",
            jid.c_str(), MAX_EXITING_RETRY_ATTEMPTS);
          log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);

          force_purge_work(pjob);
          }
        exit_mgr.lock();
        }
      else
        {
        jeri->attempts++;
        jeri->last_attempt = time_now;

        char *jobid = strdup(jeri->jobid);
        return(jobid);
        }
      }
    }

  return(NULL);
  } /* END get_next_retryable_jobid() */



/* 
 * check_exiting_jobs()
 *
 * loops over the recorded exiting job information and retries
 * jobs that have been stale long enough.
 */
int check_exiting_jobs()

  {
  int                     iter = -1;
  char                   *jobid;
  job                    *pjob;
  
  while ((jobid = get_next_retryable_jobid(&iter)) != NULL)
    {
    if ((pjob = svr_find_job(jobid, TRUE)) == NULL)
      {
      remove_from_exiting_list_by_jobid(jobid);
      free(jobid);
      }
    else
      {
      mutex_mgr pjob_mutex = mutex_mgr(pjob->ji_mutex, true);

      if (pjob->ji_qs.ji_state == JOB_STATE_COMPLETE)
        {
        remove_from_exiting_list_by_jobid(jobid);
        free(jobid);
        }
      else
        {
        pjob_mutex.unlock();

        /* jobid is freed in on_job_exit() */
        retry_job_exit(jobid);
        }
      }
    } /* END loop over exiting job information */

  return(PBSE_NONE);
  } /* END check_exiting_jobs() */




/*
 * inspect_exiting_jobs
 * looks at jobs marked as exiting and retries mom-communication as needed
 *
 * @param vp - not used
 */

void *inspect_exiting_jobs(

  void *vp)

  {
  while (1)
    {
    check_exiting_jobs();

    sleep(EXITING_SLEEP_TIME);
    }

  return(NULL);
  } /* END inspect_exiting_jobs() */




