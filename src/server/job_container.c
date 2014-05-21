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
* without reference to its choice of law rules
*/


/* job_container.c contains functions for working with the all_jobs struct, 
 * such as finding a job in the server's global list, adding jobs to that
 * list, removing from it, iterating over it, etc.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "pbs_job.h"
#include "log.h"
#include "server.h"
#include "svrfunc.h"
#include "ji_mutex.h"




extern char            server_name[];
extern struct all_jobs alljobs;
extern struct all_jobs array_summary;


/*
 * get_correct_jobname() - makes sure the job searches for the correct name
 * necessary because of SRV_ATR_display_job_server_suffix and
 * SRV_ATR_job_suffix_alias
 *
 * allocs the correct job name
 * @param jobid (I) - the jobid as passed in (NUM.SERVER_NAME)
 * @return a pointer to the correct job name (alloc'd)
 */
char *get_correct_jobname(

  const char *jobid) /* I */

  {
  char *correct = NULL;
  char *dot;
  char *work_jobid = strdup(jobid);
  /* first suffix could be the server name or the alias */
  char *first_suffix = NULL;

  /* second suffix can only be the alias */
  char *second_suffix = NULL;
  int   server_suffix = TRUE;

  int len;

  long  display_suffix = TRUE;
  char *alias = NULL;

  get_svr_attr_l(SRV_ATR_display_job_server_suffix, &display_suffix);
  if (display_suffix == FALSE)
    server_suffix = FALSE;

  if ((dot = strchr((char *)work_jobid,'.')) != NULL)
    {
    first_suffix = dot + 1;

    if ((dot = strchr(first_suffix,'.')) != NULL)
      {
      second_suffix = dot + 1;
      }
  
    dot = NULL;
    }

  /* check current settings */
  get_svr_attr_str(SRV_ATR_job_suffix_alias, &alias);
  if ((alias != NULL) &&
      (server_suffix == TRUE))
    {
    /* display the server suffix and the alias */

    /* check if alias is already there */
    if (second_suffix != NULL)
      {
      if (strcmp(second_suffix,alias) == 0)
        {
        correct = strdup(jobid);

        if (correct == NULL)
          log_err(-1, __func__, "ERROR:    Fatal - Cannot allocate memory\n");
      
        free(work_jobid);

        return(correct);
        }
      }
    else if (first_suffix == NULL)
      {
      /* alloc memory and sprint, add 3 for 2 '.' and NULL terminator */
      len = strlen(work_jobid) + strlen(server_name) + strlen(alias) + 3;
      correct = (char *)calloc(1, len);

      if (correct == NULL)
        {
        log_err(-1, __func__, "ERROR:    Fatal - Cannot allocate memory\n");
        free(work_jobid);
        return(NULL);
        }

      snprintf(correct,len,"%s.%s.%s",
        work_jobid,server_name,alias);
      }
    else
      {
      /* add 2 for null terminator and '.' */
      len = strlen(alias) + 2 + strlen(work_jobid);

      correct = (char *)calloc(1, len);

      if (correct == NULL)
        {
        log_err(-1, __func__, "ERROR:    Fatal - Cannot allocate memory\n");
        free(work_jobid);
        return(NULL);
        }

      snprintf(correct,len,"%s.%s",work_jobid,alias);
      }
    } /* END if (server_suffix && alias) */
  else if (server_suffix == TRUE)
    {
    /* just the server suffix */

    if (first_suffix != NULL)
      {
      correct = strdup(work_jobid);

      if (correct == NULL)
        {
        log_err(-1, __func__, "ERROR:    Fatal - Cannot allocate memory\n");
        free(work_jobid);
        return(NULL);
        }
      }
    else
      {
      len = strlen(work_jobid) + strlen(server_name) + 2;

      correct = (char *)calloc(1, len);

      if (correct == NULL)
        {
        log_err(-1, __func__, "ERROR:    Fatal - Cannot allocate memory\n");
        free(work_jobid);
        return(NULL);
        }

      snprintf(correct,len,"%s.%s",
        work_jobid,server_name);
      }
    } /* END if (just server_suffix) */
  else if (alias != NULL)
    {
    /* just the alias, not the server */

    if (first_suffix == NULL)
      {
      len = strlen(work_jobid) + strlen(alias) + 2;

      correct = (char *)calloc(1, len);

      if (correct == NULL)
        {
        log_err(-1, __func__, "ERROR:    Fatal - Cannot allocate memory\n");
        free(work_jobid);
        return(NULL);
        }

      snprintf(correct,len,"%s.%s",work_jobid,alias);
      }
    else
      {
      len = strlen(alias) + 2;

      dot = first_suffix - 1;
      *dot = '\0';

      len += strlen(work_jobid);
      correct = (char *)calloc(1, len);

      if (correct == NULL)
        {
        log_err(-1, __func__, "ERROR:    Fatal - Cannot allocate memory\n");
        free(work_jobid);
        return(NULL);
        }

      snprintf(correct,len,"%s.%s",
        work_jobid,
        alias);

      *dot = '.';
      }
    } /* END else if (just alias) */
  else
    {
    /* no server suffix nor alias */
    if (first_suffix != NULL)
      {
      dot = first_suffix - 1;
      *dot = '\0';
      }

    len = strlen(work_jobid) + 1;
    correct = (char *)calloc(1, len);

    if (correct == NULL)
      {
      log_err(-1, __func__, "ERROR:    Fatal - Cannot allocate memory\n");
      free(work_jobid);
      return(NULL);
      }

    snprintf(correct,len,"%s",work_jobid);

    if (first_suffix != NULL)
      *dot = '.';
    }

  free(work_jobid);

  return(correct);
  } /* END get_correct_jobname() */


/*
 * Searches the array passed in for the job_id
 * @parent svr_find_job()
 */

job *find_job_by_array(
    
  struct all_jobs *aj,
  char            *job_id,
  int              get_subjob)

  {
  job *pj = NULL;
  int  i;

  if (aj == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "null struct all_jobs pointer fail");
    return(NULL);
    }
  if (job_id == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "null job_id pointer fail");
    return(NULL);
    }

  pthread_mutex_lock(aj->alljobs_mutex);
  
  i = get_value_hash(aj->ht, job_id);
  
  if (i >= 0)
    pj = (job *)aj->ra->slots[i].item;
  if (pj != NULL)
    lock_ji_mutex(pj, __func__, NULL, LOGLEVEL);
  
  pthread_mutex_unlock(aj->alljobs_mutex);
  
  if (pj != NULL)
    {
    if (get_subjob == TRUE)
      {
      if (pj->ji_cray_clone != NULL)
        {
        pj = pj->ji_cray_clone;
        unlock_ji_mutex(pj->ji_parent_job, __func__, NULL, LOGLEVEL);
        lock_ji_mutex(pj, __func__, NULL, LOGLEVEL);
        }
      }

    if (pj->ji_being_recycled == TRUE)
      {
      unlock_ji_mutex(pj, __func__, "1", LOGLEVEL);
      pj = NULL;
      }
    }

  return(pj);
  } /* END find_job_by_array() */




/*
 * svr_find_job() - find job by jobid
 *
 * Search list of all server jobs for one with same job id
 * Return NULL if not found or pointer to job struct if found
 * @param jobid - get the job whose jobid matches jobid
 * @param get_subjob - whether or not we should return the sub
 * job (for heterogeneous jobs) if there is one
 */

job *svr_find_job(

  char *jobid,      /* I */
  int   get_subjob) /* I */

  {
  char *at;
  char *comp = NULL;
  int   different = FALSE;
  char *dash = NULL;
  char *dot = NULL;
  char  without_dash[PBS_MAXSVRJOBID + 1];

  job  *pj = NULL;

  if (LOGLEVEL >= 10)
    LOG_EVENT(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, jobid);

  if ((at = strchr(jobid, '@')) != NULL)
    *at = '\0'; /* strip off @server_name */

  /* jobid-0.server indicates the external sub-job of a heterogeneous
   * job. For this case we want to get jobid.server, find that, and 
   * the get the external sub-job */
  if (get_subjob == TRUE)
    {
    dot = strchr(jobid, '.');

    if (((dash = strchr(jobid, '-')) != NULL) &&
        (dot != NULL) &&
        (dash < dot))
      {
      *dash = '\0';
      snprintf(without_dash, sizeof(without_dash), "%s%s", jobid, dash + 2);
      jobid = without_dash;
      }
    else
      dash = NULL;
    }

  if ((is_svr_attr_set(SRV_ATR_display_job_server_suffix)) ||
      (is_svr_attr_set(SRV_ATR_job_suffix_alias)))
    {
    comp = get_correct_jobname(jobid);
    different = TRUE;

    if (comp == NULL)
      return(NULL);
    }
  else
    {
    comp = jobid;
    }

  if (strstr(jobid,"[]") == NULL)
    {
    /* if we're searching for the external we want find_job_by_array to 
     * return the parent, but if we're searching for the cray subjob then
     * we want find_job_by_array to return the sub job */
    pj = find_job_by_array(&alljobs, comp, (dash != NULL) ? FALSE : get_subjob);
    }

  /* when remotely routing jobs, they are removed from the 
   * regular job list first and the array summary after. 
   * Attempt to find them there if NULL
   * OR it's an array, try to find the job */
  if (pj == NULL)
    {
    /* see the comment on the above call to find_job_by_array() */
    pj = find_job_by_array(&array_summary, comp, (dash != NULL) ? FALSE : get_subjob);
    }

  if (at)
    *at = '@'; /* restore @server_name */

  if ((get_subjob == TRUE) &&
      (pj != NULL))
    {
    if (dash != NULL)
      {
      *dash = '-';
      
      if (pj->ji_external_clone != NULL)
        {
        pj = pj->ji_external_clone;
        
        lock_ji_mutex(pj, __func__, NULL, LOGLEVEL);
        unlock_ji_mutex(pj->ji_parent_job, __func__, NULL, LOGLEVEL);

        if (pj->ji_being_recycled == TRUE)
          {
          unlock_ji_mutex(pj, __func__, NULL, LOGLEVEL);
          pj = NULL;
          }
        }
      else
        {
        unlock_ji_mutex(pj, __func__, NULL, LOGLEVEL);
        pj = NULL;
        }
      }
    }

  if (different)
    free(comp);

  return(pj);  /* may be NULL */
  }   /* END svr_find_job() */




/* initializes the all_jobs array */
void initialize_all_jobs_array(
    
  struct all_jobs *aj)

  {
  if (aj == NULL)
    {
    log_err(PBSE_BAD_PARAMETER,__func__,"null input job array");
    return;
    }

  aj->ra = initialize_resizable_array(INITIAL_JOB_SIZE);
  aj->ht = create_hash(INITIAL_HASH_SIZE);

  aj->alljobs_mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(aj->alljobs_mutex, NULL);
  } /* END initialize_all_jobs_array() */




/*
 * insert a new job into the array
 *
 * @param pjob - the job to be inserted
 * @return PBSE_NONE on success 
 */
int insert_job(
    
  struct all_jobs *aj, 
  job             *pjob)

  {
  int rc = -1;

  if (aj == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc,__func__,"null job array input");
    return(rc);
    }
  if (pjob == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc,__func__,"null job input");
    return(rc);
    }

  pthread_mutex_lock(aj->alljobs_mutex);

  rc = insert_thing(aj->ra,pjob);
  if (rc == -1)
    {
    rc = ENOMEM;
    log_err(rc, __func__, "No memory to resize the array...SYSTEM FAILURE\n");
    }
  else
    {
    add_hash(aj->ht, rc, pjob->ji_qs.ji_jobid);
    rc = PBSE_NONE;
    }

  pthread_mutex_unlock(aj->alljobs_mutex);

  return(rc);
  } /* END insert_job() */





/*
 * insert a new job into the array after a previous one
 *
 * @param already_in - the job this job should follow 
 * @param pjob - the job to be inserted
 * @return PBSE_NONE if the job is inserted correctly
 */
int insert_job_after(

  struct all_jobs *aj,
  job             *already_in,
  job             *pjob)

  {
  int rc = -1;
  int i = 0;

  if (aj == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc, __func__, "null job array input");
    return(rc);
    }
  if (already_in == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc, __func__, "null job after input");
    return(rc);
    }
  if (pjob == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc, __func__, "null job input");
    return(rc);
    }

  pthread_mutex_lock(aj->alljobs_mutex);

  i = get_value_hash(aj->ht,already_in->ji_qs.ji_jobid);
  
  if (i < 0)
    rc = THING_NOT_FOUND;
  else
    {
    rc = insert_thing_after(aj->ra,pjob,i);
    if (rc == -1)
      {
      rc = ENOMEM;
      log_err(rc, __func__, "No memory to resize the array...SYSTEM FAILURE");
      }
    else
      {
      add_hash(aj->ht,rc,pjob->ji_qs.ji_jobid);
      rc = PBSE_NONE;
      }
    }

  pthread_mutex_unlock(aj->alljobs_mutex);

  return(rc);
  } /* END insert_job_after() */




int insert_job_after_index(

  struct all_jobs *aj,
  int              index,
  job             *pjob)

  {
  int rc = -1;

  if (aj == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc, __func__, "null job array input");
    return(rc);
    }
  if (pjob == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc, __func__, "null job input");
    return(rc);
    }

  pthread_mutex_lock(aj->alljobs_mutex);

  rc = insert_thing_after(aj->ra, pjob, index);
  if (rc == -1)
    {
    rc = ENOMEM;
    log_err(rc, __func__, "No memory to resize the array...SYSTEM FAILURE");
    }
  else
    {
    add_hash(aj->ht, rc, pjob->ji_qs.ji_jobid);
    rc = PBSE_NONE;
    }

  pthread_mutex_unlock(aj->alljobs_mutex);

  return(rc);
  } /* END insert_job_after_index() */





int insert_job_first(

  struct all_jobs *aj,
  job             *pjob)

  {
  int rc = -1;

  if (aj == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc, __func__, "null job array input");
    return(rc);
    }
  if (pjob == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc, __func__, "null job input");
    return(rc);
    }

  pthread_mutex_lock(aj->alljobs_mutex);

  rc = insert_thing_after(aj->ra,pjob,ALWAYS_EMPTY_INDEX);
  if (rc == -1)
    {
    rc = ENOMEM;
    log_err(rc, __func__, "No memory to resize the array...SYSTEM FAILURE");
    }
  else
    {
    add_hash(aj->ht,rc,pjob->ji_qs.ji_jobid);
    rc = PBSE_NONE;
    }

  pthread_mutex_unlock(aj->alljobs_mutex);

  return(rc);
  } /* END insert_job_first () */




/*
 * get the job's index in the array 
 */

int get_jobs_index(

  struct all_jobs *aj,
  job             *pjob)

  {
  int index = -1;

  if (aj == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "null job array input");
    return(-1 * PBSE_BAD_PARAMETER);
    }
  if (pjob == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "null job input");
    return(-1 * PBSE_BAD_PARAMETER);
    }

  if (pthread_mutex_trylock(aj->alljobs_mutex))
    {
    unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
    pthread_mutex_lock(aj->alljobs_mutex);
    lock_ji_mutex(pjob, __func__, NULL, LOGLEVEL);

    if (pjob->ji_being_recycled == TRUE)
      {
      pthread_mutex_unlock(aj->alljobs_mutex);
      unlock_ji_mutex(pjob, __func__, "2", LOGLEVEL);
      return(-1);
      }
    }

  index = get_value_hash(aj->ht, pjob->ji_qs.ji_jobid);
  pthread_mutex_unlock(aj->alljobs_mutex);

  return(index);
  } /* END get_jobs_index() */




/*
 * check if an object is in the all_jobs object
 */

int has_job(

  struct all_jobs *aj,
  job             *pjob)

  {
  int  rc = -1;
  char jobid[PBS_MAXSVRJOBID + 1];

  if (aj == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc,__func__,"null job array input");
    return(rc);
    }
  if (pjob == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc,__func__,"null job input");
    return(rc);
    }

  strcpy(jobid, pjob->ji_qs.ji_jobid);

  if (pthread_mutex_trylock(aj->alljobs_mutex))
    {
    unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
    pthread_mutex_lock(aj->alljobs_mutex);
    lock_ji_mutex(pjob, __func__, NULL, LOGLEVEL);

    if (pjob->ji_being_recycled == TRUE)
      {
      pthread_mutex_unlock(aj->alljobs_mutex);
      unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);

      return(PBSE_JOB_RECYCLED);
      }
    }

  if (get_value_hash(aj->ht, pjob->ji_qs.ji_jobid) < 0)
    rc = FALSE;
  else
    rc = TRUE;

  pthread_mutex_unlock(aj->alljobs_mutex);

  return(rc);
  } /* END has_job() */





/* 
 * remove a job from the array
 *
 * @param pjob - the job to remove
 * @return PBSE_NONE if the job is removed 
 */

int  remove_job(
   
  struct all_jobs *aj, 
  job             *pjob)

  {
  int rc = PBSE_NONE;
  int index;

  if (pjob == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc,__func__,"null input job pointer fail");
    return(rc);
    }
  if (aj == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc,__func__,"null input array pointer fail");
    return(rc);
    }

  if (LOGLEVEL >= 10)
    LOG_EVENT(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, pjob->ji_qs.ji_jobid);
  if (pthread_mutex_trylock(aj->alljobs_mutex))
    {
    unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
    pthread_mutex_lock(aj->alljobs_mutex);
    lock_ji_mutex(pjob, __func__, NULL, LOGLEVEL);

    if (pjob->ji_being_recycled == TRUE)
      {
      pthread_mutex_unlock(aj->alljobs_mutex);
      unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
      return(PBSE_JOB_RECYCLED);
      }
    }

  if ((index = get_value_hash(aj->ht,pjob->ji_qs.ji_jobid)) < 0)
    rc = THING_NOT_FOUND;
  else
    {
    remove_thing_from_index(aj->ra,index);
    remove_hash(aj->ht,pjob->ji_qs.ji_jobid);
    }

  pthread_mutex_unlock(aj->alljobs_mutex);

  return(rc);
  } /* END remove_job() */





job *next_job(

  struct all_jobs *aj,
  int             *iter)

  {
  job *pjob;

  if (aj == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "null input pointer to all_jobs struct");
    return(NULL);
    }
  if (iter == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "null input iterator");
    return(NULL);
    }

  pthread_mutex_lock(aj->alljobs_mutex);

  pjob = (job *)next_thing(aj->ra,iter);

  pthread_mutex_unlock(aj->alljobs_mutex);

  if (pjob != NULL)
    {
    lock_ji_mutex(pjob, __func__, NULL, LOGLEVEL);

    if (pjob->ji_being_recycled == TRUE)
      {
      unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);

      pjob = next_job(aj,iter);
      }
    }

  return(pjob);
  } /* END next_job() */





job *next_job_from_back(

  struct all_jobs *aj,
  int             *iter)

  {
  job *pjob;

  if (aj == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "null input pointer to all_jobs struct");
    return(NULL);
    }
  if (iter == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "null input iterator");
    return(NULL);
    }

  pthread_mutex_lock(aj->alljobs_mutex);

  pjob = (job *)next_thing_from_back(aj->ra,iter);
  if (pjob != NULL)
    lock_ji_mutex(pjob, __func__, NULL, LOGLEVEL);

  pthread_mutex_unlock(aj->alljobs_mutex);

  if (pjob != NULL)
    {
    if (pjob->ji_being_recycled == TRUE)
      {
      unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);

      pjob = next_job_from_back(aj,iter);
      }
    }

  return(pjob);
  } /* END next_job_from_back() */




/* currently this function can only be called for jobs in the alljobs array */
int swap_jobs(

  struct all_jobs *aj,
  job             *job1,
  job             *job2)

  {
  int rc = -1;
  int new1;
  int new2;

  if (job1 == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc,__func__,"null input pointer to job1");
    return(rc);
    }
  if (job2 == NULL)
    {
    rc = PBSE_BAD_PARAMETER;
    log_err(rc,__func__,"null input pointer to job2");
    return(rc);
    }

  if (aj == NULL)
    {
    aj = &alljobs;
    }

  pthread_mutex_lock(aj->alljobs_mutex);

  new2 = get_value_hash(aj->ht,job1->ji_qs.ji_jobid);
  new1 = get_value_hash(aj->ht,job2->ji_qs.ji_jobid);

  if ((new1 == -1) ||
      (new2 == -1))
    {
    rc = THING_NOT_FOUND;
    }
  else
    {
    rc = swap_things(aj->ra,job1,job2);
    
    change_value_hash(aj->ht,job1->ji_qs.ji_jobid,new1);
    change_value_hash(aj->ht,job2->ji_qs.ji_jobid,new2);
    }

  pthread_mutex_unlock(aj->alljobs_mutex);
  
  return(rc);
  } /* END swap_jobs() */
