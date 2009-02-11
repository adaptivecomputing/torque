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
 *
 *  pbs_ifl.h
 *
 */

#ifndef _PBS_IFL_DEF
#define _PBS_IFL_DEF

#ifdef __STDC__
/*
 * The following macro definations take affect when compiling under ansi C
 *
 * The A_ macro is provided for function prototype declarations.  It allows
 * ANSI C prototypes to be complied under K&R C
 */
#define A_(x) x

#else

/* The following macro definations take affect when compiling under K&R C */

#define A_(x) ()
#define const
#define volatile

#endif /* __STDC__ */

/* Attribute Names used by user commands */

#define ATTR_a "Execution_Time"
#define ATTR_c "Checkpoint"
#define ATTR_e "Error_Path"
#define ATTR_f "fault_tolerant"
#define ATTR_g "group_list"
#define ATTR_h "Hold_Types"
#define ATTR_j "Join_Path"
#define ATTR_k "Keep_Files"
#define ATTR_l "Resource_List"
#define ATTR_m "Mail_Points"
#define ATTR_o "Output_Path"
#define ATTR_p "Priority"
#define ATTR_q "destination"
#define ATTR_r "Rerunable"
#define ATTR_t "job_array_request"
#define ATTR_array_id "job_array_id"
#define ATTR_u "User_List"
#define ATTR_v "Variable_List"
#define ATTR_A "Account_Name"
#define ATTR_M "Mail_Users"
#define ATTR_N "Job_Name"
#define ATTR_S "Shell_Path_List"
#define ATTR_depend   "depend"
#define ATTR_inter    "interactive"
#define ATTR_stagein  "stagein"
#define ATTR_stageout "stageout"

/* additional job and general attribute names */

#define ATTR_ctime "ctime"
#define ATTR_exechost "exec_host"
#define ATTR_mtime "mtime"
#define ATTR_qtime "qtime"
#define ATTR_session    "session_id"
#define ATTR_euser      "euser"
#define ATTR_egroup     "egroup"
#define ATTR_hashname   "hashname"
#define ATTR_hopcount   "hop_count"
#define ATTR_security   "security"
#define ATTR_sched_hint "sched_hint"
#define ATTR_substate   "substate"
#define ATTR_name       "Job_Name"
#define ATTR_owner      "Job_Owner"
#define ATTR_used       "resources_used"
#define ATTR_state      "job_state"
#define ATTR_queue      "queue"
#define ATTR_server     "server"
#define ATTR_maxrun     "max_running"
#define ATTR_maxreport  "max_report"
#define ATTR_total      "total_jobs"
#define ATTR_comment    "comment"
#define ATTR_cookie     "cookie"
#define ATTR_qrank      "queue_rank"
#define ATTR_altid      "alt_id"
#define ATTR_etime      "etime"
#define ATTR_exitstat   "exit_status"
#define ATTR_forwardx11 "forward_x11"
#define ATTR_submit_args "submit_args"
#define ATTR_tokens     "tokens"
#define ATTR_netcounter "net_counter"
#define ATTR_umask      "umask"
#define ATTR_start_time "start_time"
#define ATTR_start_count "start_count"
#define ATTR_checkpoint_dir  "checkpoint_dir"    /* user specified directory for checkpoint files */
#define ATTR_checkpoint_name  "checkpoint_name"   /* user specified name of checkpoint file */
#define ATTR_checkpoint_time  "checkpoint_time"   /* timestamp of start of last checkpoint */
#define ATTR_checkpoint_restart_status  "checkpoint_restart_status"   /* checkpoint restart status */

#ifdef ENABLE_CSA
#define ATTR_pagg         "pagg_id"
#endif /* ENABLE_CSA */

/* additional queue attributes names */

#define ATTR_aclgren  "acl_group_enable"
#define ATTR_aclgroup  "acl_groups"
#define ATTR_aclhten  "acl_host_enable"
#define ATTR_aclhost  "acl_hosts"
#define ATTR_acluren  "acl_user_enable"
#define ATTR_acluser  "acl_users"
#define ATTR_altrouter  "alt_router"
#define ATTR_checkpoint_min  "checkpoint_min"
#define ATTR_enable  "enabled"
#define ATTR_fromroute  "from_route_only"
#define ATTR_hostlist    "hostlist"         /* TORQUE only */
#define ATTR_killdelay   "kill_delay"
#define ATTR_maxgrprun   "max_group_run"
#define ATTR_maxque      "max_queuable"
#define ATTR_maxuserque  "max_user_queuable"
#define ATTR_maxuserrun  "max_user_run"
#define ATTR_qtype       "queue_type"
#define ATTR_rescassn    "resources_assigned"
#define ATTR_rescdflt    "resources_default"
#define ATTR_rescmax     "resources_max"
#define ATTR_rescmin     "resources_min"
#define ATTR_rerunnable  "restartable"      /* TORQUE only */
#define ATTR_rndzretry   "rendezvous_retry"
#define ATTR_routedest   "route_destinations"
#define ATTR_routeheld   "route_held_jobs"
#define ATTR_routewait   "route_waiting_jobs"
#define ATTR_routeretry  "route_retry_time"
#define ATTR_routelife   "route_lifetime"
#define ATTR_rsvexpdt    "reserved_expedite"
#define ATTR_rsvsync     "reserved_sync"
#define ATTR_start       "started"
#define ATTR_count       "state_count"
#define ATTR_number      "number_jobs"
#define ATTR_acllogic    "acl_logic_or"
#define ATTR_aclgrpslpy  "acl_group_sloppy"
#define ATTR_keepcompleted "keep_completed"
#define ATTR_disallowedtypes "disallowed_types"

/* additional server attributes names */

/* NOTE: steps for adding new attribute described in ??? */
/*  - create #define ATTR_* in include/pbs_ifl.h
    - insert SRV_ATR_* in include/server.h
    - add SRV_ATR_* in include/qmgr_svr_public.h
    - insert structure in server/svr_attr_def.c
        NOTE:  structure must be in same relative position as SRV_ATR_*
    - insert usage code in proper location
*/

#define ATTR_aclroot  "acl_roots"
#define ATTR_managers  "managers"
#define ATTR_dfltque  "default_queue"
#define ATTR_defnode  "default_node"
#define ATTR_locsvrs  "location_servers"
#define ATTR_logevents  "log_events"
#define ATTR_logfile  "log_file"
#define ATTR_loglevel    "log_level"
#define ATTR_mailfrom  "mail_from"
#define ATTR_nodepack  "node_pack"
#define ATTR_nodesuffix  "node_suffix"
#define ATTR_operators  "operators"
#define ATTR_queryother  "query_other_jobs"
#define ATTR_resccost  "resources_cost"
#define ATTR_rescavail  "resources_available"
#define ATTR_schedit  "scheduler_iteration"
#define ATTR_scheduling  "scheduling"
#define ATTR_status      "server_state"
#define ATTR_syscost     "system_cost"
#define ATTR_pingrate    "node_ping_rate"
#define ATTR_ndchkrate   "node_check_rate"
#define ATTR_tcptimeout  "tcp_timeout"
#define ATTR_jobstatrate "job_stat_rate"
#define ATTR_polljobs    "poll_jobs"
#define ATTR_downonerror "down_on_error"
#define ATTR_disableserveridcheck "disable_server_id_check"
#define ATTR_jobnanny    "job_nanny"
#define ATTR_ownerpurge  "owner_purge"
#define ATTR_qcqlimits   "queue_centric_limits"
#define ATTR_momjobsync  "mom_job_sync"
#define ATTR_maildomain  "mail_domain"
#define ATTR_pbsversion  "pbs_version"
#define ATTR_submithosts  "submit_hosts"
#define ATTR_allownodesubmit  "allow_node_submit"
#define ATTR_allowproxyuser   "allow_proxy_user"
#define ATTR_autonodenp  "auto_node_np"
#define ATTR_servername  "server_name"
#define ATTR_logfilemaxsize "log_file_max_size"
#define ATTR_logfilerolldepth "log_file_roll_depth"
#define ATTR_nextjobnum  "next_job_number"
#define ATTR_extraresc   "extra_resc"
#define ATTR_schedversion "sched_version"

/* additional node "attributes" names */

#define ATTR_NODE_state  "state"
#define ATTR_NODE_np  "np"
#define ATTR_NODE_properties "properties"
#define ATTR_NODE_ntype         "ntype"
#define ATTR_NODE_jobs          "jobs"
#define ATTR_NODE_status        "status"
#define ATTR_NODE_note          "note"



/* various attribute values */

#define CHECKPOINT_UNSPECIFIED "u"
#define NO_HOLD "n"
#define NO_JOIN "n"
#define NO_KEEP "n"
#define MAIL_AT_ABORT "a"


#define DELDELAY  "deldelay=" /* see qdel.c */
#define DELPURGE  "delpurge="   /* see qdel.c */
#define EXECQUEONLY  "exec_queue_only"   /* see req_stat.c */
#define RERUNFORCE "force"
#define CHECKPOINTED "checkpoint_success"

#define USER_HOLD "u"
#define OTHER_HOLD "o"
#define SYSTEM_HOLD "s"

/* node-attribute values (state,ntype) */

#define ND_free   "free"
#define ND_offline  "offline"
#define ND_down   "down"
#define ND_reserve  "reserve"
#define ND_job_exclusive "job-exclusive"
#define ND_job_sharing  "job-sharing"
#define ND_busy   "busy"
#define ND_state_unknown "state-unknown"
#define ND_timeshared  "time-shared"
#define ND_cluster  "cluster"

/* queue disallowed types */
#define Q_DT_batch              "batch"
#define Q_DT_interactive        "interactive"
#define Q_DT_rerunable          "rerunable"
#define Q_DT_nonrerunable       "nonrerunable"
#define Q_DT_fault_tolerant     "fault_tolerant"
#define Q_DT_fault_intolerant   "fault_intolerant"

/*constant related to sum of string lengths for above strings*/
#define MAX_ENCODE_BFR  100

#define MGR_CMD_CREATE 0
#define MGR_CMD_DELETE 1
#define MGR_CMD_SET 2
#define MGR_CMD_UNSET 3
#define MGR_CMD_LIST 4
#define MGR_CMD_PRINT 5
#define MGR_CMD_ACTIVE 6

#define MGR_OBJ_NONE -1
#define MGR_OBJ_SERVER  0
#define MGR_OBJ_QUEUE   1
#define MGR_OBJ_JOB 2
#define MGR_OBJ_NODE 3

/* Misc defines for various requests */

#define MSG_OUT 1
#define MSG_ERR 2

#define SHUT_SIG -1
#define SHUT_IMMEDIATE 0
#define SHUT_DELAY 1
#define SHUT_QUICK 2

#define SIG_RESUME "resume"
#define SIG_SUSPEND "suspend"

#define PBS_MAXHOSTNAME  64 /* max host name length */
#ifndef MAXPATHLEN
#define MAXPATHLEN  1024 /* max path name length */
#endif
#ifndef MAXNAMLEN
#define MAXNAMLEN  255
#endif
#define MAX_NOTE  256 /* max node note length */
#define MAX_NOTE_STR  "256" /* max node note length as a string literal (this MUST match MAX_NOTE) */

#define PBS_MAXUSER  16 /* max user name length */
#define PBS_MAXGRPN  16 /* max group name length */
#define PBS_MAXQUEUENAME 15 /* max queue name length */
#define PBS_MAXSERVERNAME PBS_MAXHOSTNAME /* max server name length */
#define PBS_MAXJOBARRAYLEN      6       /* number of characters allowed in jobarray portion of job id, including '-' */
#define PBS_MAXSEQNUM  8 /* max sequence number length */
#define PBS_MAXPORTNUM  5 /* udp/tcp port numbers max=16 bits */
#define PBS_MAXJOBARRAY  99999
#define PBS_MAXSVRJOBID  (PBS_MAXSEQNUM + PBS_MAXSERVERNAME + PBS_MAXPORTNUM + PBS_MAXJOBARRAYLEN + 2 ) /* server job id size */
#define PBS_MAXCLTJOBID  (PBS_MAXSVRJOBID + PBS_MAXSERVERNAME + PBS_MAXPORTNUM + PBS_MAXJOBARRAYLEN + 2) /* client job id size */
#define PBS_MAXDEST  1024  /* destination size -- increased from 256 */
#define PBS_MAXROUTEDEST (PBS_MAXQUEUENAME + PBS_MAXSERVERNAME + PBS_MAXPORTNUM + 2) /* destination size */
#define PBS_USE_IFF  1 /* pbs_connect() to call pbs_iff */
#define PBS_INTERACTIVE  1 /* Support of Interactive jobs */
#define PBS_TERM_BUF_SZ  80 /* Interactive term buffer size */
#define PBS_TERM_CCA  6 /* Interactive term cntl char array */

#define PBS_QS_VERSION  0x00020300 /* magic number used to determine version of pbs job quick save struct */
/* the magic number is split into 4 8-bit chunks.  the first 8 bits are
   unused.  the second b bits represent the major version number
   third 8 bits are the minor version, and the final 8 bits are the
   bug fix version.  we write the torque version that the job qs struct
   was last changed in this constant writing them as if they are two
   digit decimal numbers. all that really matters is that we assign a
   unique value for each change in the ji_qs struct and that it can't
   be confused with data at the start of a ji_qs struct from before
   torque version 2.2.0 */
/* someday the PBS_*_PORT definition will go away and only the */
/* PBS_*_SERVICE_NAME form will be used, maybe   */

#define PBS_BATCH_SERVICE_NAME  "pbs"

#ifndef PBS_BATCH_SERVICE_PORT
#define PBS_BATCH_SERVICE_PORT  15001
#endif /* PBS_BATCH_SERVICE_PORT */

#define PBS_BATCH_SERVICE_NAME_DIS "pbs_dis" /* new DIS port   */

#ifndef PBS_BATCH_SERVICE_PORT_DIS
#define PBS_BATCH_SERVICE_PORT_DIS 15001  /* new DIS port   */
#endif /* PBS_BATCH_SERVICE_PORT_DIS */

#define PBS_MOM_SERVICE_NAME  "pbs_mom"

#ifndef PBS_MOM_SERVICE_PORT
#define PBS_MOM_SERVICE_PORT  15002
#endif /* PBS_MOM_SERVICE_PORT */

#define PBS_MANAGER_SERVICE_NAME "pbs_resmon"

#ifndef PBS_MANAGER_SERVICE_PORT
#define PBS_MANAGER_SERVICE_PORT 15003
#endif /* PBS_MANAGER_SERVICE_PORT */

#define PBS_SCHEDULER_SERVICE_NAME "pbs_sched"

#ifndef PBS_SCHEDULER_SERVICE_PORT
#define PBS_SCHEDULER_SERVICE_PORT 15004
#endif /* PBS_SCHEDULER_SERVICE_PORT */

enum batch_op { SET, UNSET, INCR, DECR, EQ, NE, GE, GT, LE, LT, DFLT, MERGE };

/*
** This structure is identical to attropl so they can be used
** interchangably.  The op field is not used.
*/

struct attrl
  {

  struct attrl  *next;
  char          *name;
  char          *resource;
  char          *value;
  enum batch_op  op; /* not used */
  };

struct attropl
  {

  struct attropl *next;
  char   *name;
  char   *resource;
  char   *value;
  enum batch_op   op;
  };

struct batch_status
  {

  struct batch_status *next;
  char                *name;

  struct attrl        *attribs;
  char                *text;
  };




/* Resource Reservation Information */
typedef int resource_t;  /* resource reservation handle */

#define RESOURCE_T_NULL  (resource_t)0
#define RESOURCE_T_ALL  (resource_t)-1

extern int pbs_errno;  /* error number */

extern char *pbs_server; /* server attempted to connect | connected to */
/* see pbs_connect(3B)         */

extern char *avail A_((int connect, char *resc));
extern int pbs_asyrunjob A_((int c, char *jobid, char *location, char *extend));
extern int pbs_alterjob_async A_((int connect, char *job_id, struct attrl *attrib, char *extend));
extern int pbs_alterjob A_((int connect, char *job_id, struct attrl *attrib, char *extend));
extern int pbs_connect A_((char *server));
extern int pbs_query_max_connections();
extern char *pbs_default A_((void));
extern char *pbs_fbserver A_((void));
extern char *pbs_get_server_list A_((void));

extern int pbs_deljob A_((int connect, char *job_id, char *extend));
extern int pbs_disconnect A_((int connect));
extern char *pbs_geterrmsg A_((int connect));
extern int pbs_holdjob A_((int connect, char *job_id, char *hold_type, char *extend));
extern int pbs_checkpointjob A_((int connect, char *job_id, char *extend));
extern char *pbs_locjob A_((int connect, char *job_id, char *extend));

extern int
  pbs_manager A_((int connect, int command, int obj_type, char *obj_name,
                  struct attropl *attrib, char *extend));

extern int
  pbs_movejob A_((int connect, char *job_id, char *destination, char *extend));

extern int
  pbs_msgjob A_((int connect, char *job_id, int file, char *message,
                 char *extend));

extern int
  pbs_orderjob A_((int connect, char *job1, char *job2, char *extend));

extern int
  pbs_rescquery A_((int connect, char **rlist, int nresc, int *avail,
                    int *alloc, int *resv, int *down));

extern int
  pbs_rescreserve A_((int connect, char **rlist, int nresc, resource_t *phandle));

extern int
  pbs_rescrelease A_((int connect, resource_t rhandle));

extern int
  pbs_rerunjob A_((int connect, char *job_id, char *extend));

extern int
  pbs_rlsjob A_((int connect, char *job_id, char *hold_type, char *extend));

extern int
  pbs_runjob A_((int connect, char *jobid, char *loc, char *extend));

extern char **
  pbs_selectjob A_((int connect, struct attropl *select_list, char *extend));

extern int
  pbs_sigjob A_((int connect, char *job_id, char *signal, char *extend));

extern void
  pbs_statfree A_((struct batch_status *stat));

extern struct batch_status *
        pbs_statdest A_((int connect, char *id, char *extend));

extern struct batch_status *
        pbs_statjob A_((int connect, char *id, struct attrl *attrib, char *extend));

extern struct batch_status *
        pbs_selstat A_((int connect, struct attropl *select_list, char *extend));

extern struct batch_status *
        pbs_statque A_((int connect, char *id, struct attrl *attrib, char *extend));

extern struct batch_status *
        pbs_statserver A_((int connect, struct attrl *attrib, char *extend));

extern struct batch_status *
        pbs_statnode A_((int connect, char *id, struct attrl *attrib, char *extend));

extern char *
  pbs_submit A_((int connect, struct attropl *attrib, char *script,
                 char *destination, char *extend));

extern int
  pbs_terminate A_((int connect, int manner, char *extend));

extern int
  totpool A_((int connect, int update));

extern int
  usepool A_((int connect, int update));

#endif /* _PBS_IFL_DEF */

/*  end of pbs_ifl.h  */
