/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2010 Veridian Information Solutions, Inc.
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
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "threadpool.h"
#include "utils.h"
#include "log.h"
#include "../Liblog/pbs_log.h"
#include "../Liblog/log_event.h"



#define MINIMUM_STACK_SIZE 16777216
/*extern int    LOGLEVEL;*/
sigset_t      fillset;

threadpool_t *request_pool;

static void *work_thread(void *);

int create_work_thread(void)

  {
  int             rc;
  sigset_t        oldset;
  pthread_t       wthread;
  pthread_attr_t  attr;
  size_t          stack_size;

  if (request_pool == NULL)
    {
    initialize_threadpool(&request_pool,5,5,-1);
    }

  pthread_attr_init(&attr);
  pthread_attr_getstacksize(&attr, &stack_size);
  if (stack_size < MINIMUM_STACK_SIZE)
    stack_size = MINIMUM_STACK_SIZE;
  pthread_attr_setstacksize(&attr, stack_size);

  /* save old signal mask */
  pthread_sigmask(SIG_SETMASK,&fillset,&oldset);
  rc = pthread_create(&wthread,&request_pool->tp_attr,work_thread, &attr);
  pthread_sigmask(SIG_SETMASK,&oldset,NULL);
  
  return(rc);
  } /* END create_work_thread() */




static void work_thread_cleanup(
  
  void *a)

  {
  --request_pool->tp_nthreads;

  if (request_pool->tp_flags & POOL_DESTROY)
    {
    if (request_pool->tp_nthreads == 0)
      pthread_cond_broadcast(&request_pool->tp_can_destroy);
    }
  else if (request_pool->tp_nthreads == 0)
    {
    if (create_work_thread() == 0)
      request_pool->tp_nthreads++;
    }
  else if ((request_pool->tp_first != NULL) &&
           (request_pool->tp_nthreads < request_pool->tp_min_threads) &&
           (create_work_thread() == 0))
    {
    request_pool->tp_nthreads++;
    }

  pthread_mutex_unlock(&request_pool->tp_mutex);
  }




void work_cleanup(
    
  void *a)

  {
  pthread_t   my_id = pthread_self();
  tp_working_t  *curr;
  tp_working_t  *prev = NULL;

  pthread_mutex_lock(&request_pool->tp_mutex);

  /* make sure the active process gets removed */
  curr = request_pool->tp_active;

  while (curr != NULL)
    {
    if (curr->working_id == my_id)
      {
      /* handle if it's first in the list */
      if (prev == NULL)
        {
        request_pool->tp_active = curr->next;
        }
      /* anywhere after first position */
      else
        {
        prev->next = curr->next;
        }

      /* all done */       
      break;
      }

    prev = curr;
    curr = curr->next;
    }

  } /* END work_cleanup() */






static void *work_thread(

  void *a)

  {
  static char      *id = "work_thread";

  int               rc;

  void             *(*func)(void *);
  void             *arg;
  tp_work_t        *mywork;
  tp_working_t      working;
  /*char             *tid = (char *)pthread_self();*/

  struct timespec   ts;
  /*char              log_buf[LOCAL_LOG_BUF_SIZE];*/

  if (request_pool == NULL)
    {
    log_err(-1,id,"Pool doesn't exist, and thread is active??\nTerminating");
    return(NULL);
    }

  pthread_mutex_lock(&request_pool->tp_mutex);
  pthread_cleanup_push(work_thread_cleanup,a);
  working.working_id = pthread_self();

  /* this is the main work loop, which is only exited on timeout, if 
   * a timeout is configured */
  for (;;) 
    {
    /* reset signal mask for each job */
    pthread_sigmask(SIG_SETMASK,&fillset,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);

    request_pool->tp_idle_threads++;
  
    /* stay asleep until the pool is started */
    while (request_pool->tp_started == FALSE)
      {
      pthread_mutex_unlock(&request_pool->tp_mutex);
      
      sleep(1);
      
      pthread_mutex_lock(&request_pool->tp_mutex);
      }


    while ((request_pool->tp_first == NULL) &&
           (!(request_pool->tp_flags & POOL_DESTROY)))
      {
      if ((request_pool->tp_nthreads <= request_pool->tp_min_threads) ||
          (request_pool->tp_max_idle_secs < 0))
        {
        /* wait until something is ready */ 
        pthread_cond_wait(&request_pool->tp_waiting_work,&request_pool->tp_mutex);
        }
      else
        {
        clock_gettime(CLOCK_REALTIME,&ts);
        ts.tv_sec += request_pool->tp_max_idle_secs;
        rc = pthread_cond_timedwait(&request_pool->tp_waiting_work,&request_pool->tp_mutex,&ts);

        if (rc == ETIMEDOUT)
          {
          break;
          }
        }
      }

    request_pool->tp_idle_threads--;

    /* if we're shutting down, leave this loop */
    if (request_pool->tp_flags & POOL_DESTROY)
      break;

    if ((mywork = request_pool->tp_first) != NULL)
      {
      func = mywork->work_func;
      arg  = mywork->work_arg;

      request_pool->tp_first = mywork->next;
      if (request_pool->tp_last == mywork)
        request_pool->tp_last = NULL;

      working.next = request_pool->tp_active;
      request_pool->tp_active = &working;

      pthread_mutex_unlock(&request_pool->tp_mutex);
      pthread_cleanup_push(work_cleanup,NULL);
      free(mywork);

/*      if (LOGLEVEL >= 7)
        {
        sprintf(log_buf,"starting work from thread %s", tid);
        log_event(PBSEVENT_SYSTEM,PBS_EVENTCLASS_SERVER,id,log_buf);
        }*/

      /* do the work */
      func(arg);

      /*
      if (LOGLEVEL >= 7)
        {
        sprintf(log_buf,"finished work from thread %s", tid);
        log_event(PBSEVENT_SYSTEM,PBS_EVENTCLASS_SERVER,id,log_buf);
        }*/

      /* cleanup the work */
      pthread_cleanup_pop(1); /* calls work_cleanup(NULL) */
      }
    }

  pthread_cleanup_pop(1); /* calls work_thread_cleanup(NULL) */
  return(NULL);
  } /* END work_thread() */





int initialize_threadpool(

  threadpool_t **pool,           /* I/O */
  int            min_threads,    /* I */
  int            max_threads,    /* I */
  int            max_idle_time)  /* I */

  {
  int           i;
  int           rc = 0;

  sigfillset(&fillset);

  if ((min_threads > max_threads) ||
      (max_threads < 1))
    {
    /* bad parameters */
    return(EINVAL);
    }

  if ((*pool = (threadpool_t *)calloc(1, sizeof(threadpool_t))) == NULL)
    {
    return(ENOMEM);
    }

  memset(*pool,0,sizeof(threadpool_t));
  pthread_mutex_init(&(*pool)->tp_mutex,NULL);
  pthread_cond_init(&(*pool)->tp_waiting_work,NULL);
  pthread_cond_init(&(*pool)->tp_can_destroy,NULL);
  (*pool)->tp_min_threads = min_threads;
  (*pool)->tp_max_threads = max_threads;
  (*pool)->tp_max_idle_secs = max_idle_time;
  (*pool)->tp_started = FALSE;
  
  /* initialize attributes */
  pthread_attr_init(&(*pool)->tp_attr);
  pthread_attr_setdetachstate(&(*pool)->tp_attr,PTHREAD_CREATE_DETACHED);

  /* if threads are static, create them all now */
  if ((*pool)->tp_max_idle_secs < 0)
    {
    for (i = 0; i < (*pool)->tp_min_threads; i++)
      {
      if ((rc = create_work_thread()) != 0)
        {
        return(rc);
        }

      (*pool)->tp_nthreads++;
      }
    }

  return(rc);
  } /* END initialize_threadpool() */





int enqueue_threadpool_request(

  void *(*func)(void *),
  void *arg)

  {
  tp_work_t *work = NULL;

  if ((work = (tp_work_t *)calloc(1, sizeof(tp_work_t))) == NULL)
    {
    return(ENOMEM);
    }

  work->next = NULL;
  work->work_func = func;
  work->work_arg  = arg;

  pthread_mutex_lock(&request_pool->tp_mutex);

  if (request_pool->tp_first == NULL)
    request_pool->tp_first = work;
  else
    request_pool->tp_last->next = work;
  request_pool->tp_last = work;

  if (request_pool->tp_idle_threads > 0)
    pthread_cond_signal(&request_pool->tp_waiting_work);
  else if ((request_pool->tp_nthreads < request_pool->tp_max_threads) &&
           (create_work_thread() == 0))
    request_pool->tp_nthreads++;

  pthread_mutex_unlock(&request_pool->tp_mutex);

  return(0);
  } /* END enqueue_threadpool_request() */





void destroy_request_pool(void)

  {
  tp_work_t    *work;
  tp_working_t *ptr;

  pthread_mutex_lock(&request_pool->tp_mutex);

  /* set the pool to be destroyed and notify all threads */
  request_pool->tp_flags |= POOL_DESTROY;
  pthread_cond_broadcast(&request_pool->tp_waiting_work);

  /* cancel any active work */
  ptr = request_pool->tp_active;
  while (ptr != NULL)
    {
    pthread_cancel(ptr->working_id);
    ptr = ptr->next;
    }

  /* wait to be awoken */
  while (request_pool->tp_nthreads != 0)
    pthread_cond_wait(&request_pool->tp_can_destroy,&request_pool->tp_mutex);

  pthread_mutex_unlock(&request_pool->tp_mutex);

  /* free pending work */
  while ((work = request_pool->tp_first) != NULL)
    {
    request_pool->tp_first = work->next;
    free(work);
    }
  } /* END destroy_request_pool() */




void start_request_pool()

  {
  pthread_mutex_lock(&request_pool->tp_mutex);

  request_pool->tp_started = TRUE;

  pthread_mutex_unlock(&request_pool->tp_mutex);
  } /* END start_request_pool() */



/* END u_threadpool.c */

