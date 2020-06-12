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

#include <string>
#include <stdio.h>
#include <errno.h>
#include "user_info.h"
#include "svrfunc.h"
#include "array.h"
#include "server.h"

user_info_holder users;


/*
 * remove_server_suffix()
 *
 * removes the @servername suffix from user_name
 * @post-cond: user_name will have no @servername suffix
 * @param user_name - the user name that should have a suffix removed
 * if present.
 * 
 */
void remove_server_suffix(

  std::string &user_name)

  {
  size_t pos = user_name.find("@");

  if (pos != std::string::npos)
    user_name.erase(pos);
  }



unsigned int get_num_queued(
    
  user_info_holder *uih,
  const char       *user_name)

  {
  unsigned int  num_queued = 0;
  user_info    *ui = NULL;

  uih->lock();

  if ((ui = uih->find(user_name)) != NULL)
    {
    num_queued = ui->num_jobs_queued;
    }

  uih->unlock();

  return(num_queued);
  } /* END get_num_queued() */



unsigned int count_jobs_submitted(

  job *pjob)

  {
  unsigned int num_to_add = 1;

  if (pjob->ji_wattr[JOB_ATR_job_array_request].at_val.at_str != NULL)
    {
    num_to_add = num_array_jobs(pjob->ji_wattr[JOB_ATR_job_array_request].at_val.at_str);
    }

  return(num_to_add);
  } /* END count_jobs_submitted() */



int  can_queue_new_job(

  char *user_name,
  job  *pjob)

  {
  long         max_queuable = -1;
  int          can_queue_another = TRUE;
  unsigned int num_queued = 0;
  unsigned int num_to_add;
  std::string  uname(user_name);

  get_svr_attr_l(SRV_ATR_MaxUserQueuable, &max_queuable);

  remove_server_suffix(uname);

  if (max_queuable >= 0)
    {
    num_to_add = count_jobs_submitted(pjob);
    num_queued = get_num_queued(&users, uname.c_str());

    if (num_queued + num_to_add > (unsigned int)max_queuable)
      can_queue_another = FALSE;
    }
  
  return(can_queue_another);
  } /* END can_queue_new_job() */



int  increment_queued_jobs(
   
  user_info_holder *uih,
  char             *user_name,
  job              *pjob)

  {
  int           rc = PBSE_NONE;
  user_info    *ui;
  unsigned      bit = COUNTED_GLOBALLY;

  /* If pbs_server is restarting we may get jobs in a completed state.
     we do not want to count these jobs as queued */
  if (pjob->ji_qs.ji_state == JOB_STATE_COMPLETE)
    return(rc);

  if (uih != &users)
    bit = COUNTED_IN_QUEUE;

  // Prevent recounting a job  
  if (pjob->ji_queue_counted & bit)
    return(rc);
 
  // Only increment the count for array templates and normal jobs
  if ((pjob->ji_is_array_template == TRUE) ||
      ((pjob->ji_wattr[JOB_ATR_job_array_id].at_flags & ATR_VFLAG_SET) == 0))
    {
    unsigned int  num_submitted = count_jobs_submitted(pjob);
    std::string   uname(user_name);

    remove_server_suffix(uname);

    uih->lock();

    /* get the user if there is one */
    if ((ui = uih->find(uname)) != NULL)
      {
      ui->num_jobs_queued += num_submitted;
      }
    else
      {
      /* user doesn't exist, create a new one and insert */
      ui = (user_info *)calloc(1, sizeof(user_info));
      ui->user_name = strdup(uname.c_str());
      ui->num_jobs_queued = num_submitted;

      if (!uih->insert(ui, ui->user_name))
        {
        rc = ENOMEM;
        log_err(rc, __func__, "Can't resize the user info array");
        }
      }
  
    uih->unlock();
    }

  // Mark this job as being counted as queued
  pjob->ji_queue_counted |= bit;

  return(rc);
  } /* END increment_queued_jobs() */




int  decrement_queued_jobs(

  user_info_holder *uih,    
  char             *user_name,
  job              *pjob)

  {
  user_info *ui;
  int        rc = THING_NOT_FOUND;
  char       log_buf[LOCAL_LOG_BUF_SIZE];
  std::string  uname(user_name);
  
  unsigned      bit = COUNTED_GLOBALLY;

  if (uih != &users)
    bit = COUNTED_IN_QUEUE;

  // Prevent re-decrementing the same job twice
  if ((pjob->ji_queue_counted & bit) == 0)
    return(PBSE_NONE);
  
  remove_server_suffix(uname);

  uih->lock();

  if ((ui = uih->find(uname)) != NULL)
    {
    if (ui->num_jobs_queued == 0)
      {

      if (LOGLEVEL >= 6)
        {
        snprintf(log_buf, sizeof(log_buf), "decremented number of jobs queued when already at 0");
        log_event(PBSEVENT_ERROR, PBS_EVENTCLASS_JOB, __func__, log_buf);
        }
      }
    else
      {
      ui->num_jobs_queued--;
      }
    rc = PBSE_NONE;
    }

  uih->unlock();

  // Mark this job as no longer counted as queued
  pjob->ji_queue_counted &= ~bit;

  return(rc);
  } /* END decrement_queued_jobs() */



