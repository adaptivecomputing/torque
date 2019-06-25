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
 * queue_func.c - various functions dealing with queues
 *
 * Included functions are:
 * que_alloc() - allocacte and initialize space for queue structure
 * que_free() - free queue structure
 * que_purge() - remove queue from server
 * find_queuebyname() - find a queue with a given name
 * get_dfltque() - get default queue
 */

#include <pbs_config.h>   /* the master config generated by configure */
#include "queue_func.h"

#include <sys/param.h>
#include <memory.h>
#include <stdlib.h>
#include "pbs_ifl.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "list_link.h"
#include "log.h"
#include "../lib/Liblog/pbs_log.h"
#include "attribute.h"
#include "server_limits.h"
#include "server.h"
#include "queue.h"
#include "pbs_job.h"
#include "pbs_error.h"
#if __STDC__ != 1
#include <memory.h>
#endif
#include <pthread.h>
#include "queue_recycler.h"
#include "svrfunc.h"
#include "svr_func.h" /* get_svr_attr_* */
#include "ji_mutex.h"
#include "mutex_mgr.hpp"


#define MSG_LEN_LONG 160

/* Global Data */

extern int LOGLEVEL;

extern char     *msg_err_unlink;
extern char *path_queues;

extern struct    server server;
extern queue_recycler q_recycler;

int lock_queue(

  struct pbs_queue *the_queue,
  const char       *id,
  const char       *msg,
  int               logging)

  {
  int   rc = PBSE_NONE;
  char *err_msg = NULL;

  if (logging >= 10)
    { 
    err_msg = (char *)calloc(1, MSG_LEN_LONG);
    snprintf(err_msg, MSG_LEN_LONG, "locking %s in method %s", the_queue->qu_qs.qu_name, id);
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
    }

  if (pthread_mutex_lock(the_queue->qu_mutex) != 0)
    {
    if (logging >= 10)
      {
      snprintf(err_msg, MSG_LEN_LONG, "ALERT: cannot lock queue %s mutex in method %s",
          the_queue->qu_qs.qu_name, id);
      log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
      }
    rc = PBSE_MUTEX;
    }

  if (err_msg != NULL)
    free(err_msg);

  return rc;
  }

int unlock_queue(

  struct pbs_queue *the_queue,
  const char       *id,
  const char       *msg,
  int               logging)

  {
  int rc = PBSE_NONE;
  char *err_msg = NULL;
  char stub_msg[] = "no pos";

  if (logging >= 10)
    {
    err_msg = (char *)calloc(1, MSG_LEN_LONG);
    if (msg == NULL)
      msg = stub_msg;
    snprintf(err_msg, MSG_LEN_LONG, "unlocking %s in method %s-%s", the_queue->qu_qs.qu_name, id, msg);
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
    }

  if (pthread_mutex_unlock(the_queue->qu_mutex) != 0)
    {
    if (logging >= 10)
      {
      snprintf(err_msg, MSG_LEN_LONG, "ALERT: cannot unlock queue %s mutex in method %s",
          the_queue->qu_qs.qu_name, id);
      log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
      }
    rc = PBSE_MUTEX;
    }

  if (err_msg != NULL)
    free(err_msg);

  return rc;
  }


/*
 * que_alloc - allocate space for a queue structure and initialize
 * attributes to "unset"
 *
 * Returns: pointer to structure or null is space not available.
 */

pbs_queue *que_alloc(

  const char *name,
  int   sv_qs_mutex_held)

  {
  static char *mem_err = (char *)"no memory";

  int        i;
  pbs_queue *pq;

  pq = (pbs_queue *)calloc(1, sizeof(pbs_queue));

  if (pq == NULL)
    {
    log_err(errno, __func__, mem_err);

    return(NULL);
    }

  pq->qu_qs.qu_type = QTYPE_Unset;

  pq->qu_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pq->qu_jobs = new all_jobs();
  pq->qu_jobs_array_sum = new all_jobs();
  
  if ((pq->qu_mutex == NULL) ||
      (pq->qu_jobs == NULL) ||
      (pq->qu_jobs_array_sum == NULL))
    {
    log_err(ENOMEM, __func__, mem_err);
    free(pq);
    return(NULL);
    }

  pthread_mutex_init(pq->qu_mutex,NULL);
  lock_queue(pq, __func__, NULL, LOGLEVEL);

  snprintf(pq->qu_qs.qu_name, sizeof(pq->qu_qs.qu_name), "%s", name);

  insert_queue(&svr_queues,pq);
 
  if (sv_qs_mutex_held == FALSE)
    lock_sv_qs_mutex(server.sv_qs_mutex, __func__);
  server.sv_qs.sv_numque++;
  if (sv_qs_mutex_held == FALSE)
    unlock_sv_qs_mutex(server.sv_qs_mutex, __func__);

  /* set up the user info struct */
  pq->qu_uih = new user_info_holder();

  /* set the working attributes to "unspecified" */

  for (i = 0; i < QA_ATR_LAST; i++)
    {
    clear_attr(&pq->qu_attr[i], &que_attr_def[i]);
    }

  /* Set route_retry_thread_id in case this is a routing queue */
  pq->route_retry_thread_id = -1;

  return(pq);
  }  /* END que_alloc() */




/*
 * que_free - free queue structure and its various sub-structures
 */

void que_free(

  pbs_queue *pq,
  int        sv_qs_mutex_held)

  {
  int            i;
  pbs_attribute *pattr;
  attribute_def *pdef;

  int rc;
  boost::shared_ptr<mutex_mgr> pq_mutex = create_managed_mutex(pq->qu_mutex, true, rc);
  if (rc != PBSE_NONE)
    {
    char log_buf[LOCAL_LOG_BUF_SIZE];

    sprintf(log_buf, "Failed to allocate queue mutex for %s", pq->qu_qs.qu_name);
	log_err(rc, __func__, log_buf);
    throw std::runtime_error(log_buf);
    }


  /* remove any calloc working pbs_attribute space */
  for (i = 0;i < QA_ATR_LAST;i++)
    {
    pdef  = &que_attr_def[i];
    pattr = &pq->qu_attr[i];

    pdef->at_free(pattr);

    /* remove any acl lists associated with the queue */

    if (pdef->at_type == ATR_TYPE_ACL)
      {
      pattr->at_flags |= ATR_VFLAG_MODIFY;

      save_acl(pattr, pdef, pdef->at_name, pq->qu_qs.qu_name);
      }
    }

  /* now free the main structure */
  if (sv_qs_mutex_held == FALSE)
    lock_sv_qs_mutex(server.sv_qs_mutex, __func__);
  server.sv_qs.sv_numque--;
  if (sv_qs_mutex_held == FALSE)
    unlock_sv_qs_mutex(server.sv_qs_mutex, __func__);

  delete pq->qu_uih;

  remove_queue(&svr_queues, pq);
  pq->q_being_recycled = TRUE;
  insert_into_queue_recycler(pq);
  return;
  }  /* END que_free() */




/*
 * que_purge - purge queue from system
 *
 * The queue is dequeued, the queue file is unlinked.
 * If the queue contains any jobs, the purge is not allowed.
 */

int que_purge(

  pbs_queue *pque)

  {
  char     namebuf[MAXPATHLEN];
  char     log_buf[LOCAL_LOG_BUF_SIZE];

  if (pque->qu_numjobs != 0)
    {
    return(PBSE_QUEBUSY);
    }

  snprintf(namebuf, sizeof(namebuf), "%s%s", path_queues, pque->qu_qs.qu_name);

  if (unlink(namebuf) < 0)
    {
    sprintf(log_buf, msg_err_unlink, "Queue", namebuf);

    log_err(errno, "queue_purge", log_buf);
    }

  try
	{
	que_free(pque, FALSE);
	}
  catch(...)
	{
	return(PBSE_SYSTEM);
	}

  return(0);
  }





/*
 * find_queuebyname() - find a queue by its name
 */

pbs_queue *find_queuebyname(

  const char *quename) /* I */

  {
  char  *pc;
  pbs_queue *pque = NULL;
  char   qname[PBS_MAXDEST + 1];

  snprintf(qname, sizeof(qname), "%s", quename);

  pc = strchr(qname, (int)'@'); /* strip off server (fragment) */

  if (pc != NULL)
    *pc = '\0';

  svr_queues.lock();

  pque = svr_queues.find(qname);

  svr_queues.unlock();
  
  if (pque != NULL)
    {
    if (pque->q_being_recycled)
      {
      pque = NULL;
      }
    else
      {
      lock_queue(pque, __func__, NULL, LOGLEVEL);
      }
    }

  if (pc != NULL)
    *pc = '@'; /* restore '@' server portion */

  return(pque);
  }  /* END find_queuebyname() */





/*
 * get_dfltque - get the default queue (if declared)
 */

pbs_queue *get_dfltque(void)

  {
  pbs_queue *pq = NULL;
  char      *dque = NULL;

  if (get_svr_attr_str(SRV_ATR_dflt_que, &dque) == PBSE_NONE)
    {
    pq = find_queuebyname(dque);
    }

  return(pq);
  }  /* END get_dfltque() */



int insert_queue(

  all_queues *aq,
  pbs_queue  *pque)

  {
  int          rc;

  aq->lock();

  if(!aq->insert(pque,pque->qu_qs.qu_name))
    {
    rc = ENOMEM;
    log_err(rc, __func__, "No memory to resize the array");
    }
  else
    {
    rc = PBSE_NONE;
    }

  aq->unlock();

  return(rc);
  } /* END insert_queue() */





int remove_queue(

  all_queues *aq,
  pbs_queue  *pque)

  {
  int  rc = PBSE_NONE;
  char log_buf[1000];

  if (aq->trylock())
    {
    unlock_queue(pque, __func__, NULL, LOGLEVEL);
    aq->lock();
    lock_queue(pque, __func__, NULL, LOGLEVEL);
    }

  if(!aq->remove(pque->qu_qs.qu_name))
    rc = THING_NOT_FOUND;

  snprintf(log_buf, sizeof(log_buf), "name = %s", pque->qu_qs.qu_name);
  log_err(-1, __func__, log_buf);

  aq->unlock();

  return(rc);
  } /* END remove_queue() */





pbs_queue *next_queue(

  all_queues          *aq,
  all_queues_iterator *iter)

  {
  pbs_queue *pque;

  aq->lock();

  pque = iter->get_next_item();
  if (pque != NULL)
    lock_queue(pque, "next_queue", (char *)NULL, LOGLEVEL);
  aq->unlock();

  if (pque != NULL)
    {
    if (pque->q_being_recycled != FALSE)
      {
      unlock_queue(pque, __func__, "recycled queue", LOGLEVEL);
      pque = next_queue(aq, iter);
      }
    }

  return(pque);
  } /* END next_queue() */




/* 
 * gets the locks on both queues without releasing the all_queues mutex lock.
 * Doing this another way can cause deadlock.
 *
 * @return PBSE_NONE on success
 */

int get_parent_dest_queues(

  char       *queue_parent_name,
  char       *queue_dest_name,
  pbs_queue **parent,
  pbs_queue **dest,
  job       **pjob_ptr)

  {
  pbs_queue *pque_parent;
  pbs_queue *pque_dest;
  char       jobid[PBS_MAXSVRJOBID + 1];
  char       log_buf[LOCAL_LOG_BUF_SIZE + 1];
  job       *pjob = *pjob_ptr;
  int        rc = PBSE_NONE;

  strcpy(jobid, pjob->ji_qs.ji_jobid);
  *dest   = NULL;

  if ((queue_parent_name != NULL) && (queue_dest_name != NULL))
    {
    if (!strcmp(queue_parent_name, queue_dest_name))
      {
      /* parent and destination are the same. 
         Job is already in destnation queue. return */
      snprintf(log_buf, sizeof(log_buf), "parent and destination queues are the same: parent %s - dest %s. jobid: %s",
          queue_parent_name,
          queue_dest_name,
          jobid);
      log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
      return(-1); 
      }
    }
  else
    return(-1);

  unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);

  unlock_queue(*parent, __func__, NULL, LOGLEVEL);

  *parent = NULL;

  svr_queues.lock();

  pque_parent = svr_queues.find(queue_parent_name);
  pque_dest   = svr_queues.find(queue_dest_name);

  if ((pque_parent == NULL) ||
      (pque_dest == NULL))
    {
    rc = -1;
    }
  else
    {
    /* SUCCESS! */
    if (LOGLEVEL >= 6)
      {
      snprintf(log_buf, sizeof(log_buf), "Job %s successfully routed:  %s (%p) -> %s (%p)",
               jobid, queue_parent_name, (void *)pque_parent, queue_dest_name, (void *)pque_dest);
      log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
      }
    lock_queue(pque_parent, __func__, NULL, LOGLEVEL);
    lock_queue(pque_dest,   __func__, (char *)NULL, LOGLEVEL);
    *parent = pque_parent;
    *dest = pque_dest;

    rc = PBSE_NONE;
    }

  svr_queues.unlock();

  if ((*pjob_ptr = svr_find_job(jobid, TRUE)) == NULL)
    rc = -1;

  return(rc);
  } /* END get_parent_dest_queues() */





pbs_queue *lock_queue_with_job_held(

  pbs_queue  *pque,
  job       **pjob_ptr)

  {
  char       jobid[PBS_MAXSVRJOBID + 1];
  job       *pjob = *pjob_ptr;
  char       log_buf[LOCAL_LOG_BUF_SIZE];

  if (pque != NULL)
    {
    if (pthread_mutex_trylock(pque->qu_mutex))
      {
      /* if fail */
      strcpy(jobid, pjob->ji_qs.ji_jobid);
      unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
      lock_queue(pque, __func__, NULL, LOGLEVEL);

      if ((pjob = svr_find_job(jobid, TRUE)) == NULL)
        {
        unlock_queue(pque, __func__, NULL, LOGLEVEL);
        pque = NULL;
        *pjob_ptr = NULL;
        }
      }
    else
      {
      if (LOGLEVEL >= 10)
        {
        snprintf(log_buf, sizeof(log_buf), "try lock succeeded for queue %s on job %s", pque->qu_qs.qu_name, pjob->ji_qs.ji_jobid);
        log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
        }
      }
    }

  return(pque);
  } /* END lock_queue_with_job_held() */



/* END queue_func.c */

