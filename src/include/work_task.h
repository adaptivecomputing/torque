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

#ifdef ENABLE_PTHREADS
#include <pthread.h>
#endif

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

struct work_task
  {
  list_link  wt_linkall; /* link to event type work list */
  list_link  wt_linkobj; /* link to others of same object */
#ifdef ENABLE_PTHREADS
  pthread_mutex_t *wt_objmutex;
#endif
  long   wt_event; /* event id: time, pid, socket, ... */
  enum work_type  wt_type; /* type of event */
  void (*wt_func)(struct work_task *);
  /* function to perform task */
  void  *wt_parm1; /* obj pointer for use by func */
  void  *wt_parm2; /* optional pointer for use by func */
  void (*wt_parmfunc)(struct work_task *);
  /* used in reissue_to_svr to store wt_func */
  int   wt_aux; /* optional info: e.g. child status */
  };

#ifdef ENABLE_PTHREADS
void mark_task_linkobj_mutex(struct work_task *,pthread_mutex_t *);
#endif
extern struct work_task *set_task(enum work_type, long event, void (*func)(), void *param);
extern void clear_task(struct work_task *ptask);
extern void dispatch_task(struct work_task *,void *);
extern void delete_task(struct work_task *ptask,int);

#endif /* WORK_TASK_H */
