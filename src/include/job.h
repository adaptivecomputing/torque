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
 *	<sys/types.h>
 *	"list_link.h"
 *	"attribute.h"
 *	"server_limits.h"
 */

#ifndef JOB_H
#define JOB_H 1
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

struct depend {
	list_link dp_link;	/* link to next dependency, if any       */
	short	  dp_type;	/* type of dependency (all) 	         */
	short	  dp_numexp;	/* num jobs expected (on or syncct only) */
	short	  dp_numreg;	/* num jobs registered (syncct only)     */
	short	  dp_released;	/* This job released to run (syncwith)   */
	tlist_head dp_jobs;	/* list of related jobs  (all)           */
};

/*
 * The depend_job structure is used to record the name and location
 * of each job which is involved with the dependency
 */

struct depend_job {
	list_link dc_link;
	short	dc_state;	/* released / ready to run (syncct)	 */
	long	dc_cost;	/* cost of this child (syncct)		 */
	char	dc_child[PBS_MAXSVRJOBID+1]; /* child (dependent) job	 */
	char	dc_svr[PBS_MAXSERVERNAME+1]; /* server owning job	 */
};

/*
 * Warning: the relation between the numbers assigned to after* and before*
 * is critical.
 */
#define JOB_DEPEND_TYPE_AFTERSTART	 0
#define JOB_DEPEND_TYPE_AFTEROK		 1
#define JOB_DEPEND_TYPE_AFTERNOTOK	 2
#define JOB_DEPEND_TYPE_AFTERANY	 3
#define JOB_DEPEND_TYPE_BEFORESTART	 4
#define JOB_DEPEND_TYPE_BEFOREOK	 5
#define JOB_DEPEND_TYPE_BEFORENOTOK	 6
#define JOB_DEPEND_TYPE_BEFOREANY	 7
#define JOB_DEPEND_TYPE_ON		 8
#define JOB_DEPEND_TYPE_SYNCWITH	 9
#define JOB_DEPEND_TYPE_SYNCCT		10
#define JOB_DEPEND_NUMBER_TYPES		11

#define JOB_DEPEND_OP_REGISTER		1
#define JOB_DEPEND_OP_RELEASE		2
#define JOB_DEPEND_OP_READY		3
#define JOB_DEPEND_OP_DELETE		4
#define JOB_DEPEND_OP_UNREG		5

/*
 * The badplace structure is used to keep track of destinations
 * which have been tried by a route queue and given a "reject"
 * status back, see svr_movejob.c.
 */
typedef struct	badplace {
  list_link	bp_link;
  char		bp_dest[PBS_MAXROUTEDEST + 1];
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
struct grpcache {
  int    gc_ngroup;	/* number of active supplementary groups */
  int    gc_groups[NGROUPS_MAX];
  char   gc_homedir[1];	/* more space allocated as part of this	 */
			/* structure following here		 */
  };
#endif /* END NGROUPS_MAX */
	
/*
 * Job attributes/resources are maintained in one of two ways.
 * Most of the attributes are maintained in a decoded or parsed form.
 * This allows quick access to the attribute and resource values
 * when making decisions about the job (scheduling, routing, ...),
 * see ji_wattr[].
 *
 * Any attribute or resource which is not recognized on this server
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

/* sync w/XXX */

enum job_atr {
  JOB_ATR_jobname,	/* this set appears first as they show */
  JOB_ATR_job_owner,	/* in a basic job status display       */
  JOB_ATR_resc_used,
  JOB_ATR_state,
  JOB_ATR_in_queue,
  JOB_ATR_at_server,    /* (5) */

  JOB_ATR_account,	/* the bulk of the attributes are in   */
  JOB_ATR_chkpnt,	/* alphabetic order for no good reason */
  JOB_ATR_ctime,
  JOB_ATR_depend,
  JOB_ATR_errpath,
  JOB_ATR_exec_host,
  JOB_ATR_exectime,
  JOB_ATR_grouplst,
  JOB_ATR_hold,
  JOB_ATR_interactive,
  JOB_ATR_join,
  JOB_ATR_keep,
  JOB_ATR_mailpnts,
  JOB_ATR_mailuser,
  JOB_ATR_mtime,         /* (20) */
  JOB_ATR_outpath,
  JOB_ATR_priority,
  JOB_ATR_qtime,
  JOB_ATR_rerunable,
  JOB_ATR_resource,
  JOB_ATR_session_id,
  JOB_ATR_shell,
  JOB_ATR_stagein,
  JOB_ATR_stageout,
  JOB_ATR_substate,
  JOB_ATR_userlst,
  JOB_ATR_variables,    /* (32) */
			/* this set contains private attributes,  */
			/* as such not sent to clients (status)   */

  JOB_ATR_euser,	/* execution user name for MOM		  */
  JOB_ATR_egroup,	/* execution group name for MOM		  */
  JOB_ATR_hashname,	/* job name hashed into 14 characters	  */
  JOB_ATR_hopcount,
  JOB_ATR_qrank,
  JOB_ATR_queuetype,
  JOB_ATR_sched_hint,   /* 39 */
  JOB_ATR_security,	
  JOB_ATR_Comment,
  JOB_ATR_Cookie,
  JOB_ATR_altid,	/* alternate job id, for irix6 = array id */
  JOB_ATR_etime,	/* time job became eligible to run	  */
  JOB_ATR_exitstat,	/* exit status of job			  */
  JOB_ATR_forwardx11,
  JOB_ATR_submit_args,
  JOB_ATR_job_array_size,
  JOB_ATR_job_array_id,
  JOB_ATR_umask,
#include "site_job_attr_enum.h"

  JOB_ATR_UNKN,		/* the special "unknown" type		  */
  JOB_ATR_LAST		/* This MUST be LAST	*/
  };

/*
 * The "definitions" for the job attributes are in the following array,
 * it is also indexed by the JOB_ATR_... enums.
 */

extern attribute_def job_attr_def[];

#ifdef	PBS_MOM
#include "tm_.h"

/*
**	Track nodes with an array of structures which each
**	point to a list of events
*/
typedef	struct	hnodent {
	int		hn_node;	/* host (node) identifier (index) */
	char	       *hn_host;	/* hostname of node */
	int		hn_stream;	/* stream to MOM on node */
	int		hn_sister;	/* save error for KILL_JOB event */
	tlist_head	hn_events;	/* pointer to list of events */
  } hnodent;


typedef struct vnodent {
  tm_node_id  vn_node;	/* user's vnode identifier */
  hnodent    *vn_host;	/* parent (host) nodeent entry */
  int         vn_index;	/* index (window) */
  } vnodent;


/*
**	Mother Superior gets to hold an array of information from each
**	of the other nodes for resource usage.
*/

typedef struct	noderes {
  long nr_cput;	/* cpu time */
  long nr_mem;	/* memory */
  long nr_vmem; /* virtual memory */
  } noderes;




/* mother superior to sister communication */
/*   (sync w/TSisterFailure[] in resmom/mom_main.c) */

#define	SISTER_OKAY	0
#define SISTER_KILLDONE	1000
#define SISTER_BADPOLL	1001
#define SISTER_EOF	1099


/* Flags for ji_flags (mom only) */

#define	MOM_CHKPT_ACTIVE	1	/* checkpoint in progress */
#define	MOM_CHKPT_POST		2	/* post checkpoint call returned */
#define MOM_HAS_NODEFILE	4	/* Mom wrote job PBS_NODEFILE */
#define MOM_NO_PROC		8	/* no procs found for job */
#define MOM_HAS_TMPDIR		16	/* Mom made a tmpdir */
#endif	/* MOM */

/*
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
 */

typedef struct {
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
  
  
  
  

/* job array stuff. */ 
#ifndef PBS_MOM

#define ARRAY_FILE_SUFFIX ".AR"

#define ARRAY_STRUCT_VERSION 1

  /* pbs_server will keep a list of these structs, with one struct per job array*/
  struct array_job_list {
     list_link all_arrays;
     tlist_head array_alljobs;

     struct array_info {
       int  struct_version;
       char parent_id[PBS_MAXSVRJOBID + 1];
       int  array_size;
       char fileprefix[PBS_JOBBASE + 1];
       int  num_cloned;
     } ai_qs; 
  
  };
  
  typedef struct array_job_list array_job_list;

#endif


struct job {

	/* Note: these members, up to ji_qs, are not saved to disk
           (except for ji_stdout, ji_stderr) */

	list_link       ji_alljobs;	/* links to all jobs in server */
	list_link       ji_jobque;	/* SVR: links to jobs in same queue */
					/* MOM: links to polled jobs */
	time_t		ji_momstat;	/* SVR: time of last status from MOM */
					/* MOM: time job suspend (Cray)	*/
	int		ji_modified;	/* struct changed, needs to be saved */
	int		ji_momhandle;	/* open connection handle to MOM */
#ifdef	PBS_MOM				/* MOM ONLY */
	struct grpcache *ji_grpcache;	/* cache of user's groups */
	time_t		ji_chkpttime;	/* periodic checkpoint time */
	time_t		ji_chkptnext;	/* next checkpoint time */
	time_t		ji_sampletim;	/* last usage sample time, irix only */
	pid_t		ji_momsubt;	/* pid of mom subtask   */
	int	      (*ji_mompost)();	/* ptr to post processing func  */
	struct batch_request *ji_preq;	/* hold request until finish_exec */
	int		ji_numnodes;	/* number of nodes (at least 1) */
	int		ji_numvnod;	/* number of virtual nodes */
	tm_node_id	ji_nodeid;	/* my node id */
	tm_task_id	ji_taskid;	/* generate task id's for job */
	tm_event_t	ji_obit;	/* event for end-of-job */
	hnodent	       *ji_hosts;	/* ptr to job host management stuff */
	vnodent	       *ji_vnods;	/* ptr to job vnode management stuff */
	noderes	       *ji_resources;	/* ptr to array of node resources */
	tlist_head       ji_tasks;	/* list of task structs */
	tm_node_id	ji_nodekill;	/* set to nodeid requesting job die */
	int		ji_flags;	/* mom only flags */
	char	       *ji_globid;	/* global job id */
	int		ji_stdout;	/* port for stdout */
	int		ji_stderr;	/* port for stderr */
#else					/* END MOM ONLY */
	tlist_head	ji_svrtask;	/* links to svr work_task list */
	struct pbs_queue  *ji_qhdr;	/* current queue header */
	int		ji_lastdest;	/* last destin tried by route */
	int		ji_retryok;	/* ok to retry, some reject was temp */
	tlist_head	ji_rejectdest;	/* list of rejected destinations */
	list_link	ji_arrayjobs;	/* links to all jobs in same array */
	array_job_list	*ji_arrayjoblist; /* pointer to array_job_list for this array */
#endif					/* END SERVER ONLY */

	/*
	 * fixed size internal data - maintained via "quick save" 
	 * some of the items are copies of attributes, if so this
	 * internal version takes precendent
	 */

	struct jobfix {
	    int     qs_version;		/* quick save version */
	    int	    ji_state;		/* internal copy of state */
	    int	    ji_substate;	/* job sub-state */
	    int	    ji_svrflags;	/* server flags */
	    int	    ji_numattr;		/* number of attributes in list */
	    int	    ji_ordering;	/* special scheduling ordering */
	    int	    ji_priority;	/* internal priority */
	    time_t  ji_stime;		/* time job started execution */
	    char    ji_jobid[PBS_MAXSVRJOBID + 1];   /* job identifier */
	    char    ji_fileprefix[PBS_JOBBASE + 1];  /* job file prefix */
	    char    ji_queue[PBS_MAXQUEUENAME + 1];  /* name of current queue */
	    char    ji_destin[PBS_MAXROUTEDEST + 1]; /* dest from qmove/route */
	    int	    ji_un_type;		/* type of ji_un union */
	    union {	/* depends on type of queue currently in */
		struct {	/* if in execution queue .. */
	     	    pbs_net_t ji_momaddr;  /* host addr of Server */
		    int	      ji_exitstat; /* job exit status from MOM */
		} ji_exect;
		struct {
		    time_t  ji_quetime;		      /* time entered queue */
		    time_t  ji_rteretry;	      /* route retry time */
		} ji_routet;
		struct {
                    pbs_net_t  ji_fromaddr;     /* host job coming from   */
		    int	       ji_fromsock;	/* socket job coming over */
		    int	       ji_scriptsz;	/* script size */
		} ji_newt;
		struct {
	     	    pbs_net_t ji_svraddr;  /* host addr of Server */
		    int	      ji_exitstat; /* job exit status from MOM */
		    uid_t     ji_exuid;	   /* execution uid */
		    gid_t     ji_exgid;	   /* execution gid */
		} ji_momt;
	    } ji_un;
	} ji_qs;

	/*
	 * The following array holds the decode	format of the attributes.
	 * Its presence is for rapid acces to the attributes.
	 */

  attribute ji_wattr[JOB_ATR_LAST]; /* decoded attributes  */
  };

typedef struct job job;


#ifdef	PBS_MOM
/*
**	Tasks are sessions belonging to a job, running on one of the
**	nodes assigned to the job.
*/
typedef struct	task {
	job		*ti_job;	/* pointer to owning job */
	list_link	ti_jobtask;	/* links to tasks for this job */
	int		ti_fd;		/* DIS file descriptor to task */
	int		ti_flags;	/* task internal flags */
	tm_event_t	ti_register;	/* event if task registers */
	tlist_head	ti_obits;	/* list of obit events */
	tlist_head	ti_info;	/* list of named info */
	struct taskfix {
		char    	ti_parentjobid[PBS_MAXSVRJOBID+1];
		tm_node_id	ti_parentnode;
		tm_task_id	ti_parenttask;
		tm_task_id	ti_task;	/* task's taskid */
		int		ti_status;	/* status of task */
		pid_t		ti_sid;		/* session id */
		int		ti_exitstat;	/* exit status */
		union {
			int	ti_hold[16];	/* reserved space */
		} ti_u;
	} ti_qs;
} task;

/*
**	Events need to be linked to either a task or another event
**	waiting at another MOM.  This is the information needed so
**	we can forward the event to another MOM.
*/
typedef struct	fwdevent {
	tm_node_id	fe_node;	/* where does notification go */
	tm_event_t	fe_event;	/* event number */
	tm_task_id	fe_taskid;	/* which task id */
} fwdevent;

/*
**	A linked list of eventent structures is maintained for all events
**	for which we are waiting for another MOM to report back.
*/
typedef struct	eventent {
	int		ee_command;	/* command event is for */
	tm_event_t	ee_event;	/* event number */
	tm_task_id	ee_taskid;	/* which task id */
	fwdevent	ee_forward;	/* event to get notified */
	char		**ee_argv;	/* save args for spawn */
	char		**ee_envp;	/* save env for spawn */
	list_link	ee_next;	/* link to next one */
} eventent;

/*
**	A task can have events which are triggered when it exits.
**	These are tracked by obitent structures linked to the task.
*/
typedef struct	obitent {
	fwdevent	oe_info;	/* who gets the event */
	list_link	oe_next;	/* link to next one */
} obitent;

/*
**	A task can have a list of named infomation which it makes
**	available to other tasks in the job.
*/
typedef struct	infoent {
  char		*ie_name;	/* published name */
  void		*ie_info;	/* the glop */
  size_t	ie_len;		/* how much glop */
  list_link	ie_next;	/* link to next one */
  } infoent;

#define	TI_FLAGS_INIT		1		/* task has called tm_init */
#define	TI_FLAGS_CHKPT		2		/* task has checkpointed */

#define TI_STATE_EMBRYO  0
#define TI_STATE_RUNNING 1              /* includes suspended jobs */
#define TI_STATE_EXITED	 2		/* ti_exitstat valid */
#define TI_STATE_DEAD    3

/*
**      Here is the set of commands for InterMOM (IM) requests.
*/

/* sync w/PMOMCommand[] */

#define IM_ALL_OKAY	0
#define IM_JOIN_JOB	1
#define IM_KILL_JOB	2
#define IM_SPAWN_TASK	3
#define IM_GET_TASKS	4
#define IM_SIGNAL_TASK	5
#define IM_OBIT_TASK	6
#define IM_POLL_JOB	7
#define IM_GET_INFO	8
#define IM_GET_RESC	9
#define IM_ABORT_JOB	10
#define IM_GET_TID	11
#define IM_ERROR	99

#define IM_MAX          12

eventent *event_alloc A_((
  int		command,
  hnodent	*pnode,
  tm_event_t	event,
  tm_task_id	taskid));

task *pbs_task_create A_((	
  job *pjob,
  tm_task_id	taskid));

task *task_find A_((
  job *pjob,
  tm_task_id	taskid));

#endif	/* MOM */

/*
 * server flags (in ji_svrflags)
 */
#define JOB_SVFLG_HERE     0x01	/* SERVER: job created here */
				/* MOM: set for Mother Superior */
#define JOB_SVFLG_HASWAIT  0x02 /* job has timed task entry for wait time */
#define JOB_SVFLG_HASRUN   0x04	/* job has been run before (being rerun */
#define JOB_SVFLG_HOTSTART 0x08	/* job was running, if hot init, restart */
#define JOB_SVFLG_CHKPT	   0x10 /* job has checkpoint file for restart */
#define JOB_SVFLG_SCRIPT   0x20	/* job has a Script file */
#define JOB_SVFLG_OVERLMT1 0x40 /* job over limit first time, MOM only */
#define JOB_SVFLG_OVERLMT2 0x80 /* job over limit second time, MOM only */
#define JOB_SVFLG_ChkptMig 0x100 /* job has migratable checkpoint */
#define JOB_SVFLG_Suspend  0x200 /* job suspended (signal suspend) */
#define JOB_SVFLG_StagedIn 0x400 /* job has files that have been staged in */
#define JOB_SVFLG_HasNodes 0x1000 /* job has nodes allocated to it */
#define JOB_SVFLG_RescAssn 0x2000 /* job resources accumulated in server/que */

/*
 * Related defines
 */
#define SAVEJOB_QUICK 0
#define SAVEJOB_FULL  1
#define SAVEJOB_NEW   2
#define SAVEJOB_ARY   3  /* used to temporarily save the "parent" for a job array */

#define MAIL_NONE  (int)'n'
#define MAIL_ABORT (int)'a'
#define MAIL_BEGIN (int)'b'
#define MAIL_DEL   (int)'d'
#define MAIL_END   (int)'e'
#define MAIL_OTHER (int)'o'
#define MAIL_STAGEIN (int)'s'
#define MAIL_NORMAL 0
#define MAIL_FORCE  1

#define JOB_FILE_COPY      ".JC"	/* tmp copy while updating */
#define JOB_FILE_SUFFIX    ".JB"	/* job control file */
#define JOB_SCRIPT_SUFFIX  ".SC"	/* job script file  */
#define JOB_STDOUT_SUFFIX  ".OU"	/* job standard out */
#define JOB_STDERR_SUFFIX  ".ER"	/* job standard error */
#define JOB_CKPT_SUFFIX    ".CK"	/* job checkpoint file */
#define JOB_TASKDIR_SUFFIX ".TK"	/* job task directory */
#define JOB_BAD_SUFFIX	   ".BD"	/* save bad job file */

/*
 * Job states are defined by POSIX as:
 */

/* sync w/PJobState[] */

#define JOB_STATE_TRANSIT	0
#define JOB_STATE_QUEUED	1
#define JOB_STATE_HELD		2
#define JOB_STATE_WAITING	3
#define JOB_STATE_RUNNING	4
#define JOB_STATE_EXITING	5
#define JOB_STATE_COMPLETE	6



/*
 * job sub-states are defined by PBS (more detailed) as:
 */

/* sync w/PJobSubState[] */

#define JOB_SUBSTATE_TRANSIN 	00	/* Transit in, wait for commit */
#define JOB_SUBSTATE_TRANSICM	01	/* Transit in, wait for commit */
#define JOB_SUBSTATE_TRNOUT	02	/* transiting job outbound */
#define JOB_SUBSTATE_TRNOUTCM	03	/* transiting outbound, rdy to commit */

#define JOB_SUBSTATE_QUEUED	10	/* job queued and ready for selection */
#define JOB_SUBSTATE_PRESTAGEIN	11	/* job queued, has files to stage in */
#define JOB_SUBSTATE_SYNCRES	13	/* job waiting on sync start ready */
#define JOB_SUBSTATE_STAGEIN	14	/* job staging in files then wait */
#define JOB_SUBSTATE_STAGEGO	15	/* job staging in files and then run */	
#define JOB_SUBSTATE_STAGECMP	16	/* job stage in complete */

#define JOB_SUBSTATE_HELD	20	/* job held - user or operator */
#define JOB_SUBSTATE_SYNCHOLD	21	/* job held - waiting on sync regist */
#define JOB_SUBSTATE_DEPNHOLD	22	/* job held - waiting on dependency */

#define JOB_SUBSTATE_WAITING	30	/* job waiting on execution time */
#define JOB_SUBSTATE_STAGEFAIL	37	/* job held - file stage in failed */

#define JOB_SUBSTATE_PRERUN     40      /* job sent to MOM to run */
#define JOB_SUBSTATE_STARTING   41      /* job sent to MOM - job start finalizing */
#define JOB_SUBSTATE_RUNNING	42	/* job running */
#define JOB_SUBSTATE_SUSPEND	43	/* job suspended, CRAY only */

#define JOB_SUBSTATE_EXITING	50	/* Start of job exiting processing */
#define JOB_SUBSTATE_STAGEOUT	51	/* job staging out (other) files   */
#define JOB_SUBSTATE_STAGEDEL	52	/* job deleteing staged out files  */
#define JOB_SUBSTATE_EXITED	53	/* job exit processing completed   */
#define JOB_SUBSTATE_ABORT      54	/* job is being aborted by server  */
#define JOB_SUBSTATE_PREOBIT    57	/* (MOM) preobit jobstat sent */
#define JOB_SUBSTATE_OBIT       58	/* (MOM) job obit notice sent */
#define JOB_SUBSTATE_COMPLETE   59	/* job is complete */

#define JOB_SUBSTATE_RERUN	60	/* job is rerun, recover output stage */
#define JOB_SUBSTATE_RERUN1	61	/* job is rerun, stageout phase */
#define JOB_SUBSTATE_RERUN2	62	/* job is rerun, delete files stage */
#define JOB_SUBSTATE_RERUN3	63	/* job is rerun, mom delete job */

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

/* Special Job Exit Values,  Set by the job starter (child of MOM)   */
/* see server/req_jobobit() & mom/start_exec.c			     */

#define JOB_EXEC_OK	   0	/* job exec successful */
#define JOB_EXEC_FAIL1	  -1	/* job exec failed, before files, no retry */
#define JOB_EXEC_FAIL2	  -2	/* job exec failed, after files, no retry  */
#define JOB_EXEC_RETRY	  -3	/* job execution failed, do retry    */
#define JOB_EXEC_INITABT  -4	/* job aborted on MOM initialization */
#define JOB_EXEC_INITRST  -5	/* job aborted on MOM init, chkpt, no migrate */
#define JOB_EXEC_INITRMG  -6	/* job aborted on MOM init, chkpt, ok migrate */
#define JOB_EXEC_BADRESRT -7	/* job restart failed */
#define JOB_EXEC_CMDFAIL  -8	/* exec() of user command failed */

extern void  add_dest A_((job *));
extern void  depend_clrrdy A_((job *));
extern int   depend_on_que A_((attribute *, void *, int));
extern int   depend_on_exec A_((job *));
extern int   depend_on_term A_((job *));
extern job  *find_job A_((char *));
extern char *get_egroup A_((job *));
extern char *get_variable A_((job *, char *));
extern int   init_chkmom A_((job *));
extern void  issue_track A_((job *));
extern int   job_abt A_((job **,char *));
extern job  *job_alloc();
extern job  *job_clone A_((job *,int));
extern void  job_free A_((job *));
extern void  job_purge A_((job *));
extern job  *job_recov A_((char *));
extern int   job_save A_((job *,int));
extern int   modify_job_attr A_((job *,svrattrl *,int,int *));
extern char *prefix_std_file A_((job *,int));
extern int   set_jobexid A_((job *,attribute *,char *));
extern int   site_check_user_map A_((job *,char *,char *));
extern void  svr_dequejob A_((job *));
extern int   svr_enquejob A_((job *));
extern void  svr_evaljobstate A_((job *,int *,int *,int));
extern void  svr_mailowner A_((job *,int,int,char *));
extern void  set_resc_deflt A_((job *,attribute *));
extern void  set_statechar A_((job *));
extern int   svr_setjobstate A_((job *,int,int));

#ifdef BATCH_REQUEST_H
extern job  *chk_job_request A_((char *, struct batch_request *));
extern int   net_move A_((job *, struct batch_request *));
extern int   svr_chk_owner A_((struct batch_request *,job *));
extern struct batch_request *cpy_stage A_((struct batch_request *,job *,enum job_atr,int));
#endif	/* BATCH_REQUEST_H */

#ifdef QUEUE_H
extern int   svr_chkque A_((job *,pbs_queue *,char *,int,char *));
extern int   default_router A_((job *,pbs_queue *,long));
extern int   site_alt_router A_((job *,pbs_queue *,long));
extern int   site_acl_check A_((job *,pbs_queue *));
#endif	/* QUEUE_H */

#ifdef WORK_TASK_H
extern void  job_clone_wt A_((struct work_task *));
extern int   issue_signal A_((job *,char *,void(*)(struct work_task *),void *));
extern void   on_job_exit A_((struct work_task *));
#endif	/* WORK_TASK_H */

#endif	/* JOB_H */

