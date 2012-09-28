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
 * job.h - structure definations for job objects
 *
 * Include Files Required:
 * "list_link.h"
 * "attribute.h"
 * "server_limits.h"
 */


#ifndef PBS_JOB_H
#define PBS_JOB_H 1

#include <limits.h>
#include "server_limits.h"
#include "list_link.h"
#include "pbs_ifl.h"
#include "attribute.h"
#include "resizable_array.h"
#include "hash_table.h"
#include "mom_hierarchy.h"
#include "dynamic_string.h"
#include "tcp.h" /* tcp_chan */

#define SAVEJOB_BUF_SIZE 8192

#ifndef MAX_LINE
#define MAX_LINE 1024
#endif

/* anything including job.h also needs array.h so lets just include it this way*/
#ifndef SUCCESS
#define SUCCESS 1
#endif 
#ifndef FAILURE
#define FAILURE 0
#endif

#ifndef PBS_MOM
struct job_array;
#endif

/*
 * Dependent Job Structures
 *
 * This set of structures are used by the server to track job
 * dependency.
 *
 * The depend (parent) structure is used to record the type of
 * dependency.  It also heads the list of depend_job related via this type.
 * For a type of "sycnto", the number of jobs expected, registered and
 * ready are also recorded.
 */

struct depend
  {
  list_link dp_link; /* link to next dependency, if any       */
  short   dp_type; /* type of dependency (all)           */
  short   dp_numexp; /* num jobs expected (on or syncct only) */
  short   dp_numreg; /* num jobs registered (syncct only)     */
  short   dp_released; /* This job released to run (syncwith)   */
  tlist_head dp_jobs; /* list of related jobs  (all)           */
  };

/*
 * The depend_job structure is used to record the name and location
 * of each job which is involved with the dependency
 */

struct depend_job
  {
  list_link dc_link;
  short dc_state; /* released / ready to run (syncct)  */
  long dc_cost; /* cost of this child (syncct)   */
  char dc_child[PBS_MAXSVRJOBID+1]; /* child (dependent) job  */
  char dc_svr[PBS_MAXSERVERNAME+1]; /* server owning job  */
  };

struct array_depend
  {
  list_link  dp_link;
  short      dp_type;
  tlist_head dp_jobs;
  };

struct array_depend_job
  {
  list_link dc_link;
  /* in this case, the child is the job depending on the array */
  char dc_child[PBS_MAXSVRJOBID+1];
  char dc_svr[PBS_MAXSERVERNAME+1];
  int  dc_num;
  };

/*
 * Warning: the relation between the numbers assigned to after* and before*
 * is critical.
 */
#define JOB_DEPEND_TYPE_AFTERSTART  0
#define JOB_DEPEND_TYPE_AFTEROK   1
#define JOB_DEPEND_TYPE_AFTERNOTOK  2
#define JOB_DEPEND_TYPE_AFTERANY  3
#define JOB_DEPEND_TYPE_BEFORESTART  4
#define JOB_DEPEND_TYPE_BEFOREOK  5
#define JOB_DEPEND_TYPE_BEFORENOTOK  6
#define JOB_DEPEND_TYPE_BEFOREANY  7
#define JOB_DEPEND_TYPE_ON   8
#define JOB_DEPEND_TYPE_SYNCWITH  9
#define JOB_DEPEND_TYPE_SYNCCT  10
#define JOB_DEPEND_TYPE_AFTERSTARTARRAY 11
#define JOB_DEPEND_TYPE_AFTEROKARRAY 12
#define JOB_DEPEND_TYPE_AFTERNOTOKARRAY 13
#define JOB_DEPEND_TYPE_AFTERANYARRAY 14
#define JOB_DEPEND_TYPE_BEFORESTARTARRAY 15
#define JOB_DEPEND_TYPE_BEFOREOKARRAY 16
#define JOB_DEPEND_TYPE_BEFORENOTOKARRAY 17
#define JOB_DEPEND_TYPE_BEFOREANYARRAY 18
#define JOB_DEPEND_NUMBER_TYPES  19

#define JOB_DEPEND_OP_REGISTER  1
#define JOB_DEPEND_OP_RELEASE  2
#define JOB_DEPEND_OP_READY  3
#define JOB_DEPEND_OP_DELETE  4
#define JOB_DEPEND_OP_UNREG  5

/* Job recovery levels. Options used to start pbs_mom */
#define JOB_RECOV_REQUE       0  /* -q option */
#define JOB_RECOV_TERM_REQUE  1  /* -r option */
#define JOB_RECOV_RUNNING     2  /* -p option */
#define JOB_RECOV_DELETE      3  /* -P option */

/* These are flags to distinguish Mother Superior from intermediate moms
   for a job_radix */
#define MOTHER_SUPERIOR       0
#define INTERMEDIATE_MOM      1
#define LEAF_MOM              2

/*
 * The badplace structure is used to keep track of destinations
 * which have been tried by a route queue and given a "reject"
 * status back, see svr_movejob.c.
 */

typedef struct badplace
  {
  list_link bp_link;
  char  bp_dest[PBS_MAXROUTEDEST + 1];
  } badplace;

/*
 * The grpcache structure defined here is used by MOM to maintain the
 * home directory and supplementary group list of the user name under
 * which the job is running.   This information is needed to set up the
 * groups when running the job and returning output and staged-out files.
 * The information is keep here rather than make repeated hits on the
 * password and group files.
 */

#ifdef NGROUPS_MAX

struct grpcache
  {
  int    gc_ngroup; /* number of active supplementary groups */
  int    gc_groups[NGROUPS_MAX];
  char   gc_homedir[1]; /* more space allocated as part of this  */
  /* structure following here   */
  };

#endif /* END NGROUPS_MAX */

enum job_types
  {
  JOB_TYPE_normal,       /* no cray nodes involved */
  JOB_TYPE_cray,         /* only cray nodes involved */
  JOB_TYPE_login,        /* only cray login nodes involved */
  JOB_TYPE_heterogeneous /* both cray computes and external nodes involved */
  };

/*
 * Job attributes/resources are maintained in one of two ways.
 * Most of the attributes are maintained in a decoded or parsed form.
 * This allows quick access to the pbs_attribute and resource values
 * when making decisions about the job (scheduling, routing, ...),
 * see ji_wattr[].
 *
 * Any pbs_attribute or resource which is not recognized on this server
 * are kept in an "attrlist", a linked list of the "external"
 * form (attr_extern, see attribute.h).  These are maintained because
 * the job may be passed on to another server (route or qmove) that
 * does recognize them.
 * See the job structure entry ji_attrlist and the attrlist structure.
 */


/*
 * The following job_atr enum provide an index into the array of
 * decoded job attributes, ji_wattr[], for quick access.
 * Most of the attributes here are "public", but some are Read Only,
 * Private, or even Internal data items; maintained here because of
 * their variable size.
 *
 * "JOB_ATR_LAST" must be the last value as its number is used to
 * define the size of the array.
 */

/*
 * sync w/job_attr_def[]  (NOTE:  must maintain same ordering between enum and
 * job_attr_def[] table in src/server/job_attr_def.c)
 * sync w/TJobAttr[] table in src/resmom/request.c
 *
 * @see struct job
 */

enum job_atr
  {
  JOB_ATR_jobname,   /* this set appears first as they show */
  JOB_ATR_job_owner, /* in a basic job status display       */
  JOB_ATR_resc_used,
  JOB_ATR_state,
  JOB_ATR_in_queue,
  JOB_ATR_at_server,    /* (5) */

  JOB_ATR_account,    /* the bulk of the attributes are in   */
  JOB_ATR_checkpoint, /* alphabetic order for no good reason */
  JOB_ATR_ctime,
  JOB_ATR_depend,
  JOB_ATR_errpath, /* (10) */
  JOB_ATR_exec_host,
  JOB_ATR_exec_port,
  JOB_ATR_exec_gpus,
  JOB_ATR_exectime,
  JOB_ATR_grouplst,
  JOB_ATR_hold,
  JOB_ATR_interactive,
  JOB_ATR_join,
  JOB_ATR_keep,
  JOB_ATR_mailpnts,
  JOB_ATR_mailuser,   /* (20) */
  JOB_ATR_mtime,         /* what triggers modification of mtime? (20) */
  JOB_ATR_outpath,
  JOB_ATR_priority,
  JOB_ATR_qtime,
  JOB_ATR_rerunable,
  JOB_ATR_resource,
  JOB_ATR_session_id,
  JOB_ATR_shell,
  JOB_ATR_stagein,
  JOB_ATR_stageout,           /* (30) */
  JOB_ATR_substate,
  JOB_ATR_userlst,
  JOB_ATR_variables,    /* (33) */
  /* this set contains private attributes,  */
  /* as such not sent to clients (status)   */

  JOB_ATR_euser, /* execution user name for MOM    */
  JOB_ATR_egroup, /* execution group name for MOM    */
  JOB_ATR_hashname, /* job name hashed into 14 characters   */
  JOB_ATR_hopcount,
  JOB_ATR_qrank,
  JOB_ATR_queuetype,
  JOB_ATR_sched_hint,   /* 40 */
  JOB_ATR_security,
  JOB_ATR_Comment,
  JOB_ATR_Cookie,
  JOB_ATR_altid, /* alternate job id, for irix6 = array id */
  JOB_ATR_etime, /* time job became eligible to run   */
  JOB_ATR_exitstat, /* exit status of job     */
  JOB_ATR_forwardx11,
  JOB_ATR_submit_args,
  JOB_ATR_job_array_id,
  JOB_ATR_job_array_request, /* (50) */
  JOB_ATR_umask,
  JOB_ATR_start_time,  /* time when job was first started */
  JOB_ATR_start_count, /* number of times the job has been started */
  JOB_ATR_checkpoint_dir,    /* directory where job checkpoint file is stored */
  JOB_ATR_checkpoint_name,   /* name of checkpoint file */
  JOB_ATR_checkpoint_time,   /* timestamp of start of last checkpoint */
  JOB_ATR_checkpoint_restart_status,   /* checkpoint restart status */
  JOB_ATR_restart_name,   /* name of checkpoint restart file */
  JOB_ATR_fault_tolerant, /* indicates if a job should keep going if it can't contact a sister while polling */
  JOB_ATR_comp_time,  /* time when job was completed */
  JOB_ATR_reported, /* tracks whether job has been reported to scheduler */
  JOB_ATR_jobtype,     /* opaque job type string */
  JOB_ATR_inter_cmd,      /* command for interactive job */
  JOB_ATR_job_radix,    /* 64 */
  JOB_ATR_total_runtime, /* the difference between JOB_ATR_comp_time and JOB_ATR_start_time */
  JOB_ATR_sister_list,  /* For job_radix. List of sisters in a particular radix */
  JOB_ATR_proxy_user,
  JOB_ATR_node_exclusive,
  JOB_ATR_submit_host,  /* host which submitted the job */
  JOB_ATR_init_work_dir,    /* initial working directory */
  JOB_ATR_pagg_id,
#ifdef NVIDIA_GPUS
  JOB_ATR_gpu_flags,   /* gpu flags - mode and reset flags */
#endif  /* NVIDIA_GPUS */
  JOB_ATR_job_id,
  JOB_ATR_arguments,
  JOB_ATR_reservation_id,
  JOB_ATR_login_node_id,
  JOB_ATR_login_prop,
  JOB_ATR_external_nodes,
  JOB_ATR_multi_req_alps,
#include "site_job_attr_enum.h"

  JOB_ATR_UNKN,  /* the special "unknown" type    */
  JOB_ATR_LAST  /* This MUST be LAST */
  };

/*
 * The "definitions" for the job attributes are in job_attr_def[],
 * it is also indexed by the JOB_ATR_... enums.
 */

extern attribute_def job_attr_def[];

#ifdef PBS_MOM
#include "tm_.h"

/*
** Track nodes with an array of structures which each
** point to a list of events
*/

typedef struct hnodent
  {
  int                 hn_node; /* host (node) identifier (index) */
  char               *hn_host; /* hostname of node */
  int                 hn_stream; /* stream to MOM on node */
  int                 hn_sister; /* save error for KILL_JOB event */
  unsigned short      hn_port; /*  resmom port default 15003 */
  tlist_head          hn_events; /* pointer to list of events */
  struct sockaddr_in  sock_addr;
  } hnodent;


typedef struct vnodent
  {
  tm_node_id  vn_node; /* user's vnode identifier */
  hnodent    *vn_host; /* parent (host) nodeent entry */
  int         vn_index; /* index (window) */
  } vnodent;


/*
** Mother Superior gets to hold an array of information from each
** of the other nodes for resource usage.
*/

typedef struct noderes
  {
  long nr_cput; /* cpu time */
  long nr_mem; /* memory */
  long nr_vmem; /* virtual memory */
  } noderes;




/* mother superior to sister communication */
/*   (sync w/TSisterFailure[] in resmom/mom_main.c) */

#define SISTER_OKAY 0
#define SISTER_KILLDONE 1000
#define SISTER_BADPOLL 1001
#define SISTER_EOF 1099


/* Flags for ji_flags (mom only) */

#define MOM_CHECKPOINT_ACTIVE 1 /* checkpoint in progress */
#define MOM_CHECKPOINT_POST  2 /* post checkpoint call returned */
#define MOM_HAS_NODEFILE 4 /* Mom wrote job PBS_NODEFILE */
#define MOM_NO_PROC  8 /* no procs found for job */
#define MOM_HAS_TMPDIR  16 /* Mom made a tmpdir */

#ifdef USESAVEDRESOURCES
#define MOM_JOB_RECOVERY   32  /* recovering dead job on restart */
#endif    /* USESAVEDRESOURCES */

#endif /* MOM */


typedef struct
  {
  void     *pjob;
  void     *ptask;

  char      sjr[2048];  /* provide buffer space for struct startjob_rtn */
  /* struct startjob_rtn defined in resmom/{OS}/mom_mach.h */

  int       ReadRC;     /* return code from read of child pipe */
  int       ReadCount;  /* number of bytes read from child pipe */

  int       ptc;

  /* NOTE:  change ptc_name to alloc to prevent race conditions */

  char     *ptc_name;  /* (ptr, no-alloc) */

  int       is_interactive;

  int       port_out;
  int       port_err;

  void     *pwdp;

  int       pipe_script[2];

  int       jsmpipe[2];     /* job starter to MOM for sid */
  int       upfds;
  int       mjspipe[2];     /* MOM to job starter for ack */
  int       downfds;
  } pjobexec_t;


/* on the server this array will replace many of the doubly linked-lists */
typedef struct all_jobs
  {
  resizable_array *ra;
  hash_table_t    *ht;

  pthread_mutex_t *alljobs_mutex;
  }all_jobs;





/**
 * THE JOB
 *
 * This structure is used by the server to maintain internal
 * quick access to the state and status of each job.
 * There is one instance of this structure per job known by the server.
 *
 * This information must be PRESERVED and is done so by updating the
 * job file in the jobs subdirectory which corresponds to this job.
 *
 * ji_state is the state of the job.  It is kept up front to provide for a
 * "quick" update of the job state with minimum rewritting of the job file.
 * Which is why the sub-struct ji_qs exists, that is the part which is
 * written on the "quick" save.
 *
 * The unparsed string set forms of the attributes (including resources)
 * are maintained in the struct attrlist as discussed above.
 *
 * @see job_alloc() - creates new job
 * @see job_free() - free job structure
 */

struct job
  {
  /* Note: these members, up to ji_qs, are not saved to disk
            (except for ji_stdout, ji_stderr) */

#ifndef PBS_MOM
  list_link       ji_jobque_array_sum;
#else
  list_link       ji_jobque;  /* used for polling in mom */
#endif
  /* MOM: links to polled jobs */
  time_t  ji_momstat; /* SVR: time of last status from MOM */
  /* MOM: time job suspend (Cray) */
  int  ji_modified; /* struct changed, needs to be saved */
  int  ji_momhandle; /* open connection handle to MOM */
  int  ji_radix;    /* number of nodes in a job radix. used for qsub -W job_radix option  */
#ifdef PBS_MOM    /* MOM ONLY */

  list_link       ji_alljobs; /* link to next job in server job list */
  struct grpcache *ji_grpcache; /* cache of user's groups */
  time_t  ji_checkpoint_time; /* periodic checkpoint time */
  time_t  ji_checkpoint_next; /* next checkpoint time */
  time_t  ji_sampletim;       /* last usage sample time, irix only */
  pid_t  ji_momsubt;          /* pid of mom subtask   */
  int (*ji_mompost)();        /* ptr to post processing func  */

  struct batch_request *ji_preq; /* hold request until finish_exec */
  int            ji_numnodes; /* number of nodes (at least 1) */
  int            ji_numsisternodes; /* number of nodes nodes for job_radix (at least 1) */
  int            ji_numvnod; /* number of virtual nodes */
  int            ji_numsistervnod; /* number of virtual nodes job_radix*/
  int            ji_outstanding;  /* number of nodes left to reply to an IM request*/
  tm_node_id     ji_im_nodeid;   /* node id of an intermediate mom. If set to 1 is intermediate mom. 0 leaf or mother superior */
  tm_node_id     ji_nodeid; /* my node id */
  tm_task_id     ji_taskid; /* generate task id's for job */
  char           ji_altid[PBS_MAXSVRJOBID + 1];
  tm_event_t     ji_obit; /* event for end-of-job */
  tm_event_t     ji_intermediate_join_event; /* event to write back from join job for intermediate moms */
  hnodent        *ji_hosts; /* ptr to job host management stuff */
  hnodent        *ji_sisters; /* ptr to job host management stuff for intermediate moms */
  vnodent        *ji_vnods; /* ptr to job vnode management stuff */
  vnodent        *ji_sister_vnods; /* ptr to job vnode management stuff for job_radix requests*/
  noderes        *ji_resources; /* ptr to array of node resources */
  tlist_head     ji_tasks; /* list of task structs */
  tm_node_id     ji_nodekill; /* set to nodeid requesting job die */
  int            ji_flags; /* mom only flags */
  char           ji_globid[64]; /* global job id */
  int            ji_portout; /* socket port allocated for ji_stdout */
  int            ji_porterr; /* socket port allocated for ji_stderr */
  int            ji_stdout; /* socket for stdout */
  int            ji_stderr; /* socket for stderr */
  int            ji_im_stdout;
  int            ji_im_stderr;
  int            ji_im_portout; /* for job_radix intermediate mom demux port for ji_stdout */
  int            ji_im_porterr; /* for job_radix intermediate mom demux port for ji_stderr */
  short          ji_job_is_being_rerun; /* special flag for jobs being rerun */
#ifdef PENABLE_LINUX26_CPUSETS
  int            ji_mempressure_curr;  /* current memory_pressure value */
  int            ji_mempressure_cnt;   /* counts MOM cycles memory_pressure is over threshold */
#endif

#else     /* END MOM ONLY */

  int               ji_has_delete_nanny;
  struct pbs_queue *ji_qhdr; /* current queue header */
  int               ji_lastdest; /* last destin tried by route */
  int               ji_retryok; /* ok to retry, some reject was temp */
  tlist_head        ji_rejectdest; /* list of rejected destinations */
  struct job_array *ji_arraystruct; /* pointer to job_array for this array */
  int               ji_is_array_template;    /* set to TRUE if this is a "template job" for a job array*/
  int               ji_have_nodes_request; /* set to TRUE if node spec uses keyword nodes */
  int               ji_cold_restart; /* set to TRUE if this job has been loaded through a cold restart */

  /* these three are only used for heterogeneous jobs */
  struct job       *ji_external_clone; /* the sub-job on the external (to the cray) nodes */
  struct job       *ji_cray_clone;     /* the sub-job on the cray nodes */
  struct job       *ji_parent_job;     /* parent job (only populated on the sub-jobs */

  pthread_mutex_t  *ji_mutex;
  char              ji_being_recycled;
#endif/* PBS_MOM */   /* END SERVER ONLY */

  /*
   * fixed size internal data - maintained via "quick save"
   * some of the items are copies of attributes, if so this
   * internal version takes precendent
   * 
   * NOTE: IF YOU MAKE ANY CHANGES TO THIS STRUCT THEN YOU ARE INTRODUCING 
   * AN INCOMPATIBILITY WITH .JB FILES FROM PREVIOUS VERSIONS OF TORQUE.
   * YOU SHOULD INCREMENT THE VERSION OF THE STRUCT AND PROVIDE APPROPRIATE 
   * SUPPORT IN joq_qs_upgrade() FOR UPGRADING PREVIOUS VERSIONS OF THIS 
   * STRUCT TO THE CURRENT VERSION.  ALSO NOTE THAT ANY CHANGES TO CONSTANTS
   * THAT DEFINE THE SIZE OF ANY ARRAYS IN THIS STRUCT ALSO INTRODUCE AN 
   * INCOMPATIBILITY WITH .JB FILES FROM PREVIOUS VERSIONS AND REQUIRE A NEW
   * STRUCT VERSION AND UPGRADE SUPPORT.
   */

  struct jobfix
    {
    int     qs_version;  /* quick save version */
    int     ji_state;    /* internal copy of state */
    int     ji_substate; /* job sub-state */
    int     ji_svrflags; /* server flags */
    int     ji_numattr;  /* number of attributes in list - never used, delete me */
    int     ji_ordering; /* special scheduling ordering - also never used */
    int     ji_priority; /* internal priority  - also never used */
    time_t  ji_stime;    /* time job started execution */
    char    ji_jobid[PBS_MAXSVRJOBID + 1];   /* job identifier */
    char    ji_fileprefix[PBS_JOBBASE + 1];  /* job file prefix */
    char    ji_queue[PBS_MAXQUEUENAME + 1];  /* name of current queue */
    char    ji_destin[PBS_MAXROUTEDEST + 1]; /* dest from qmove/route */
    /*
     * ji_destin gets set when we assign hosts and gets
     * cleared on failure to run job on mom or at obit_reply
     */
    int     ji_un_type;  /* type of ji_un union */
    union   /* depends on type of queue currently in */
      {
      struct   /* if in execution queue .. */
        {
        pbs_net_t ji_momaddr;  /* host addr of Server */
        unsigned short ji_momport;  /* host port of Server default 15002 */
        unsigned short ji_mom_rmport; /* host mom manager port of Server default 15003 */
        int       ji_exitstat; /* job exit status from MOM */
        } ji_exect;

      struct
        {
        time_t  ji_quetime;        /* time entered queue */
        time_t  ji_rteretry;       /* route retry time */
        } ji_routet;

      struct
        {
        pbs_net_t  ji_fromaddr;     /* host job coming from   */
        int        ji_fromsock; /* socket job coming over */
        int        ji_scriptsz; /* script size */
        } ji_newt;

      struct
        {
        pbs_net_t ji_svraddr;  /* host addr of Server */
        int       ji_exitstat; /* job exit status from MOM */
        uid_t     ji_exuid;    /* execution uid */
        gid_t     ji_exgid;    /* execution gid */
        } ji_momt;
      } ji_un;
    } ji_qs;

  /*
   * The following array holds the decode format of the attributes.
   * Its presence is for rapid acces to the attributes.
   */

  pbs_attribute ji_wattr[JOB_ATR_LAST]; /* decoded attributes  */

  int  maxAdoptedTaskId;  /* DJH 27 Feb 2002. Keep track of the task ids
                             the local mom allocates to adopted tasks; */
  };

typedef struct job job;



#ifndef PBS_MOM
#define INITIAL_JOB_SIZE           5000
#define JOB_NOT_FOUND             -1
#define MAX_RECYCLE_JOBS           5000
#define TOO_MANY_JOBS_IN_RECYCLER -1
#define JOBS_TO_REMOVE             1000




void initialize_all_jobs_array(struct all_jobs *);
int  insert_job(struct all_jobs *, job *);
int  insert_job_after(struct all_jobs *,job *before,job *after);
int  insert_job_after_index(struct all_jobs *, int index, job *after);
int  insert_job_first(struct all_jobs *,job *);
int  get_jobs_index(struct all_jobs *, job *);
int  remove_job(struct all_jobs *,job *);
int  has_job(struct all_jobs *,job *);
int  swap_jobs(struct all_jobs *,job *,job *);
struct pbs_queue *get_jobs_queue(job **);

job *next_job(struct all_jobs *,int *);
job *next_job_from_back(struct all_jobs *,int *);


typedef struct job_recycler
  {
  unsigned int     rc_next_id;
  struct all_jobs  rc_jobs;
  int              rc_iter;
  pthread_mutex_t *rc_mutex;
  } job_recycler;


int   insert_into_recycler(job *);
job  *get_recycled_job();
void  update_recycler_next_id();
void  initialize_recycler();
void  garbage_collect_recycling();

#endif



#ifdef PBS_MOM
/*
** Tasks are sessions belonging to a job, running on one of the
** nodes assigned to the job.
*/

typedef struct task
  {
  job  *ti_job; /* pointer to owning job */
  list_link ti_jobtask; /* links to tasks for this job */
  struct tcp_chan *ti_chan;  /* DIS file descriptor to task */
  int  ti_flags; /* task internal flags */
  tm_event_t ti_register; /* event if task registers - never used*/
  tlist_head ti_obits; /* list of obit events */
  tlist_head ti_info; /* list of named info */

  struct taskfix
    {
    char     ti_parentjobid[PBS_MAXSVRJOBID+1];
    tm_node_id ti_parentnode;
    tm_task_id ti_parenttask;
    tm_task_id ti_task; /* task's taskid */
    int  ti_status; /* status of task */
    pid_t  ti_sid;  /* session id */
    int  ti_exitstat; /* exit status */
    union
      {
      int ti_hold[16]; /* reserved space */
      } ti_u;
    } ti_qs;
  } task;



/*
** Events need to be linked to either a task or another event
** waiting at another MOM.  This is the information needed so
** we can forward the event to another MOM.
*/

typedef struct fwdevent
  {
  tm_node_id fe_node; /* where does notification go */
  tm_event_t fe_event; /* event number */
  tm_task_id fe_taskid; /* which task id */
  } fwdevent;

/*
** A linked list of eventent structures is maintained for all events
** for which we are waiting for another MOM to report back.
*/

typedef struct eventent
  {
  int  ee_command; /* command event is for */
  tm_event_t ee_event; /* event number */
  tm_event_t ee_parent_event; /* For job_radix calls. event of parent calling intermediate MOM */
  tm_task_id ee_taskid; /* which task id */
  fwdevent ee_forward; /* event to get notified */
  char  **ee_argv; /* save args for spawn */
  char  **ee_envp; /* save env for spawn */
  list_link ee_next; /* link to next one */
  } eventent;

/*
** A task can have events which are triggered when it exits.
** These are tracked by obitent structures linked to the task.
*/

typedef struct obitent
  {
  fwdevent oe_info; /* who gets the event */
  list_link oe_next; /* link to next one */
  } obitent;

/*
** A task can have a list of named infomation which it makes
** available to other tasks in the job.
*/

typedef struct infoent
  {
  char  *ie_name; /* published name */
  void  *ie_info; /* the glop */
  size_t ie_len;  /* how much glop */
  list_link ie_next; /* link to next one */
  } infoent;



#ifdef PBS_MOM
typedef struct job_file_delete_info
  {
  char           jobid[PBS_MAXSVRJOBID+1];
  char           prefix[PBS_JOBBASE + 1];
  char          *checkpoint_dir;
  unsigned char  has_temp_dir;
  unsigned char  has_node_file;
  gid_t          gid;
  uid_t          uid;
  } job_file_delete_info;
#endif




#define TI_FLAGS_INIT           1  /* task has called tm_init */
#define TI_FLAGS_CHECKPOINT     2  /* task has checkpointed */

#ifdef USESAVEDRESOURCES
#define TI_FLAGS_RECOVERY       4  /* recovering dead task on restart */
#endif    /* USESAVEDRESOURCES */

#define TI_STATE_EMBRYO  0
#define TI_STATE_RUNNING 1    /* includes suspended jobs */
#define TI_STATE_EXITED  2  /* ti_exitstat valid */
#define TI_STATE_DEAD    3


/*
**      Here is the set of commands for InterMOM (IM) requests.
*/

/* sync w/PMOMCommand[] */

#define IM_ALL_OKAY        0
#define IM_JOIN_JOB        1
#define IM_KILL_JOB        2
#define IM_SPAWN_TASK      3
#define IM_GET_TASKS       4
#define IM_SIGNAL_TASK     5
#define IM_OBIT_TASK       6
#define IM_POLL_JOB        7
#define IM_GET_INFO        8
#define IM_GET_RESC        9
#define IM_ABORT_JOB      10
#define IM_GET_TID        11
#define IM_RADIX_ALL_OK   12
#define IM_JOIN_JOB_RADIX 13
#define IM_KILL_JOB_RADIX 14
#define IM_MAX            15

#define IM_ERROR          99

eventent *event_alloc(int  command,
                           hnodent *pnode,
                           tm_event_t event,
                           tm_task_id taskid);

task *pbs_task_create(job *pjob, tm_task_id taskid);

task *task_find(job *pjob, tm_task_id taskid);
#else

#define LOCUTION_SUCCESS  0
#define LOCUTION_FAIL    -1
#define LOCUTION_REQUEUE -2

#define LOCUTION_SIZE     20


typedef struct send_job_request
  {
  char      *jobid;
  int        move_type;
  void      *data;
  } send_job_request;



#endif /* MOM */

/*
 * server flags (in ji_svrflags)
 */
#define JOB_SVFLG_HERE     0x01 /* SERVER: job created here */
/* MOM: set for Mother Superior */
#define JOB_SVFLG_HASWAIT  0x02 /* job has timed task entry for wait time */
#define JOB_SVFLG_HASRUN   0x04 /* job has been run before (being rerun */
#define JOB_SVFLG_HOTSTART 0x08 /* job was running, if hot init, restart */
#define JOB_SVFLG_CHECKPOINT_FILE    0x10 /* job has checkpoint file for restart */
#define JOB_SVFLG_SCRIPT   0x20 /* job has a Script file */
#define JOB_SVFLG_OVERLMT1 0x40 /* job over limit first time, MOM only */
#define JOB_SVFLG_OVERLMT2 0x80 /* job over limit second time, MOM only */
#define JOB_SVFLG_CHECKPOINT_MIGRATEABLE 0x100 /* job has migratable checkpoint */
#define JOB_SVFLG_Suspend  0x200 /* job suspended (signal suspend) */
#define JOB_SVFLG_StagedIn 0x400 /* job has files that have been staged in */
#define JOB_SVFLG_HasNodes 0x1000 /* job has nodes allocated to it */
#define JOB_SVFLG_RescAssn 0x2000 /* job resources accumulated in server/que */
#define JOB_SVFLG_CHECKPOINT_COPIED 0x4000 /* job checkpoint file that has been copied */
#define JOB_SVFLG_INTERMEDIATE_MOM  0x8000 /* This is for job_radix. I am an intermediate mom */

/*
 * Related defines
 */
#define SAVEJOB_QUICK 0
#define SAVEJOB_FULL  1
#define SAVEJOB_NEW   2

#define MAIL_NONE  (int)'n'
#define MAIL_ABORT (int)'a'
#define MAIL_BEGIN (int)'b'
#define MAIL_DEL   (int)'d'
#define MAIL_END   (int)'e'
#define MAIL_OTHER (int)'o'
#define MAIL_STAGEIN (int)'s'
#define MAIL_CHKPTCOPY (int)'c'
#define MAIL_NORMAL 0
#define MAIL_FORCE  1

#define JOB_FILE_COPY           ".JC"    /* tmp copy while updating */
#define JOB_FILE_SUFFIX         ".JB"    /* job control file */
#define JOB_FILE_BACKUP         ".BK"    /* job file backup */
#define JOB_SCRIPT_SUFFIX       ".SC"    /* job script file  */
#define JOB_STDOUT_SUFFIX       ".OU"    /* job standard out */
#define JOB_STDERR_SUFFIX       ".ER"    /* job standard error */
#define JOB_CHECKPOINT_SUFFIX   ".CK"    /* job checkpoint file */
#define JOB_TASKDIR_SUFFIX      ".TK"    /* job task directory */
#define JOB_BAD_SUFFIX          ".BD"    /* save bad job file */
#define JOB_FILE_TMP_SUFFIX     ".TA"    /* job array template file suffix */
/*
 * Job states are defined by POSIX as:
 */

/* sync w/PJobState[] */

#define JOB_STATE_TRANSIT 0
#define JOB_STATE_QUEUED 1
#define JOB_STATE_HELD  2
#define JOB_STATE_WAITING 3
#define JOB_STATE_RUNNING 4
#define JOB_STATE_EXITING 5
#define JOB_STATE_COMPLETE 6



/*
 * job sub-states are defined by PBS (more detailed) as:
 */

/* sync w/PJobSubState[] */

#define JOB_SUBSTATE_TRANSIN  00 /* Transit in, wait for commit */
#define JOB_SUBSTATE_TRANSICM 01 /* Transit in, wait for commit */
#define JOB_SUBSTATE_TRNOUT 02 /* transiting job outbound */
#define JOB_SUBSTATE_TRNOUTCM 03 /* transiting outbound, rdy to commit */

#define JOB_SUBSTATE_QUEUED 10 /* job queued and ready for selection */
#define JOB_SUBSTATE_PRESTAGEIN 11 /* job queued, has files to stage in */
#define JOB_SUBSTATE_SYNCRES 13 /* job waiting on sync start ready */
#define JOB_SUBSTATE_STAGEIN 14 /* job staging in files then wait */
#define JOB_SUBSTATE_STAGEGO 15 /* job staging in files and then run */
#define JOB_SUBSTATE_STAGECMP 16 /* job stage in complete */
#define JOB_SUBSTATE_CHKPTGO 17 /* job copy checkpoint file and then run */
#define JOB_SUBSTATE_CHKPTCMP 18 /* job copy checkpoint file complete */

#define JOB_SUBSTATE_HELD 20 /* job held - user or operator */
#define JOB_SUBSTATE_SYNCHOLD 21 /* job held - waiting on sync regist */
#define JOB_SUBSTATE_DEPNHOLD 22 /* job held - waiting on dependency */

#define JOB_SUBSTATE_WAITING 30 /* job waiting on execution time */
#define JOB_SUBSTATE_STAGEFAIL 37 /* job held - file stage in failed */

#define JOB_SUBSTATE_PRERUN     40      /* job sent to MOM to run */
#define JOB_SUBSTATE_STARTING   41      /* job sent to MOM - job start finalizing */
#define JOB_SUBSTATE_RUNNING 42 /* job running */
#define JOB_SUBSTATE_SUSPEND 43 /* job suspended, CRAY only */

#define JOB_SUBSTATE_EXITING 50 /* Start of job exiting processing */
#define JOB_SUBSTATE_EXIT_WAIT 51 /* Waiting for response from other moms
                                     or from server */
#define JOB_SUBSTATE_STAGEOUT 52 /* job staging out (other) files   */
#define JOB_SUBSTATE_STAGEDEL 53 /* job deleteing staged out files  */
#define JOB_SUBSTATE_EXITED 54 /* job exit processing completed   */
#define JOB_SUBSTATE_ABORT      55 /* job is being aborted by server  */
#define JOB_SUBSTATE_NOTERM_REQUE 56 /* (MOM) on mom initialization.
                                        Requeue job but do not terminate
                                        any running process */
#define JOB_SUBSTATE_PREOBIT    57 /* (MOM) preobit jobstat sent */
#define JOB_SUBSTATE_OBIT       58 /* (MOM) job obit notice sent */
#define JOB_SUBSTATE_COMPLETE   59 /* job is complete */

#define JOB_SUBSTATE_RERUN 60 /* job is rerun, recover output stage */
#define JOB_SUBSTATE_RERUN1 61 /* job is rerun, stageout phase */
#define JOB_SUBSTATE_RERUN2 62 /* job is rerun, delete files stage */
#define JOB_SUBSTATE_RERUN3 63 /* job is rerun, mom delete job */

#define JOB_SUBSTATE_RETURNSTD 70      /* job has checkpoint file, return
stdout / stderr files to server spool
dir so that job can be restarted */

#define JOB_SUBSTATE_ARRAY_TEMP 75  /* job is an array template */

/* decriminator for ji_un union type */

#define JOB_UNION_TYPE_NEW   0
#define JOB_UNION_TYPE_EXEC  1
#define JOB_UNION_TYPE_ROUTE 2
#define JOB_UNION_TYPE_MOM   3

/* job hold (internal) types */

#define HOLD_n 0
#define HOLD_u 1
#define HOLD_o 2
#define HOLD_s 4
/* jobs in a job array are held until the whole array is ready for now this is
 * a system hold, but it may be a special hold in the future */
#define HOLD_a 8
#define HOLD_l 16 /* job is on hold due to slot limits */

/* Special Job Exit Values,  Set by the job starter (child of MOM)   */
/* see server/req_jobobit() & mom/start_exec.c        */

#define JOB_EXEC_OK          0 /* job exec successful */
#define JOB_EXEC_FAIL1      -1 /* job exec failed, before files, no retry */
#define JOB_EXEC_FAIL2      -2 /* job exec failed, after files, no retry  */
#define JOB_EXEC_RETRY      -3 /* job execution failed, do retry    */
#define JOB_EXEC_INITABT    -4 /* job aborted on MOM initialization */
#define JOB_EXEC_INITRST    -5 /* job aborted on MOM init, checkpoint, no migrate */
#define JOB_EXEC_INITRMG    -6 /* job aborted on MOM init, checkpoint, ok migrate */
#define JOB_EXEC_BADRESRT   -7 /* job restart failed */
#define JOB_EXEC_CMDFAIL    -8 /* exec() of user command failed */
#define JOB_EXEC_STDOUTFAIL -9 /* could not create/open stdout stderr files */
#define JOB_EXEC_OVERLIMIT  -10

extern void  add_dest(job *);
extern void  depend_clrrdy(job *);
extern int   depend_on_que(pbs_attribute *, void *, int);
extern int   depend_on_exec(job *);
extern int   depend_on_term(job *);
job         *find_job_regular_jobs(char *);
job         *find_job_array_jobs(char *);
extern char *get_egroup(job *);
extern char *get_variable(job *, char *);
extern int   init_chkmom(job *);
extern void  issue_track(job *);
extern int   job_abt(job **, char *);
extern job  *job_alloc();
extern int   job_unlink_file(job *pjob, const char *name);
#ifndef PBS_MOM
job         *job_clone(job *,struct job_array *, int);
job         *svr_find_job(char *jobid, int get_subjob);
#else
extern job  *mom_find_job(char *);
#endif
extern job  *job_recov(char *);
extern int   job_save(job *, int, int);
extern int   modify_job_attr(job *, svrattrl *, int, int *);
extern char *prefix_std_file(job *, dynamic_string *, int);
extern char *add_std_filename(job *, char *, int, dynamic_string *);
extern int   set_jobexid(job *, pbs_attribute *, char *);
extern int   site_check_user_map(job *, char *, char *, int);
int  svr_dequejob(char *, int);
extern int   svr_enquejob(job *, int, int);
extern void  svr_evaljobstate(job *, int *, int *, int);
extern void  svr_mailowner(job *, int, int, char *);
extern void  set_resc_deflt(job *, pbs_attribute *, int);
extern void  set_statechar(job *);
extern int   svr_setjobstate(job *, int, int, int);
int          split_job(job *);

#ifdef BATCH_REQUEST_H
extern job  *chk_job_request(char *, struct batch_request *);
extern int   net_move(job *, struct batch_request *);
extern int   svr_chk_owner(struct batch_request *, job *);

extern struct batch_request *cpy_stage(struct batch_request *, job *, enum job_atr, int);
extern struct batch_request *setup_cpyfiles(struct batch_request *, job *, char *, char *, int, int);
extern struct batch_request *cpy_checkpoint(struct batch_request *, job *, enum job_atr, int);
int   issue_signal(job **, char *, void(*)(batch_request *), void *);
#endif /* BATCH_REQUEST_H */



#endif /* PBS_JOB_H */

