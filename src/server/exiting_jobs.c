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
#include "id_map.hpp"



void on_job_exit(batch_request *preq, char *jobid);
void force_purge_work(job *pjob);


std::vector<job_exiting_retry_info>  exiting_jobs_info;
extern pthread_mutex_t              *exiting_jobs_info_mutex;
extern bool                          exit_called;


int record_job_as_exiting(

  job *pjob)

  {

  pthread_mutex_lock(exiting_jobs_info_mutex);

  // see if we can find an open spot
  for (unsigned int i = 0; i < exiting_jobs_info.size(); i++)
    {
    if (exiting_jobs_info[i].internal_job_id == -1)
      {
      exiting_jobs_info[i].internal_job_id = pjob->ji_internal_id;
      exiting_jobs_info[i].attempts = 0;
      exiting_jobs_info[i].last_attempt = time(NULL);

      pthread_mutex_unlock(exiting_jobs_info_mutex);
      return(PBSE_NONE);
      }
    }

  // if we reach here there are no open spots so make a new one
  job_exiting_retry_info jeri;

  jeri.attempts = 0;
  jeri.internal_job_id = pjob->ji_internal_id;
  jeri.last_attempt = time(NULL);
  exiting_jobs_info.push_back(jeri);
  pthread_mutex_unlock(exiting_jobs_info_mutex);

  return(PBSE_NONE);
  } /* END record_job_as_exiting() */



int remove_from_exiting_list_by_jobid(

  int internal_job_id)

  {
  pthread_mutex_lock(exiting_jobs_info_mutex);

  for (unsigned int i = 0; i < exiting_jobs_info.size(); i++)
    {
    if (internal_job_id == exiting_jobs_info[i].internal_job_id)
      {
      exiting_jobs_info[i].internal_job_id = -1;
      }
    }
  
  pthread_mutex_unlock(exiting_jobs_info_mutex);

  return(PBSE_NONE);
  } /* END remove_from_exiting_list_by_jobid() */



int remove_job_from_exiting_list(

  job **pjob)

  {
  int internal_job_id = (*pjob)->ji_internal_id;

  unlock_ji_mutex(*pjob,__func__, NULL, LOGLEVEL);
  int rc = remove_from_exiting_list_by_jobid(internal_job_id);
  *pjob = svr_find_job_by_id(internal_job_id);
  return rc;
  } /* END remove_job_from_exiting_list() */


int retry_job_exit(

  int internal_job_id)

  {
  char  log_buf[LOCAL_LOG_BUF_SIZE];
  char *job_id = strdup(job_mapper.get_name(internal_job_id));

  snprintf(log_buf, sizeof(log_buf), "Retrying job exiting for job %s",
    job_id);
  log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
  
  /* job_id will be freed in on_job_exit() */
  on_job_exit(NULL, job_id);

  return(PBSE_NONE);
  } /* END retry_job_exit() */



int get_next_retryable_jobid(

  unsigned int &index)

  {
  job                    *pjob;
  time_t                  time_now = time(NULL);
  char                    log_buf[LOCAL_LOG_BUF_SIZE];

  if (exit_called == true)
    return(-1);

  pthread_mutex_lock(exiting_jobs_info_mutex);

  for (; index < exiting_jobs_info.size(); index++)
    {
    job_exiting_retry_info &jeri = exiting_jobs_info[index];
    if (time_now - jeri.last_attempt > EXITING_RETRY_TIME)
      { 
      int internal_job_id = jeri.internal_job_id;
      if (internal_job_id != -1)
        {
        if (jeri.attempts >= MAX_EXITING_RETRY_ATTEMPTS)
          {
          // mark as invalid
          jeri.internal_job_id = -1;

          pthread_mutex_unlock(exiting_jobs_info_mutex);
        
          if ((pjob = svr_find_job_by_id(internal_job_id)) != NULL)
            {
            snprintf(log_buf, sizeof(log_buf),
              "Job %s has had its exiting re-tried %d times, purging.",
              job_mapper.get_name(internal_job_id), MAX_EXITING_RETRY_ATTEMPTS);
            log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);

            force_purge_work(pjob);
            }
    
          pthread_mutex_lock(exiting_jobs_info_mutex);
          }
        else
          {
          jeri.attempts++;
          jeri.last_attempt = time_now;
          pthread_mutex_unlock(exiting_jobs_info_mutex);

          return(internal_job_id);
          }
        }
      }
    }

  pthread_mutex_unlock(exiting_jobs_info_mutex);

  return(-1);
  } /* END get_next_retryable_jobid() */



/* 
 * check_exiting_jobs()
 *
 * loops over the recorded exiting job information and retries
 * jobs that have been stale long enough.
 */
int check_exiting_jobs()

  {
  unsigned int  index = 0;
  int           internal_job_id;
  job          *pjob;
  
  while ((internal_job_id = get_next_retryable_jobid(index)) != -1)
    {
    if ((pjob = svr_find_job_by_id(internal_job_id)) == NULL)
      {
      remove_from_exiting_list_by_jobid(internal_job_id);
      }
    else
      {
      mutex_mgr pjob_mutex = mutex_mgr(pjob->ji_mutex, true);

      if (pjob->ji_qs.ji_state == JOB_STATE_COMPLETE)
        {
        remove_from_exiting_list_by_jobid(internal_job_id);
        }
      else
        {
        pjob_mutex.unlock();

        /* jobid is freed in on_job_exit() */
        retry_job_exit(internal_job_id);
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




