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

/* DIAGTODO: write diag.h and fill out new attrs */
/*
 * server.h - definitions for the server object (structure)
 *
 * Other include files required:
 * <sys/types.h>
 * "attribute.h"
 * "list_link.h"
 * "server_limits.h"
 *
 * The server object (structure) contains the parameters which
 * control the operation of the server itself.  This includes
 * the server attributes and resource (limits).
 */

#ifndef SERVER_H
#define SERVER_H

#include "server_limits.h"

#define NO_BUFFER_SPACE -2
#define NO_ATTR_DATA    1
#define ATTR_NOT_FOUND  -2

enum srv_atr
  {
  SRV_ATR_State,                 /* 0 */
  SRV_ATR_scheduling,
  SRV_ATR_max_running,
  SRV_ATR_MaxUserRun,
  SRV_ATR_MaxGrpRun,
  SRV_ATR_TotalJobs,
  SRV_ATR_JobsByState,
  SRV_ATR_acl_host_enable,
  SRV_ATR_acl_hosts,
  SRV_ATR_AclUserEnabled,
  SRV_ATR_AclUsers,              /* 10 */
  SRV_ATR_AclRoot,
  SRV_ATR_managers,
  SRV_ATR_operators,
  SRV_ATR_dflt_que,
  SRV_ATR_log_events,
  SRV_ATR_mailfrom,
  SRV_ATR_query_others,
  SRV_ATR_resource_avail,
  SRV_ATR_resource_deflt,
  SRV_ATR_ResourceMax,           /* 20 */
  SRV_ATR_resource_assn,
  SRV_ATR_resource_cost,
  SRV_ATR_sys_cost,
  SRV_ATR_scheduler_iteration,
  SRV_ATR_ping_rate,
  SRV_ATR_check_rate,
  SRV_ATR_tcp_timeout,
  SRV_ATR_Comment,
  SRV_ATR_DefNode,
  SRV_ATR_NodePack,              /* 30 */
  SRV_ATR_NodeSuffix,
  SRV_ATR_JobStatRate,
  SRV_ATR_PollJobs,
  SRV_ATR_LogLevel,
  SRV_ATR_DownOnError,
  SRV_ATR_DisableServerIdCheck,
  SRV_ATR_JobNanny,
  SRV_ATR_OwnerPurge,
  SRV_ATR_QCQLimits,
  SRV_ATR_MomJobSync,            /* 40 */
  SRV_ATR_MailDomain,
  SRV_ATR_version,
  SRV_ATR_KillDelay,
  SRV_ATR_AclLogic,
  SRV_ATR_AclGroupSloppy,
  SRV_ATR_KeepCompleted,
  SRV_ATR_SubmitHosts,
  SRV_ATR_AllowNodeSubmit,
  SRV_ATR_AllowProxyUser,
  SRV_ATR_AutoNodeNP,            /* 50 */
  SRV_ATR_LogFileMaxSize,
  SRV_ATR_LogFileRollDepth,
  SRV_ATR_LogKeepDays,
  SRV_ATR_NextJobNumber,
  SRV_ATR_tokens,
  SRV_ATR_NetCounter,
  SRV_ATR_ExtraResc,
  SRV_ATR_ServerName,
  SRV_ATR_SchedVersion,
  SRV_ATR_AcctKeepDays,          /* 60 */
  SRV_ATR_lockfile,
  SRV_ATR_LockfileUpdateTime,
  SRV_ATR_LockfileCheckTime,
  SRV_ATR_CredentialLifetime,
  SRV_ATR_JobMustReport,
  SRV_ATR_checkpoint_dir,
  SRV_ATR_display_job_server_suffix,
  SRV_ATR_job_suffix_alias,
  SRV_ATR_MailSubjectFmt,
  SRV_ATR_MailBodyFmt,
  SRV_ATR_NPDefault,
  SRV_ATR_clonebatchsize,        /* 70 */
  SRV_ATR_clonebatchdelay,
  SRV_ATR_JobStartTimeout,
  SRV_ATR_JobForceCancelTime,
  SRV_ATR_MaxArraySize,
  SRV_ATR_MaxSlotLimit,
  SRV_ATR_RecordJobInfo,
  SRV_ATR_RecordJobScript,
  SRV_ATR_JobLogFileMaxSize,
  SRV_ATR_JobLogFileRollDepth,
  SRV_ATR_JobLogKeepDays,        /* 80 */
#ifdef MUNGE_AUTH
  SRV_ATR_authusers,
#endif
  SRV_ATR_MoabArrayCompatible,
  SRV_ATR_NoMailForce,
#include "site_svr_attr_enum.h"
  /* This must be last */
  SRV_ATR_LAST
  };
extern attribute_def svr_attr_def[];

struct server
  {

  struct server_qs
    {
    int  sv_numjobs; /* number of job owned by server    */
    int  sv_numque;  /* nuber of queues managed          */
    int  sv_jobidnumber; /* next number to use in new jobid  */
    /* the server struct must be saved  */
    /* whenever this value is updated   */
    time_t sv_savetm; /* time of server db update         */
    } sv_qs;

  time_t   sv_started;  /* time server started */
  time_t   sv_hotcycle;  /* if RECOV_HOT,time of last restart */
  time_t   sv_next_schedule; /* when to next run scheduler cycle */
  int   sv_jobstates[PBS_NUMJOBSTATE];  /* # of jobs per state */
  char   sv_jobstbuf[100];

  attribute sv_attr[SRV_ATR_LAST]; /* the server attributes      */

  int   sv_trackmodifed; /* 1 if tracking list modified     */
  int   sv_tracksize;  /* total number of sv_track entries */

  struct tracking *sv_track; /* array of track job records     */

  };

extern struct server server;


/*
 * server state values
 */
#define SV_STATE_DOWN    0
#define SV_STATE_INIT  1
#define SV_STATE_HOT  2
#define SV_STATE_RUN     3
#define SV_STATE_SHUTDEL 4
#define SV_STATE_SHUTIMM 5
#define SV_STATE_SHUTSIG 6

/*
 * Other misc defines
 */
#define SVR_HOSTACL "svr_hostacl"
#define PBS_DEFAULT_NODE "1#shared"

#define SVR_SAVE_QUICK 0
#define SVR_SAVE_FULL  1

#define SVR_HOT_CYCLE 15 /* retry mom every n sec on hot start     */
#define SVR_HOT_LIMIT 300 /* after n seconds, drop out of hot start */


/* function prototypes */

extern int svr_recov_xml(char *, int);
extern int svr_recov(char *, int);
extern int svr_save(struct server *, int);


/*
** Macros for fast min/max.
*/
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif /* END MIN */

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif /* END MAX */

#ifndef JobStatRate
#define JobStatRate MAX(PBS_JOBSTAT_MIN,server.sv_attr[(int)SRV_ATR_JobStatRate].at_val.at_long)
#endif

/* maintain a list of new nodes */

typedef struct new_node
  {
  list_link            nn_link;   /* link to other resources in list */
  char                *nn_name;   /* name of new node */
  } new_node;

/* Threaded-HA code */
#ifdef USE_HA_THREADS
typedef pthread_mutex_t mutex_t;
#else
typedef int mutex_t;
#endif /* USE_HA_THREADS */

/* END server.h */

#endif /* ifndef SERVER_H */
