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

/* libpbs.h

 The header file for private information in the pbs command
 interface library

*/
#ifndef _STDLIB_H
#include <stdlib.h>
#endif /* _STDLIB_H */
#ifndef _STRING_H
#include <string.h>
#endif /* _STRING_H */
#ifndef _MEMORY_H
#include <memory.h>
#endif /* _MEMORY_H */
#ifdef ENABLE_PTHREADS
#include <pthread.h>
#endif

#include "pbs_ifl.h"
#include "list_link.h"
#include "pbs_error.h"

#ifndef LIBPBS_H
#define LIBPBS_H

#define PBS_BATCH_PROT_TYPE 2
#define PBS_BATCH_PROT_VER 2
/* #define PBS_REQUEST_MAGIC (56) */
/* #define PBS_REPLY_MAGIC   (57) */
#define SCRIPT_CHUNK_Z (4096)
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#ifndef EOF
#define EOF -1
#endif

#define MUNGE_SIZE 256 /* I do not know what the proper size of this should be. My 
                          testing with munge shows it creates a string of 128 bytes */

/* enums for standard job files (sync w/TJobFileType[]) */

enum job_file
  {
  JScript,
  StdIn,
  StdOut,
  StdErr,
  Checkpoint
  };

#define MH(type) (type *)malloc(sizeof(type))
#define M(var,type) if( (var = MH(type)) == (type *)NULL ) \
      { return PBSE_SYSTEM; }
#define STRLEN(s) ((s==(char *)NULL)?0:strlen(s))
#define Str2QB(s) ((s==(char *)NULL)?(struct qbuf *)NULL:str2qb(s,strlen(s),0))
#define QB2Str(q) ((q==(struct qbuf *)NULL)?(char *)NULL:qb2str(q))


extern char pbs_current_user[];
extern char pbs_current_group[];

/* set ENABLE_TRUSTED_AUTH to "TRUE" to bypass using pbs_iff for authentication */
#define ENABLE_TRUSTED_AUTH FALSE

#define NCONNECTS 10

struct connect_handle
  {
  int ch_inuse; /* 1 if in use, 0 otherwise  */
  int ch_socket; /* file descriptor for the open socket */
  void   *ch_stream;
  int ch_errno; /* last error on this connection */
  char *ch_errtxt; /* pointer to last server error text */
#ifdef ENABLE_PTHREADS
  pthread_mutex_t *ch_mutex;
#endif
  };

extern struct connect_handle connection[];

/* PBS Batch Reply Structure     */
/* structures that make up the reply union */

struct brp_select    /* reply to Select Job Request */
  {
  struct brp_select *brp_next;
  char     brp_jobid[PBS_MAXSVRJOBID+1];
  };

struct brp_status    /* reply to Status Job/Queue/Server Request */
  {
  list_link brp_stlink;
  int   brp_objtype;
  char   brp_objname[(PBS_MAXSVRJOBID > PBS_MAXDEST ? PBS_MAXSVRJOBID:PBS_MAXDEST)+1];
  tlist_head brp_attr;  /* head of svrattrlist */
  };

struct brp_cmdstat
  {

  struct brp_cmdstat * brp_stlink;
  int   brp_objtype;
  char   brp_objname[(PBS_MAXSVRJOBID > PBS_MAXDEST ? PBS_MAXSVRJOBID:PBS_MAXDEST)+1];

  struct attrl *brp_attrl;
  };

struct brp_rescq              /* reply to Resource Query Request */
  {
  int      brq_number; /* number of items in following arrays */
  int     *brq_avail;
  int     *brq_alloc;
  int     *brq_resvd;
  int     *brq_down;
  };

/*
 * the following is the basic Batch Reply structure
 */

#define BATCH_REPLY_CHOICE_NULL      1  /* no reply choice, just code */
#define BATCH_REPLY_CHOICE_Queue     2  /* Job ID, see brp_jid   */
#define BATCH_REPLY_CHOICE_RdytoCom  3  /* select, see brp_jid    */
#define BATCH_REPLY_CHOICE_Commit    4  /* commit, see brp_jid   */
#define BATCH_REPLY_CHOICE_Select    5  /* select, see brp_select */
#define BATCH_REPLY_CHOICE_Status    6  /* status, see brp_status */
#define BATCH_REPLY_CHOICE_Text      7  /* text,   see brp_txt   */
#define BATCH_REPLY_CHOICE_Locate    8  /* locate, see brp_locate */
#define BATCH_REPLY_CHOICE_RescQuery 9  /* Resource Query         */

struct batch_reply
  {
  int brp_code;
  int brp_auxcode;
  int brp_choice; /* the union discriminator */
  union
    {
    char   brp_jid[PBS_MAXSVRJOBID+1];

    struct brp_select *brp_select; /* select replies */
    tlist_head     brp_status; /* status (svr) replies */

    struct brp_cmdstat *brp_statc;  /* status (cmd) replies) */

    struct
      {
      size_t   brp_txtlen;
      char *brp_str;
      } brp_txt;  /* text and credential reply */

    char   brp_locate[PBS_MAXDEST+1];

    struct brp_rescq brp_rescq; /* query resource reply */
    } brp_un;
  };

/* This construct pulls the constant parts from the batch types definitions
 *
 * @note dis_request_read() is using the PBS_BATCH_CEILING for valid message testing
 * @see pbs_batchreqtype_db.h for more info
 */
#define PbsBatchReqType(id,text) id,
enum PBatchReqTypeEnum
  {
  #include "pbs_batchreqtype_db.h"
  PBS_BATCH_CEILING
  };
#undef PbsBatchReqType

/* DIAGTODO: define PBS_BATCH_StatusDiag */
#define PBS_BATCH_FileOpt_Default  0
#define PBS_BATCH_FileOpt_OFlg     1
#define PBS_BATCH_FileOpt_EFlg     2

#define PBS_credentialtype_none 0
const char *reqtype_to_txt(int);

void initialize_connections_table();

void
PBS_free_aopl (struct attropl * aoplp);


int
PBSD_commit (int connect, char *jobid);

int
PBSD_jcred (int connect, char *buf, int len);

int
PBSD_jscript (int connect, char *script_file, char *jobid);

int
PBSD_mgr_put (int connect, int func, int cmd, int objtype, char *objname, struct attropl *al, char *extend);

int
PBSD_manager  (int connect, int func, int cmd, int objtype, char *objname, struct attropl *al, char *extend);

int
PBSD_msg_put (int connect, char *jobid, int fileopt, char *msg, char *extend);

int
PBSD_rdytocmt (int connect, char *jobid);

int
PBSD_sig_put (int connect, char *jobid, char *signal, char *extend);

int
PBSD_async_sig_put (int connect, char *jobid, char *signal, char *extend);

int
PBSD_term_put (int connect, int manner, char *extend);

int
PBSD_jobfile (int connect, int req_type, char *path, char *jobid, enum job_file which);

int
PBSD_status_put (int c, int func, char *id, struct attrl *attrib, char *extend);

struct batch_reply *PBSD_rdrpy (int connect);

void PBSD_FreeReply (struct batch_reply *);

struct batch_status *PBSD_status (int c, int function, char *id, struct attrl *attrib, char *extend);

struct batch_status *PBSD_status_get (int c);

char * PBSD_queuejob (int c, char *j, char *d, struct attropl *a, char *ex);


extern int decode_DIS_JobId (int socket, char *jobid);
extern int decode_DIS_replyCmd (int socket, struct batch_reply *);

extern int encode_DIS_JobCred (int socket, int type, char *cred, int len);
extern int encode_DIS_JobFile (int socket, int, char *, int, char *, int);
extern int encode_DIS_JobId (int socket, char *);
extern int encode_DIS_Manage (int socket, int cmd, int objt, char *, struct attropl *);
extern int encode_DIS_MoveJob (int socket, char *jid, char *dest);
extern int encode_DIS_MessageJob (int socket, char *jid, int fopt, char *m);
extern int encode_DIS_QueueJob (int socket, char *jid, char *dest, struct attropl *);
extern int encode_DIS_ReqExtend (int socket, char *extend);
extern int encode_DIS_ReqHdr (int socket, int reqt, char *user);
extern int encode_DIS_Rescq (int socket, char **rlist, int num);
extern int encode_DIS_RunJob (int socket, char *jid, char *where, unsigned int resch);
extern int encode_DIS_ShutDown (int socket, int manner);
extern int encode_DIS_SignalJob (int socket, char *jid, char *sig);
extern int encode_DIS_Status (int socket, char *objid, struct attrl *);
extern int encode_DIS_attrl (int socket, struct attrl *);
extern int encode_DIS_attropl (int socket, struct attropl *);

extern int DIS_reply_read (int socket, struct batch_reply *preply);
#endif /* LIBPBS_H */


