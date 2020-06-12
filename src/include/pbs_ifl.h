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
#include <sys/socket.h>

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
#define ATTR_args "job_arguments"
#define ATTR_reservation_id "reservation_id"
#define ATTR_login_node_id "login_node_id"
#define ATTR_login_prop "login_property"
#define ATTR_external_nodes "external_nodes"
#define ATTR_multi_req_alps "multi_req_alps"
#define ATTR_M "Mail_Users"
#define ATTR_N "Job_Name"
#define ATTR_S "Shell_Path_List"
#define ATTR_depend   "depend"
#define ATTR_inter    "interactive"
#define ATTR_stagein  "stagein"
#define ATTR_stageout "stageout"
#define ATTR_jobtype  "jobtype"
#define ATTR_submit_host "submit_host"
#define ATTR_init_work_dir "init_work_dir"
/* The following were part of the variable_list */
#define ATTR_pbs_o_initdir          "PBS_O_INITDIR"
#define ATTR_pbs_o_rootdir          "PBS_O_ROOTDIR"
#define ATTR_pbs_o_workdir          "PBS_O_WORKDIR" /* same as init_work_dir */
#define ATTR_pbs_o_host             "PBS_O_HOST" /* same as submit_host */
#define ATTR_pbs_o_server           "PBS_O_SERVER"
#define ATTR_pbs_o_home             "PBS_O_HOME"
#define ATTR_pbs_o_logname          "PBS_O_LOGNAME"
#define ATTR_pbs_o_path             "PBS_O_PATH"
#define ATTR_pbs_o_mail             "PBS_O_MAIL"
#define ATTR_pbs_o_shell            "PBS_O_SHELL"
#define ATTR_pbs_o_tz               "PBS_O_TZ"
#define ATTR_pbs_o_lang             "PBS_O_LANG"
#define ATTR_pbs_o_uid              "PBS_O_UID"
#define ATTR_pbs_o_submit_filter    "PBS_O_SUBMIT_FILTER"

/* additional job and general attribute names */

#define ATTR_ctime      "ctime"
#define ATTR_exechost   "exec_host"
#define ATTR_execport	"exec_port"
#define ATTR_mtime      "mtime"
#define ATTR_qtime      "qtime"
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
#define ATTR_restart_name	"restart_name"   /* saved name of checkpoint restart file */
#define ATTR_comp_time   	"comp_time"
#define ATTR_reported    	"reported"
#define ATTR_intcmd      	"inter_cmd"
#define ATTR_job_radix		"job_radix"
#define ATTR_sister_list	"sister_list"
#define ATTR_total_runtime  "total_runtime"
#define ATTR_P           "proxy_user"
#define ATTR_node_exclusive "node_exclusive"
#define ATTR_exec_gpus   "exec_gpus"
#define ATTR_exec_mics   "exec_mics"
#define ATTR_J           "job_id"
#define ATTR_pagg         "pagg_id"
#define ATTR_system_start_time "system_start_time"
  /* JOB_ATR_gpu_flags */
#define ATTR_gpu_flags		"gpu_flags"

/* additional queue attributes names */

#define ATTR_aclgren              "acl_group_enable"
#define ATTR_aclgroup             "acl_groups"
#define ATTR_aclhten              "acl_host_enable"
#define ATTR_aclhost              "acl_hosts"
#define ATTR_acluren              "acl_user_enable"
#define ATTR_acluser              "acl_users"
#define ATTR_altrouter            "alt_router"
#define ATTR_checkpoint_min       "checkpoint_min"
#define ATTR_checkpoint_defaults  "checkpoint_defaults"
#define ATTR_enable               "enabled"
#define ATTR_fromroute            "from_route_only"
#define ATTR_hostlist             "hostlist"         /* TORQUE only */
#define ATTR_killdelay            "kill_delay"
#define ATTR_maxgrprun            "max_group_run"
#define ATTR_maxque               "max_queuable"
#define ATTR_maxuserque           "max_user_queuable"
#define ATTR_maxuserrun           "max_user_run"
#define ATTR_qtype                "queue_type"
#define ATTR_rescassn             "resources_assigned"
#define ATTR_rescdflt             "resources_default"
#define ATTR_rescmax              "resources_max"
#define ATTR_rescmin              "resources_min"
#define ATTR_featreqd             "features_required"
#define ATTR_req_login_property   "required_login_property"
#define ATTR_rerunnable           "restartable"      /* TORQUE only */
#define ATTR_rndzretry            "rendezvous_retry"
#define ATTR_routedest            "route_destinations"
#define ATTR_routeheld            "route_held_jobs"
#define ATTR_routewait            "route_waiting_jobs"
#define ATTR_routeretry           "route_retry_time"
#define ATTR_routelife            "route_lifetime"
#define ATTR_rsvexpdt             "reserved_expedite"
#define ATTR_rsvsync              "reserved_sync"
#define ATTR_start                "started"
#define ATTR_count                "state_count"
#define ATTR_number               "number_jobs"
#define ATTR_acllogic             "acl_logic_or"
#define ATTR_aclgrpslpy           "acl_group_sloppy"
#define ATTR_keepcompleted        "keep_completed"
#define ATTR_disallowedtypes      "disallowed_types"
#define ATTR_is_transit           "is_transit"
#define ATTR_req_infomax          "req_information_max"
#define ATTR_req_infomin          "req_information_min"
#define ATTR_req_infodefault      "req_information_default"
#define ATTR_ghost_queue          "ghost_queue"

/* additional server attributes names */

/* NOTE: steps for adding new attribute described in ??? */
/*  - create #define ATTR_* in include/pbs_ifl.h
    - insert SRV_ATR_* in include/server.h
    - add ATTR_* in include/qmgr_svr_public.h
    - insert structure in server/svr_attr_def.c
        NOTE:  structure must be in same relative position as SRV_ATR_*
    - insert usage code in proper location
*/

#define ATTR_aclroot     "acl_roots"
#define ATTR_gresmodifiers "gres_modifiers"
#define ATTR_managers    "managers"
#define ATTR_dfltque     "default_queue"
#define ATTR_dispsvrsuffix "display_job_server_suffix"
#define ATTR_jobsuffixalias "job_suffix_alias"
#define ATTR_usejobssubdirs "use_jobs_subdirs"
#define ATTR_defnode     "default_node"
#define ATTR_locsvrs     "location_servers"
#define ATTR_logevents   "log_events"
#define ATTR_logfile     "log_file"
#define ATTR_loglevel    "log_level"
#define ATTR_mailfrom    "mail_from"
#define ATTR_nodepack    "node_pack"
#define ATTR_nodesuffix  "node_suffix"
#define ATTR_operators   "operators"
#define ATTR_queryother  "query_other_jobs"
#define ATTR_resccost    "resources_cost"
#define ATTR_rescavail   "resources_available"
#define ATTR_schedit     "scheduler_iteration"
#define ATTR_scheduling  "scheduling"
#define ATTR_status      "server_state"
#define ATTR_syscost     "system_cost"
#define ATTR_pingrate    "node_ping_rate"
#define ATTR_ndchkrate   "node_check_rate"
#define ATTR_tcptimeout  "tcp_timeout"
#define ATTR_jobstatrate "job_stat_rate"
#define ATTR_polljobs    "poll_jobs"
#define ATTR_note_append_on_error   "note_append_on_error"
#define ATTR_downonerror "down_on_error"
#define ATTR_disableserveridcheck "disable_server_id_check"
#define ATTR_jobnanny    "job_nanny"
#define ATTR_ownerpurge  "owner_purge"
#define ATTR_qcqlimits   "queue_centric_limits"
#define ATTR_momjobsync  "mom_job_sync"
#define ATTR_maildomain  "mail_domain"
#define ATTR_pbsversion  "pbs_version"
#define ATTR_submithosts "submit_hosts"
#define ATTR_allownodesubmit  "allow_node_submit"
#define ATTR_allowproxyuser   "allow_proxy_user"
#define ATTR_autonodenp  "auto_node_np"
#define ATTR_servername  "server_name"
#define ATTR_logfilemaxsize "log_file_max_size"
#define ATTR_logfilerolldepth "log_file_roll_depth"
#define ATTR_logkeepdays  "log_keep_days"
#define ATTR_nextjobnum   "next_job_number"
#define ATTR_extraresc    "extra_resc"
#define ATTR_schedversion "sched_version"
#define ATTR_acctkeepdays "accounting_keep_days"
#define ATTR_lockfile "lock_file"
#define ATTR_credentiallifetime "credential_lifetime"
#define ATTR_jobmustreport "job_must_report"
#define ATTR_LockfileUpdateTime "lock_file_update_time"
#define ATTR_LockfileCheckTime "lock_file_check_time"
#define ATTR_npdefault  "np_default"
#define ATTR_clonebatchsize "clone_batch_size"
#define ATTR_clonebatchdelay "clone_batch_delay"
#define ATTR_jobstarttimeout "job_start_timeout"
#define ATTR_jobforcecanceltime "job_force_cancel_time"
#define ATTR_maxarraysize    "max_job_array_size"
#define ATTR_maxslotlimit    "max_slot_limit"
#define ATTR_recordjobinfo   "record_job_info"
#define ATTR_recordjobscript   "record_job_script"
#define ATTR_joblogfilemaxsize "job_log_file_max_size"
#define ATTR_joblogfilerolldepth "job_log_file_roll_depth"
#define ATTR_joblogkeepdays  "job_log_keep_days"
#ifdef MUNGE_AUTH
  #define ATTR_authusers       "authorized_users"
#endif
#define ATTR_minthreads          "min_threads"
#define ATTR_maxthreads          "max_threads"
#define ATTR_threadidleseconds   "thread_idle_seconds"
#define ATTR_moabarraycompatible "moab_array_compatible"
#define ATTR_nomailforce             "no_mail_force"
#define ATTR_interactivejobscanroam  "interactive_jobs_can_roam" 
#define ATTR_crayenabled             "cray_enabled"
#define ATTR_nppcu                   "nppcu"
#define ATTR_login_node_key          "login_node_key"
#define ATTR_maxuserqueuable         "max_user_queuable"
#define ATTR_automaticrequeueexitcode "automatic_requeue_exit_code"
#define ATTR_jobsynctimeout           "job_sync_timeout"
#define ATTR_pass_cpu_clock           "pass_cpu_clock"
#define ATTR_request_version          "request_version"
#define ATTR_req_information          "req_information"
#define ATTR_sendmail_path            "sendmail_path"
/* additional node "attributes" names */

#define ATTR_NODE_state                "state"
#define ATTR_NODE_power_state          "power_state"
#define ATTR_NODE_np                   "np"
#define ATTR_NODE_properties           "properties"
#define ATTR_NODE_ntype                "ntype"
#define ATTR_NODE_jobs                 "jobs"
#define ATTR_NODE_status               "status"
#define ATTR_NODE_note                 "note"
#define ATTR_NODE_mom_port	           "mom_service_port"
#define ATTR_NODE_mom_rm_port          "mom_manager_port" 
#define ATTR_NODE_num_node_boards      "num_node_boards"
#define ATTR_NODE_numa_str             "numa_board_str"
#define ATTR_NODE_gpus                 "gpus"
#define ATTR_NODE_gpustatus            "gpu_status"
#define ATTR_NODE_gpus_str             "numa_gpu_node_str"
#define ATTR_NODE_mics                 "mics"
#define ATTR_NODE_micstatus            "mic_status"
#ifdef PENABLE_LINUX_CGROUPS
#define ATTR_NODE_total_sockets        "total_sockets"
#define ATTR_NODE_total_numa_nodes     "total_numa_nodes"
#define ATTR_NODE_total_cores          "total_cores"
#define ATTR_NODE_total_threads        "total_threads"
#define ATTR_NODE_dedicated_sockets    "dedicated_sockets"
#define ATTR_NODE_dedicated_numa_nodes "dedicated_numa_nodes"
#define ATTR_NODE_dedicated_cores      "dedicated_cores"
#define ATTR_NODE_dedicated_threads    "dedicated_threads"
#endif

#define ATTR_job_full_report_time     "job_full_report_time"
/* additional node "attributes" names */

#define ATTR_NODE_ttl              "TTL"
#define ATTR_NODE_acl              "acl"
#define ATTR_NODE_requestid        "requestid"

#define ATTR_cpustr                    "cpuset_string"
#define ATTR_memstr                    "memset_string"
#define ATTR_L_request                 "L_Request"
#define ATTR_gpus_reserved             "gpus_reserved"
#define ATTR_mics_reserved             "mics_reserved"
#define ATTR_user_kill_delay           "user_kill_delay"
#define ATTR_idle_slot_limit           "idle_slot_limit"
#define ATTR_default_gpu_mode          "default_gpu_mode"
#define ATTR_copy_on_rerun             "copy_on_rerun"
#define ATTR_job_exclusive_on_use      "job_exclusive_on_use"
#define ATTR_disable_automatic_requeue "disable_automatic_requeue"
#define ATTR_exitcodecanceledjob       "exit_code_canceled_job"
#define ATTR_timeoutforjobdelete       "timeout_for_job_delete"
#define ATTR_timeoutforjobrequeue      "timeout_for_job_requeue"
#define ATTR_dontwritenodesfile        "dont_write_nodes_file"
#define ATTR_aclusershosts              "acl_user_hosts"
#define ATTR_aclgroupshosts             "acl_group_hosts"
#define ATTR_nodesubmitexceptions       "node_submit_exceptions"
#define ATTR_legacy_vmem                "legacy_vmem"
#define ATTR_email_batch_seconds        "email_batch_seconds"
#define ATTR_tcpincomingtimeout        "tcp_incoming_timeout"
#define ATTR_ghost_array_recovery      "ghost_array_recovery"
#define ATTR_cgroup_per_task           "cgroup_per_task"

/* notification email formating */
#define ATTR_mailsubjectfmt "mail_subject_fmt"
#define ATTR_mailbodyfmt    "mail_body_fmt"


/* various attribute values */

#define CHECKPOINT_UNSPECIFIED "u"
#define NO_HOLD "n"
#define NO_JOIN "n"
#define NO_KEEP "n"
#define MAIL_AT_ABORT "a"
#define DEFAULT_PRIORITY "0"



#define ARRAY_RANGE  "array_range=" /* see qdel.c */
#define DELDELAY     "deldelay=" /* see qdel.c */
#define DELPURGE     "delpurge="   /* see qdel.c */
#define DELASYNC     "delasync"   /* see req_delete.c */
#define PURGECOMP    "purgecomplete="   /* see req_delete.c */
#define EXECQUEONLY  "exec_queue_only"   /* see req_stat.c */
#define RERUNFORCE   "force"

#define USER_HOLD   "u"
#define OTHER_HOLD  "o"
#define SYSTEM_HOLD "s"

/* node-attribute values (state,ntype) */

#define ND_free          "free"
#define ND_offline       "offline"
#define ND_down          "down"
#define ND_reserve       "reserve"
#define ND_job_exclusive "job-exclusive"
#define ND_job_sharing   "job-sharing"
#define ND_busy          "busy"
#define ND_nohierarchy   "MOM-list-not-sent"
#define ND_state_unknown "state-unknown"

/* node power state values */

#define ND_running       "Running"
#define ND_standby       "Standby"
#define ND_suspend       "Suspend"
#define ND_sleep         "Sleep"
#define ND_hibernate     "Hibernate"
#define ND_shutdown      "Shutdown"

/* these are not state values, they describe states and are used in pbsnodes */
/* active = job-exclusive, job-sharing, or busy */
#define ND_active        "active"
/* all = all */
#define ND_all           "all"
/* up = job-execlusive, job-sharing, reserve, free, busy and time-shared */
#define ND_up            "up"

/* this specifies the type of node */
#define ND_timeshared    "time-shared"
#define ND_cluster       "cluster"

/* queue disallowed types */
#define Q_DT_batch              "batch"
#define Q_DT_interactive        "interactive"
#define Q_DT_rerunable          "rerunable"
#define Q_DT_nonrerunable       "nonrerunable"
#define Q_DT_fault_tolerant     "fault_tolerant"
#define Q_DT_fault_intolerant   "fault_intolerant"
#define Q_DT_job_array          "job_array"

/*constant related to sum of string lengths for above strings*/
#define MAX_ENCODE_BFR  150

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
#define SHUT_QUICK 2

#define SIG_RESUME "resume"
#define SIG_SUSPEND "suspend"


/* WARNING: be careful changing these PBS_MAX* values.  They can result in a
   change in the structure of the .JB file, and binary incompatibilities between
   versions of TORQUE.  Do not change in a -fixes branch, avoid changing in
   a minor release if possible */



#define PBS_MAXHOSTNAME  1024 /* max host name length */
#ifndef MAXPATHLEN
#define MAXPATHLEN  1024 /* max path name length */
#endif

#ifndef MAXPORTLEN
#define MAXPORTLEN  5
#endif

#ifndef MAXNAMLEN
#define MAXNAMLEN  255
#endif

#define PBS_MAXUSER  32 /* max user name length */
#define PBS_MAXGRPN  16 /* max group name length */
#define PBS_MAXGPUID 16 /* max gpu id length */
#define PBS_MAXQUEUENAME 15 /* max queue name length */
#define PBS_MAXSERVERNAME PBS_MAXHOSTNAME /* max server name length */
#define PBS_MAXJOBARRAYLEN      7       /* number of characters allowed in jobarray portion of job id, including '[]' */
#ifdef USE_MAXINT_JOBIDS
#define PBS_MAXSEQNUM  12 /* max sequence number length */
#else
#define PBS_MAXSEQNUM  8 /* max sequence number length */
#endif
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

#define PBS_MAXCREDENTIAL_LEN 2048 /* Maximum authentication credential length. */
/* constants used to indicate version of job ji_qs struct written to disk.
   starting with 2.4.0 we stopped encoding the torque version number into the
   hex value e.g. 0x00020300 for torque v2.3.0. Now we just increment a value
   which is added to the last version encoded that way */
#define PBS_QS_VERSION_BASE 0x00020300 /* magic number do not change */
#define PBS_QS_VERSION_INT 2 /* increment this version number with every change to the ji_qs struct */
#define PBS_QS_VERSION  (PBS_QS_VERSION_BASE + PBS_QS_VERSION_INT) /* version number saved in the ji_qs struct */

/* someday the PBS_*_PORT definition will go away and only the */
/* PBS_*_SERVICE_NAME form will be used, maybe   */

#define PBS_BATCH_SERVICE_NAME  "pbs"

#ifndef PBS_BATCH_SERVICE_PORT
#define PBS_BATCH_SERVICE_PORT  15001
#endif /* PBS_BATCH_SERVICE_PORT */

#define PBS_BATCH_SERVICE_NAME_DIS "pbs_dis" /* new DIS port   */

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

#ifndef TRQ_AUTHD_SERVICE_PORT
#define TRQ_AUTHD_SERVICE_PORT 15005
#endif /* TRQ_AUTHD_SERVICE_PORT */

/*
 * Other misc checkpoint defines
 */
#define CHECKPOINTHOLD "checkpoint_hold"
#define CHECKPOINTCONT "checkpoint_cont"
#define MOM_DEFAULT_CHECKPOINT_DIR "$MOMDEFAULTCHECKPOINTDIR$"

#define NO_MOM_RELAY 1

enum batch_op { SET, UNSET, INCR, DECR, EQ, NE, GE, GT, LE, LT, DFLT, MERGE, INCR_OLD, SET_PLUGIN };

/* ALPS nppcu values */
enum nppcu_values { NPPCU_ALPS_CHOOSES = 0, NPPCU_NO_USE_HT, NPPCU_USE_HT };

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

extern char *pbs_server; /* server attempted to connect | connected to */

#ifdef __cplusplus
extern "C"
{
#endif
/* see pbs_connect(3B)         */

char *avail(int connect, char *resc);
int pbs_asyrunjob(int c, char *jobid, char *location, char *extend);
int pbs_alterjob_async(int connect, char *job_id, struct attrl *attrib, char *extend);
int pbs_alterjob(int connect, char *job_id, struct attrl *attrib, char *extend);
int pbs_connect(char *server);
#ifdef __cplusplus
int pbs_connect_ext(char *server, bool silence);
#endif
int pbs_query_max_connections();
char *pbs_default(void);
char *pbs_fbserver(void);
char *pbs_get_server_list(void);

int pbs_deljob(int connect, char *job_id, char *extend);
int pbs_disconnect(int connect);
char *pbs_geterrmsg(int connect);
int pbs_holdjob(int connect, char *job_id, char *hold_type, char *extend);
int pbs_checkpointjob(int connect, char *job_id, char *extend);
char *pbs_locjob(int connect, char *job_id, char *extend);

int pbs_manager(int connect, int command, int obj_type, char *obj_name, struct attropl *attrib, char *extend);

int pbs_movejob(int connect, char *job_id, char *destination, char *extend);

int pbs_msgjob(int connect, char *job_id, int file, char *message, char *extend);

int pbs_orderjob(int connect, char *job1, char *job2, char *extend);

int pbs_rescquery(int connect, char **rlist, int nresc, int *avail,
                    int *alloc, int *resv, int *down);

int pbs_rescreserve(int connect, char **rlist, int nresc, resource_t *phandle);

int pbs_rescrelease(int connect, resource_t rhandle);

int pbs_rerunjob(int connect, char *job_id, char *extend);

int pbs_rlsjob(int connect, char *job_id, char *hold_type, char *extend);

int pbs_runjob(int connect, char *jobid, char *loc, char *extend);

char **pbs_selectjob(int connect, struct attropl *select_list, char *extend);

int pbs_sigjob(int connect, char *job_id, char *signal, char *extend);

int pbs_sigjobasync(int connect, char *job_id, char *signal, char *extend);

void pbs_statfree(struct batch_status *stat);

struct batch_status *pbs_statjob(int connect, char *id, struct attrl *attrib, char *extend);

struct batch_status *pbs_selstat(int connect, struct attropl *select_list, char *extend);

struct batch_status *pbs_statque(int connect, char *id, struct attrl *attrib, char *extend);

struct batch_status *pbs_statserver(int connect, struct attrl *attrib, char *extend);

struct batch_status *pbs_statnode(int connect, char *id, struct attrl *attrib, char *extend);

char *pbs_submit(int connect, struct attropl *attrib, char *script, char *destination, char *extend);

int pbs_submit_hash_ext(int connect, void *job_attr, void *res_attr, char *script, char *destination, char *extend, char **job_id, char **msg);

int pbs_terminate(int connect, int manner, char *extend);

int totpool(int connect, int update);

int usepool(int connect, int update);

int pbs_gpumode(int connect, char *node, char *gpuid, int gpumode);

int trq_set_preferred_network_interface(char *if_name, struct sockaddr *preferred_addr);
char *trq_get_if_name(void);

/* used by Moab */
int pbs_stagein(int, char *, char *, char *);

#ifdef __cplusplus
}
#endif
#endif /* _PBS_IFL_DEF */

/*  end of pbs_ifl.h  */
