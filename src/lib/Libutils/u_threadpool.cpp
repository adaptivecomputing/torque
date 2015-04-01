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
#include "attribute.h"

#define MINIMUM_STACK_SIZE 1024 * 1024 
#define MAX_STACK_SIZE MINIMUM_STACK_SIZE * 8
sigset_t      fillset;

threadpool_t *request_pool;
threadpool_t *task_pool;
threadpool_t *async_pool;

static void *work_thread(void *);


/*
 * create_work_thread()
 *
 * NOTE: this function should only be called when the lock on tp is already held
 * @param - tp the threadpool that should create a work thread
 * @return the result of the call to pthread_create
 */

int create_work_thread(
    
  threadpool_t *tp)

  {
  int             rc;
  sigset_t        oldset;
  pthread_t       wthread;

  if (tp == NULL)
    {
    initialize_threadpool(&tp, 5, 5, -1);
    }

  /* save old signal mask */
  pthread_sigmask(SIG_SETMASK,&fillset,&oldset);
  rc = pthread_create(&wthread, &tp->tp_attr, work_thread, tp);
  pthread_sigmask(SIG_SETMASK,&oldset,NULL);
  
  return(rc);
  } /* END create_work_thread() */



/*
 * Guaranteed to be called whenever a worker thread exits
 *
 * Updates the threadpool variables and takes appropriate actions
 * NOTE: tp is locked at the time that this is called from the worker thread
 * @param a - a void pointer to the threadpool we're dealing with
 */

static void work_thread_cleanup(
  
  void *a)

  {
  threadpool_t *tp = (threadpool_t *)a;

  --tp->tp_nthreads;

  if (tp->tp_flags & POOL_DESTROY)
    {
    if (tp->tp_nthreads == 0)
      pthread_cond_broadcast(&tp->tp_can_destroy);
    }
  else if (tp->tp_nthreads == 0)
    {
    if (create_work_thread(tp) == 0)
      tp->tp_nthreads++;
    }
  else if ((tp->tp_first != NULL) &&
           (tp->tp_nthreads < tp->tp_min_threads) &&
           (create_work_thread(tp) == 0))
    {
    tp->tp_nthreads++;
    }

  pthread_mutex_unlock(&tp->tp_mutex);
  } /* END work_thread_cleanup() */




void work_cleanup(
    
  void *a)

  {
  threadpool_t *tp = (threadpool_t *)a;
  pthread_t   my_id = pthread_self();
  tp_working_t  *curr;
  tp_working_t  *prev = NULL;

  pthread_mutex_lock(&tp->tp_mutex);

  /* make sure the active process gets removed */
  curr = tp->tp_active;

  while (curr != NULL)
    {
    if (curr->working_id == my_id)
      {
      /* handle if it's first in the list */
      if (prev == NULL)
        {
        tp->tp_active = curr->next;
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
  threadpool_t     *tp = (threadpool_t *)a;
  int               rc = PBSE_NONE;

  void             *(*func)(void *);
  void             *arg;
  tp_work_t        *mywork;
  tp_working_t      working;

  struct timespec   ts;

  if (tp == NULL)
    {
    log_err(-1,__func__, "Pool doesn't exist, and thread is active??\nTerminating");
    return(NULL);
    }

  pthread_mutex_lock(&tp->tp_mutex);
  pthread_cleanup_push(work_thread_cleanup, tp);
  working.working_id = pthread_self();

  /* this is the main work loop, which is only exited on timeout, if 
   * a timeout is configured */
  for (;;) 
    {
    /* reset signal mask for each job */
    pthread_sigmask(SIG_SETMASK,&fillset,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);

    tp->tp_idle_threads++;
  
    /* stay asleep until the pool is started */
    while (tp->tp_started == FALSE)
      {
      pthread_mutex_unlock(&tp->tp_mutex);
      
      sleep(1);
      
      pthread_mutex_lock(&tp->tp_mutex);
      }


    while ((tp->tp_first == NULL) &&
           (!(tp->tp_flags & POOL_DESTROY)))
      {
      if ((tp->tp_nthreads <= tp->tp_min_threads) ||
          (tp->tp_max_idle_secs < 0))
        {
        /* wait until something is ready */ 
        pthread_cond_wait(&tp->tp_waiting_work, &tp->tp_mutex);
        }
      else
        {
        clock_gettime(CLOCK_REALTIME,&ts);
        ts.tv_sec += tp->tp_max_idle_secs;
        rc = pthread_cond_timedwait(&tp->tp_waiting_work, &tp->tp_mutex, &ts);

        if (rc == ETIMEDOUT)
          {
          break;
          }
        }
      }

    if ((rc == ETIMEDOUT) && 
        (tp->tp_nthreads > tp->tp_min_threads) &&
        (tp->tp_idle_threads > 2))
      {
      tp->tp_idle_threads--;
      break;
      }

    tp->tp_idle_threads--;

    /* if we're shutting down, leave this loop */
    if (tp->tp_flags & POOL_DESTROY)
      break;

    if ((mywork = tp->tp_first) != NULL)
      {
      func = mywork->work_func;
      arg  = mywork->work_arg;

      tp->tp_first = mywork->next;
      if (tp->tp_last == mywork)
        tp->tp_last = NULL;

      working.next = tp->tp_active;
      tp->tp_active = &working;

      pthread_mutex_unlock(&tp->tp_mutex);
      pthread_cleanup_push(work_cleanup,tp);
      free(mywork);

      /* do the work */
      func(arg);

      /* cleanup the work */
      pthread_cleanup_pop(1); /* calls work_cleanup(NULL) */
      }
    }

  /* calls work_thread_cleanup(tp), this also unlock tp->tp_mutex */
  pthread_cleanup_pop(1);

  /*sprintf(log_buf, "work_thread exiting. Current allocated threads: %d", request_pool->tp_nthreads);
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buf);
  sprintf(log_buf, "work_thread exiting. Current idle threads: %d", request_pool->tp_idle_threads);
  log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buf);*/
  pthread_exit(0);
  } /* END work_thread() */





int initialize_threadpool(

  threadpool_t **pool,           /* I/O */
  int            min_threads,    /* I */
  int            max_threads,    /* I */
  int            max_idle_time)  /* I */

  {
  int           i;
  int           rc = 0;
  size_t        stack_size;

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
  if ((rc = pthread_attr_init(&(*pool)->tp_attr)) != 0)
    {
    perror("pthread_attr_init failed. Could not init thread pool.");
    log_err(-1, __func__, "pthread_attr_init failed. Could not init thread pool.");
    return rc;
    }

  pthread_attr_getstacksize(&(*pool)->tp_attr, &stack_size);
  if (stack_size < MINIMUM_STACK_SIZE)
    stack_size = MINIMUM_STACK_SIZE;
  if (stack_size > MAX_STACK_SIZE)
    stack_size = MAX_STACK_SIZE;

  pthread_attr_setstacksize(&(*pool)->tp_attr, stack_size);

  pthread_attr_setdetachstate(&(*pool)->tp_attr,PTHREAD_CREATE_DETACHED);

  /* if threads are static, create them all now */
  if ((*pool)->tp_max_idle_secs < 0)
    {
    for (i = 0; i < (*pool)->tp_min_threads; i++)
      {
      if ((rc = create_work_thread(*pool)) != 0)
        {
        return(rc);
        }

      (*pool)->tp_nthreads++;
      }
    }

  return(rc);
  } /* END initialize_threadpool() */





int enqueue_threadpool_request(

  void         *(*func)(void *),
  void         *arg,
  threadpool_t *tp)

  {
  tp_work_t *work = NULL;
/*  char              log_buf[LOCAL_LOG_BUF_SIZE];*/

  if ((work = (tp_work_t *)calloc(1, sizeof(tp_work_t))) == NULL)
    {
    return(ENOMEM);
    }

  work->next = NULL;
  work->work_func = func;
  work->work_arg  = arg;

  pthread_mutex_lock(&tp->tp_mutex);

  if (tp->tp_first == NULL)
    tp->tp_first = work;
  else
    tp->tp_last->next = work;
  
  tp->tp_last = work;

  if (tp->tp_idle_threads > 0)
    pthread_cond_signal(&tp->tp_waiting_work);
  else if ((tp->tp_nthreads < tp->tp_max_threads) &&
           (create_work_thread(tp) == 0))
    tp->tp_nthreads++;

  pthread_mutex_unlock(&tp->tp_mutex);

  return(0);
  } /* END enqueue_threadpool_request() */



bool threadpool_is_too_busy(

  threadpool_t *tp,
  int           permissions)

  {
  bool busy = false;
  int  num_open;
  bool manager = ((permissions & ATR_DFLAG_MGRD) != 0);

  pthread_mutex_lock(&tp->tp_mutex);

  num_open = tp->tp_max_threads - tp->tp_nthreads + tp->tp_idle_threads;

  // fix this these are wrong 
  if ((manager == true) &&
      (num_open < 3))
    busy = true;
  else if (manager == false)
    {
    if (num_open < 6)
      busy = true;
    else
      {
      double pcnt_open = num_open;
      pcnt_open /= tp->tp_max_threads;
      if (pcnt_open < 0.05)
        busy = true;
      }
    }
  
  pthread_mutex_unlock(&tp->tp_mutex);

  return(busy);
  } /* END threadpool_is_too_busy() */



void destroy_request_pool(
    
  threadpool_t *tp)

  {
  tp_work_t    *work;
  tp_working_t *ptr;

  pthread_mutex_lock(&tp->tp_mutex);

  /* set the pool to be destroyed and notify all threads */
  tp->tp_flags |= POOL_DESTROY;
  pthread_cond_broadcast(&tp->tp_waiting_work);

  /* cancel any active work */
  ptr = tp->tp_active;
  while (ptr != NULL)
    {
    pthread_cancel(ptr->working_id);
    ptr = ptr->next;
    }

  /* wait to be awoken */
  while (tp->tp_nthreads != 0)
    pthread_cond_wait(&tp->tp_can_destroy, &tp->tp_mutex);

  pthread_mutex_unlock(&tp->tp_mutex);

  /* free pending work */
  while ((work = tp->tp_first) != NULL)
    {
    tp->tp_first = work->next;
    free(work);
    }
  } /* END destroy_request_pool() */



void start_request_pool(

  threadpool_t *tp)

  {
  pthread_mutex_lock(&tp->tp_mutex);

  tp->tp_started = TRUE;

  pthread_mutex_unlock(&tp->tp_mutex);
  } /* END start_request_pool() */



/* END u_threadpool.c */

