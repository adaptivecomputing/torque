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
 * @(#) $Id$
 */

/*
 * Server Work Tasks
 *
 * This structure is used by the server to track deferred work tasks.
 *
 * This information need not be preserved.
 *
 * Other Required Header Files
 * "list_link.h"
 */

#ifndef WORK_TASK_H
#define WORK_TASK_H 1

#include <pthread.h>
#include <list>
#include <vector>
#include <stdlib.h>

#define INITIAL_ALL_TASKS_SIZE 4

enum work_type
  {
  WORK_Immed,  /* immediate action: see state */
  WORK_Timed,  /* action at certain time */
  WORK_Deferred_Child, /* On Death of a Child */
  WORK_Deferred_Reply, /* On reply to an outgoing service request */
  WORK_Deferred_Local, /* On reply to a local service request */
  WORK_Deferred_Other, /* various other events */

  WORK_Deferred_Cmp /* Never set directly, used to indicate that */
  /* a WORK_Deferred_Child is ready            */
  };


struct work_task *set_task(enum work_type type, long event_id, void (*func)(struct work_task *), void *parm, int get_lock);
int dispatch_task(struct work_task *ptask);
void delete_task(struct work_task *ptask);
void clear_task(struct work_task *ptask);


int task_is_in_threadpool(struct work_task *ptask);




typedef struct timed_task
  {
  work_task *wt;
  long       task_time;
  } timed_task;

class all_tasks
  {
public:
  std::vector<work_task *> tasks;
  pthread_mutex_t *alltasks_mutex;
  all_tasks()
    {
    alltasks_mutex = (pthread_mutex_t *)calloc(1,sizeof(pthread_mutex_t));
    pthread_mutex_init(alltasks_mutex,NULL);
    }
  ~all_tasks()
    {
    free(alltasks_mutex);
    }
  };



typedef struct work_task
  {
  all_tasks           *wt_tasklist; 
  pthread_mutex_t     *wt_mutex; 
  int                  wt_being_recycled;
  long                 wt_event; /* event id: time, pid, socket, ... */
  enum work_type       wt_type; /* type of event */
  void (*wt_func)      (struct work_task *);
  /* function to perform task */
  void                *wt_parm1; /* obj pointer for use by func */
  void                *wt_parm2; /* optional pointer for use by func */
  void (*wt_parmfunc)  (struct work_task *);
  /* used in reissue_to_svr to store wt_func */
  int                  wt_aux; /* optional info: e.g. child status */
  } work_task;

int        insert_task(all_tasks *, work_task *);
int        remove_task(all_tasks *,work_task *);
int        has_task(all_tasks *);
int        dispatch_timed_task(work_task *);
work_task *pop_timed_task(time_t time_now);
int        insert_timed_task(all_tasks *, time_t task_time, work_task *);


struct batch_request;
int issue_to_svr(const char *server_name, batch_request **preq, void (*f)(struct work_task *));


#define TASKS_TO_REMOVE       1000
#define MAX_TASKS_IN_RECYCLER 5000


typedef struct task_recycler
  {
  unsigned int     next_id;
  all_tasks        tasks;
  std::vector<work_task *>::iterator iter;
  unsigned int     max_id;
  pthread_mutex_t *mutex;
  } task_recycler;

void initialize_task_recycler();
work_task *next_task_from_recycler(all_tasks *at, std::vector<work_task *>::iterator& iter);
void *remove_some_recycle_tasks(void *vp);
int insert_task_into_recycler(struct work_task *ptask);


#endif /* WORK_TASK_H */
