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
 * queue.h - structure definitions for queue objects
 *
 * Include Files Required:
 *
 * <sys/types.h>
 * "attribute.h"
 * "list_link.h"
 *      "server_limits.h"
 *      "resource.h"
 *
 * Queue Types
 */

#ifndef QUEUE_H
#define QUEUE_H

#define QTYPE_Unset 0
#define QTYPE_Execution 1
#define QTYPE_RoutePush 2
#define QTYPE_RoutePull 3

#define ROUTE_PERM_FAILURE  -1
#define ROUTE_SUCCESS        0
#define ROUTE_RETRY          1
#define ROUTE_DEFERRED       2

/**
 * Attributes, including the various resource-lists are maintained in an
 * array in a "decoded or parsed" form for quick access to the value.
 *
 * The following enum defines the index into the array.
 *
 * @see src/server/queue_attr_def.c
 */

enum queueattr
  {
  QA_ATR_QType,
  QA_ATR_Priority,
  QA_ATR_Hostlist,
  QA_ATR_Rerunnable,
  QA_ATR_MaxJobs,
  QA_ATR_MaxUserJobs,
  QA_ATR_TotalJobs,
  QA_ATR_JobsByState,
  QA_ATR_MaxReport,
  QA_ATR_MaxRun,
  QA_ATR_AclHostEnabled,
  QA_ATR_AclHost,
  QA_ATR_AclUserEnabled,
  QA_ATR_AclUsers,
  QA_ATR_FromRouteOnly,
  QA_ATR_ResourceMax,
  QA_ATR_ResourceMin,
  QA_ATR_ResourceDefault,
  QA_ATR_AclGroupEnabled,
  QA_ATR_AclGroup,          /* 19 */
  QA_ATR_AclLogic,
  QA_ATR_AclGroupSloppy,
  QA_ATR_MTime,
  QA_ATR_DisallowedTypes,

  /* The following attributes apply only to execution queues */

  QE_ATR_checkpoint_dir,
  QE_ATR_checkpoint_min,
  QE_ATR_checkpoint_defaults,
  QE_ATR_RendezvousRetry,
  QE_ATR_ReservedExpedite,
  QE_ATR_ReservedSync,
  QE_ATR_ResourceAvail,
  QE_ATR_ResourceAssn,
  QE_ATR_KillDelay,
  QE_ATR_MaxUserRun,
  QE_ATR_MaxGrpRun,
  QE_ATR_KeepCompleted,
  QE_ATR_is_transit,

  /* The following attribute apply only to routing queues... */

  QR_ATR_RouteDestin,
  QR_ATR_AltRouter,
  QR_ATR_RouteHeld,
  QR_ATR_RouteWaiting,
  QR_ATR_RouteRetryTime,
  QR_ATR_RouteLifeTime,

#include "site_que_attr_enum.h"
  QA_ATR_Enabled, /* these are last for qmgr print function   */
  QA_ATR_Started,
  QA_ATR_LAST};   /* WARNING: Must be the highest valued enum */

extern attribute_def que_attr_def[];

#define QE_ATR_ChkptDir  QE_ATR_checkpoint_dir
#define QE_ATR_ChkptMim  QE_ATR_checkpoint_min

/* at last we come to the queue definition itself */

struct pbs_queue
  {
  list_link  qu_link;  /* forward/backward links */
  tlist_head qu_jobs;  /* jobs in this queue */
  tlist_head qu_jobs_array_sum; /* jobs with job arrays summarized */

  struct queuefix
    {
    int    qu_modified; /* != 0 => update disk file */
    int    qu_type;     /* queue type: exec, route */
    time_t qu_ctime;    /* time queue created */
    time_t qu_mtime;    /* time queue last modified */
    char   qu_name[PBS_MAXQUEUENAME]; /* queue name */
    } qu_qs;

  int  qu_numjobs;  /* current numb jobs in queue */
  int  qu_numcompleted;  /* current numb completed jobs in queue */
  int  qu_njstate[PBS_NUMJOBSTATE]; /* # of jobs per state */
  char qu_jobstbuf[100];

  /* the queue attributes */

  attribute qu_attr[QA_ATR_LAST];
  };

typedef struct pbs_queue pbs_queue;

extern pbs_queue *find_queuebyname(char *);
extern pbs_queue *get_dfltque();
extern pbs_queue *que_alloc(char *name);
extern void   que_free(pbs_queue *);
extern pbs_queue *que_recov(char *);
extern int    que_save(pbs_queue *);

#endif /* QUEUE_H */
