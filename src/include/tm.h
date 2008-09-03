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
** Header file defineing the datatypes and library visiable
** variables for paralell awareness.
*/

#include "tm_.h"

/*
** The tm_roots structure contains data for the last
** tm_init call whose event has been polled.  <Me> is the
** caller's identity.  <Daddy> is the identity of the task that
** spawned the caller.  If <daddy> is the TM_NULL_TASK, the caller
** is the initial task of the job, running on job-relative
** node 0.
*/

struct tm_roots
  {
  tm_task_id tm_me;
  tm_task_id tm_parent;
  int  tm_nnodes;
  int  tm_ntasks;
  int  tm_taskpoolid;
  tm_task_id *tm_tasklist;
  };

/*
** The tm_whattodo structure contains data for the last
** tm_register event polled.  This is not implemented yet.
*/

typedef  struct tm_whattodo
  {
  int  tm_todo;
  tm_task_id tm_what;
  tm_node_id tm_where;
  } tm_whattodo_t;

/*
** Prototypes for all the TM API calls.
*/
int tm_init(void *info,
            struct tm_roots *roots);

int tm_poll(tm_event_t poll_event,
            tm_event_t *result_event,
            int  wait,
            int  *tm_errno);

int tm_notify(int  tm_signal);

int tm_spawn(int   argc,
             char  *argv[],
             char  *envp[],
             tm_node_id where,
             tm_task_id *tid,
             tm_event_t *event);

int tm_kill(tm_task_id tid,
            int  sig,
            tm_event_t *event);

int tm_obit(tm_task_id tid,
            int  *obitval,
            tm_event_t *event);

int tm_nodeinfo(tm_node_id **list,
                int  *nnodes);

int tm_taskinfo(tm_node_id node,
                tm_task_id *list,
                int  lsize,
                int  *ntasks,
                tm_event_t *event);

int tm_atnode(tm_task_id tid,
              tm_node_id *node);

int tm_rescinfo(tm_node_id node,
                char  *resource,
                int  len,
                tm_event_t *event);

int tm_publish(char  *name,
               void  *info,
               int  nbytes,
               tm_event_t *event);

int tm_subscribe(tm_task_id tid,
                 char  *name,
                 void  *info,
                 int  len,
                 int  *amount,
                 tm_event_t *event);

int tm_finalize(void);

int tm_alloc(char  *resources,
             tm_event_t *event);

int  tm_dealloc(tm_node_id node,
                tm_event_t *event);

int tm_create_event(tm_event_t *event);

int tm_destroy_event(tm_event_t *event);

int tm_register(tm_whattodo_t *what,
                tm_event_t *event);

/*
 *  DJH 15 Nov 2001.
 *  Generic "out-of-band" task adoption call for tasks parented by
 *  another job management system.  Minor security hole?
 *  Cannot be called with any other tm call.
 *  26 Feb 2002. Allows id to be jobid (adoptCmd = TM_ADOPT_JOBID)
 *  or some altid (adoptCmd = TM_ADOPT_ALTID)
 */
int tm_adopt(char *id, int adoptCmd, pid_t pid);
