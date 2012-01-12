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
 * Definition of basic batch request and reply strutures
 *
 * The basic batch request structure holds certain useful information
 * and a union of the "encode method" independent batch request data.
 *
 * This data is obtained by the encode dependent routine.
 *
 * Other required header files:
 * "list_link.h"
 * "server_limits.h"
 * "attributes.h"
 * "credential.h"
 * "BATCH-types.h"   - for protocol definition of request numbers
 *
 * First we define the reply structure as it is contained within the
 * request structure.
 */

#include "libpbs.h"
#include "attribute.h"
#include "credential.h"

#ifndef BATCH_REQUEST_H
#define BATCH_REQUEST_H 1

/*
 * The rest of this stuff is for the Batch Request Structure
 */

#define PBS_SIGNAMESZ 16

/*
 * The following strutures make up the union of encode independent
 * request data.
 *
 * The list of attributes, used in QueueJob, SelectJobs, PullJobs, and
 * Manager is an svrattrlist structure defined in "attribute.h"
 */

/* QueueJob */

struct rq_queuejob
  {
  char     rq_destin[PBS_MAXDEST+1];
  char     rq_jid[PBS_MAXSVRJOBID+1];
  tlist_head    rq_attr; /* svrattrlist */
  };

/* JobCredential */

struct rq_jobcred
  {
  int    rq_type;
  long    rq_size;
  char   *rq_data;
  };

/* Job File */

struct rq_jobfile
  {
  int  rq_sequence;
  int  rq_type;
  long  rq_size;
  char  rq_jobid[PBS_MAXSVRJOBID+1];
  char *rq_data;
  };

/*
 * job or destination id - used by RdyToCommit, Commit, RerunJob,
 * status ..., locate job, and run job - is just a char *
 *
 * Manage - used by Manager, DeleteJob, ReleaseJob, ModifyJob
 */

struct rq_manage
  {
  int      rq_cmd;
  int      rq_objtype;
  char      rq_objname[PBS_MAXSVRJOBID+1];
  tlist_head    rq_attr; /* svrattrlist */
  };

/* HoldJob -  plus preference flag */

struct rq_hold
  {

  struct rq_manage rq_orig;
  int   rq_hpref;
  };

/* MessageJob */

struct rq_message
  {
  int  rq_file;
  char  rq_jid[PBS_MAXSVRJOBID+1];
  char *rq_text;
  };

/* MoveJob */

struct rq_move
  {
  char rq_jid[PBS_MAXSVRJOBID+1];
  char rq_destin[(PBS_MAXSVRJOBID > PBS_MAXDEST ? PBS_MAXSVRJOBID:PBS_MAXDEST)+1];
  };

/* Resource Query/Reserve/Free */

struct rq_rescq
  {
  int     rq_rhandle;
  int     rq_num;
  char  **rq_list;
  };

/* RunJob */

struct rq_runjob
  {
  char rq_jid[PBS_MAXSVRJOBID + 1];  /* requested job id */
  char  *rq_destin;   /* destination hostlist */
  unsigned int rq_resch;
  };

/* SignalJob */

struct rq_signal
  {
  char rq_jid[PBS_MAXSVRJOBID+1];
  char rq_signame[PBS_SIGNAMESZ+1];
  };

/* Status (job, queue, server) */

struct rq_status
  {
  char   rq_id[(PBS_MAXSVRJOBID > PBS_MAXDEST ? PBS_MAXSVRJOBID:PBS_MAXDEST)+1];
  tlist_head rq_attr;
  };

/* TrackJob */

struct rq_track
  {
  int  rq_hopcount;
  char  rq_jid[PBS_MAXSVRJOBID+1];
  char  rq_location[PBS_MAXDEST+1];
  char  rq_state[2];
  };

/* GPU Control */

struct rq_gpuctrl
  {
  char  rq_momnode[PBS_MAXHOSTNAME + 1];
  char  rq_gpuid[PBS_MAXGPUID + 1];
  int   rq_gpumode;
  int   rq_reset_perm;
  int   rq_reset_vol;
  };

/* RegisterDependentJob */

struct rq_register
  {
  char rq_owner[PBS_MAXUSER + 1];
  char rq_svr[PBS_MAXSERVERNAME + 1];
  char rq_parent[PBS_MAXSVRJOBID + 1];
  char rq_child[PBS_MAXCLTJOBID + 1]; /* need separate entry for */
  int rq_dependtype;   /* from server_name:port   */
  int rq_op;
  long rq_cost;
  };


/* Authenticate User */

struct rq_authen
  {
  unsigned int rq_port;
  char rq_cred[PBS_MAXCREDENTIAL_LEN];
  };

/* Copy/Delete Files (Server -> MOM Only) */

#define STDJOBFILE    1
#define JOBCKPFILE    2
#define STAGEFILE     3

#define STAGE_DIR_IN  0
#define STAGE_DIR_OUT 1
#define CKPT_DIR_IN  2
#define CKPT_DIR_OUT 3

struct rq_cpyfile
  {
  char   rq_jobid[PBS_MAXSVRJOBID + 1];  /* Not used for stage-in */
  char   rq_owner[PBS_MAXUSER + 1];   /* used in Copy only    */
  char    rq_user[PBS_MAXUSER + 1];   /* used in Copy & Delete */
  char    rq_group[PBS_MAXGRPN + 1];   /* used in Copy only     */
  int   rq_dir;     /* used in Copy & checkpoint restart delete   */
  tlist_head rq_pair;                        /* list of rqfpair,  used in Copy & Delete */
  };

struct rqfpair
  {
  list_link  fp_link;
  int   fp_flag; /* 1 for std[out|err] 2 for stageout */
  char  *fp_local; /* used in Copy & Delete */
  char  *fp_rmt; /* used in Copy only     */
  };

struct rq_returnfiles
  {
  char rq_jobid[PBS_MAXSVRJOBID + 1];
  int  rq_return_stdout;
  int  rq_return_stderr;
  };

/* Job Obituary (MOM -> Server Only) */

struct rq_jobobit
  {
  char       rq_jid[PBS_MAXSVRJOBID+1];
  int        rq_status;
  tlist_head rq_attr;   /* svrattrlist */
  };

/*
 * ok we now have all the individual request structures defined,
 * so here is the union ...
 */

struct batch_request
  {
  list_link rq_link; /* linkage of all requests   */
  int rq_type; /* type of request   */
  int rq_perm; /* access permissions for the user */
  int rq_fromsvr; /* true if request from another server */
  int rq_conn; /* socket connection to client/server */
  int rq_orgconn; /* original socket if relayed to MOM */
  int rq_extsz; /* size of "extension" data  */
  long rq_time; /* time batch request created  */
  char rq_user[PBS_MAXUSER+1];     /* user name request is from    */
  char rq_host[PBS_MAXHOSTNAME+1]; /* name of host sending request */
  int   rq_refcount;
  void *rq_extra; /* optional ptr to extra info  */
  int   rq_noreply; /* Set true if no reply is required */
  char *rq_extend; /* request "extension" data  */

  struct batch_reply  rq_reply;   /* the reply area for this request */

  union indep_request
    {

    struct rq_authen rq_authen;
    int   rq_connect;

    struct rq_queuejob rq_queuejob;

    struct rq_jobcred       rq_jobcred;

    struct rq_jobfile rq_jobfile;
    char          rq_rdytocommit[PBS_MAXSVRJOBID+1];
    char          rq_commit[PBS_MAXSVRJOBID+1];

    struct rq_manage rq_delete;

    struct rq_hold  rq_hold;
    char          rq_locate[PBS_MAXSVRJOBID+1];

    struct rq_manage rq_manager;

    struct rq_message rq_message;

    struct rq_manage rq_modify;

    struct rq_move  rq_move;

    struct rq_register rq_register;

    struct rq_manage rq_release;
    char          rq_rerun[PBS_MAXSVRJOBID+1];

    struct rq_rescq  rq_rescq;

    struct rq_runjob        rq_run;
    tlist_head         rq_select; /* svrattrlist */
    int   rq_shutdown;

    struct rq_signal rq_signal;

    struct rq_status        rq_status;

    struct rq_track  rq_track;

    struct rq_gpuctrl  rq_gpuctrl;

    struct rq_cpyfile rq_cpyfile;

    struct rq_returnfiles rq_returnfiles;

    struct rq_jobobit rq_jobobit;
    } rq_ind;
  };


extern struct batch_request *alloc_br (int type);
extern void  reply_ack (struct batch_request *);
extern void  req_reject (int code, int aux, struct batch_request *, char *, char *);
extern void  reply_badattr (int code, int aux, svrattrl *, struct batch_request *);
extern void  reply_text (struct batch_request *, int code, char *text);
extern int   reply_send (struct batch_request *);
extern int   reply_jobid (struct batch_request *, char *, int);
extern void  reply_free (struct batch_reply *);
extern int   authenticate_user (struct batch_request *, struct credential *, char **);
extern void  dispatch_request (int, struct batch_request *);
extern void  free_br (struct batch_request *);
extern int   isode_request_read (int, struct batch_request *);
extern void  req_stat_job (struct batch_request *);

#ifndef PBS_MOM
extern void  req_authenuser (struct batch_request *req);
extern int   req_altauthenuser (struct batch_request *req);
extern void  req_connect (struct batch_request *req);
extern void  req_locatejob (struct batch_request *req);
extern void  req_manager (struct batch_request *req);
extern void  req_movejob (struct batch_request *req);
extern void  req_register (struct batch_request *req);
extern void  req_registerarray (struct batch_request *req);
extern void  req_releasearray (struct batch_request *req);
extern void  req_releasejob (struct batch_request *req);
extern void  req_rescq (struct batch_request *req);
extern void  req_rerunjob (struct batch_request *req);
extern void  req_runjob (struct batch_request *req);
extern void  req_selectjobs (struct batch_request *req);
extern void  req_stat_que (struct batch_request *req);
extern void  req_stat_svr (struct batch_request *req);
/* DIAGTODO: declr req_stat_diag() */
extern void  req_trackjob (struct batch_request *req);
extern void  req_gpuctrl (struct batch_request *req);
#else
extern void  req_cpyfile (struct batch_request *req);
extern void  req_delfile (struct batch_request *req);
extern void req_returnfiles (struct batch_request *req);
#endif

#ifdef SERVER_LIMITS_H
extern int   relay_to_mom (pbs_net_t, struct batch_request *, void (*)());
#endif  /* SERVER_LIMITS_H */

/* PBS Batch Request Decode/Encode routines */

/* udp decode and encode routines */
extern int decode_DIS_Authen (int socket, struct batch_request *);
extern int decode_DIS_AltAuthen (int socket, struct batch_request *);
extern int decode_DIS_CopyFiles (int socket, struct batch_request *);
extern int decode_DIS_JobCred (int socket, struct batch_request *);
extern int decode_DIS_JobFile (int socket, struct batch_request *);
extern int decode_DIS_JobObit (int socket, struct batch_request *);
extern int decode_DIS_Manage (int socket, struct batch_request *);
extern int decode_DIS_MoveJob (int socket, struct batch_request *);
extern int decode_DIS_MessageJob (int socket, struct batch_request *);
extern int decode_DIS_QueueJob (int socket, struct batch_request *);
extern int decode_DIS_Register (int socket, struct batch_request *);
extern int decode_DIS_ReturnFiles (int socket, struct batch_request *);
extern int decode_DIS_ReqExtend (int socket, struct batch_request *);
extern int decode_DIS_ReqHdr (int socket, struct batch_request *, int *tp, int *pv);
extern int decode_DIS_Rescl (int socket, struct batch_request *);
extern int decode_DIS_Rescq (int socket, struct batch_request *);
extern int decode_DIS_RunJob (int socket, struct batch_request *);
extern int decode_DIS_ShutDown (int socket, struct batch_request *);
extern int decode_DIS_SignalJob (int socket, struct batch_request *);
extern int decode_DIS_Status (int socket, struct batch_request *);
extern int decode_DIS_TrackJob (int socket, struct batch_request *);
extern int decode_DIS_replySvr (int socket, struct batch_reply *);
extern int decode_DIS_svrattrl (int socket, tlist_head *);
extern int decode_DIS_GpuCtrl (int socket, struct batch_request *);

extern int encode_DIS_CopyFiles (int socket, struct batch_request *);
extern int encode_DIS_JobObit (int socket, struct batch_request *);
extern int encode_DIS_Register (int socket, struct batch_request *);
extern int encode_DIS_ReturnFiles (int socket, struct batch_request *);
extern int encode_DIS_TrackJob (int socket, struct batch_request *);
extern int encode_DIS_reply (int socket, struct batch_reply *);
extern int encode_DIS_svrattrl (int socket, svrattrl *);

extern int dis_request_read (int socket, struct batch_request *);
extern int dis_reply_read (int socket, struct batch_reply *);
#endif /* BATCH_REQUEST_H */
