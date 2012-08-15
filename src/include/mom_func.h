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
#ifndef _MOM_FUNC_H
#define _MOM_FUNC_H

#include "libpbs.h"

#ifndef MOM_MACH
#include "mom_mach.h"
#endif /* MOM_MACH */

#include "pwd.h" /* passwd */
#include "../resmom/mom_main.h"

/* struct var_table = used to hold environment variables for the job */

struct var_table
  {
  char  *v_block;
  char **v_envp;
  int    v_bsize;
  int    v_ensize;
  int    v_used;
  };

/* struct sig_tbl = used to hold map of local signal names to values */

struct sig_tbl
  {
  char *sig_name;
  int   sig_val;
  };

#define NUM_LCL_ENV_VAR  10
#define MOM_OBIT_RETRY   -5

/* used by mom_main.c and requests.c for $usecp */

struct cphosts
  {
  char *cph_hosts;
  char *cph_from;
  char *cph_to;
  };

struct radix_buf
  {
  int current_string_len;
  int max_string_len;
  char *host_list;
  char *port_list;
  int current_port_str_len;
  int max_port_str_len;
  int count;
  };				  

extern int cphosts_num;

/* holds the varattrs */

struct varattr
  {
  list_link   va_link;
  int         va_ttl;
  char       *va_cmd;
  char       *va_value;
  time_t      va_lasttime;
  };

#define JOB_COOKIE_SIZE 33
enum momcomm_type { COMPOSE_REPLY, KILLJOB_REPLY, SPAWN_TASK_REPLY, OBIT_TASK_REPLY };

typedef struct resend_momcomm
  {
  enum momcomm_type  mc_type;
  void              *mc_struct;
  int                resend_attempts;
  } resend_momcomm;

typedef struct im_compose_info
  {
  hnodent     np;
  char        jobid[PBS_MAXSVRJOBID+1];
  char        cookie[JOB_COOKIE_SIZE];
  int         command;
  tm_event_t  event;
  tm_task_id  taskid;
  } im_compose_info;

typedef struct spawn_task_info
  {
  im_compose_info *ici;
  tm_task_id       ti_task;
  } spawn_task_info;

typedef struct killjob_reply_info
  {
  im_compose_info *ici;
  unsigned long    cputime;
  unsigned long    mem;
  unsigned long    vmem;
  int              node_id; /* -1 unless using job_radix */
  } killjob_reply_info;

typedef struct obit_task_info
  {
  im_compose_info *ici;
  int              ti_exitstat;
  } obit_task_info;

int add_to_resend_things(resend_momcomm *mc);
im_compose_info *create_compose_reply_info(char *, char *, hnodent *, int command, tm_event_t, tm_task_id);
int open_tcp_stream_to_sisters(job *pjob, int com, tm_event_t parent_event, int mom_radix, hnodent *hosts, struct radix_buf **sister_list, tlist_head *phead, int flag);

/* public funtions within MOM */

#ifdef _CRAY
#define WJSIGNAL 46

extern void  post_suspend(job *, int);
extern void  post_resume(job *, int);

#endif /* _CRAY */

extern void  bld_env_variables(struct var_table *, char *, char *);
extern int   expand_path(job *,char *,int,char *);
extern pid_t fork_me(int sock);
extern int   get_la(double *);
extern void  init_abort_jobs(int);
extern int   init_groups(char *, int, int, int *);
int   kill_job(job *, int sig, const char *killer_id_name, char *why_killed_reason);
extern void  mom_deljob(job *);
extern void  mom_freenodes(job *);
extern void  scan_for_exiting();
extern void  scan_for_terminated();
extern void  scan_non_child_tasks(void);
extern int   set_job(job *, struct startjob_rtn *);
extern void  set_globid(job *, struct startjob_rtn *);
extern int   set_mach_vars(job *, struct var_table *);
extern char *set_shell(job *, struct passwd *);
extern int   start_exec(job *);
extern int   open_master(char **);
extern int   open_slave();
extern char *rcvttype(int);
extern int   rcvwinsize(int);
extern int   remtree(char *);
extern int   setwinsize(int);
extern void  set_termcc(int);
extern int   site_job_setup(job *);
extern int   site_mom_chkuser(job *);
extern int   site_mom_postchk(job *, int);
extern int   site_mom_prerst(job *);
extern int   reader(int, int);
extern int   writer(int, int);
extern int   conn_qsub(char *, long, char *);  /* NOTE:  should be moved out of here to job_func proto header */
extern int   run_pelog(int, char *, job *, int);
extern int   is_joined(job *);
extern void  check_busy(double);
extern void  state_to_server(int, int);
extern void  dep_main_loop_cycle(void);
extern int   message_job(job *, enum job_file, char *);
extern void  term_job(job *);

extern struct passwd *check_pwd(job *);
extern int   task_save(task *) ;
extern void  DIS_rpp_reset(void);
extern void  checkret(char **, long);
extern char *get_job_envvar(job *, char *);
extern int   mom_open_socket_to_jobs_server(job* pjob, const char *id, void *(*message_hander)(void *));
void         clear_servers();

/* defined in mach-dependant mom_mach.c */
extern int kill_task(struct task *, int, int);

/* Defines for pe_io_type, see run_pelog() */

#define PE_IO_TYPE_NULL -1
#define PE_IO_TYPE_ASIS 0
#define PE_IO_TYPE_STD  1

#define PE_PROLOG   1
#define PE_EPILOG   2
#define PE_PROLOGUSER   3
#define PE_EPILOGUSER   4
#define PE_PROLOGUSERJOB 5  /* per job prologue script */
#define PE_EPILOGUSERJOB 6 /* per job epilogue script */

#ifdef LIBPBS_H
extern int   open_std_file(job *, enum job_file, int, gid_t);
extern char *std_file_name(job *, enum job_file, int *);
#endif /* LIBPBS_H */

#ifdef BATCH_REQUEST_H
extern int   start_checkpoint(job *, int, struct batch_request *);
#endif /* BATCH_REQUEST_H */

#endif /* _MOM_FUNC_H */
