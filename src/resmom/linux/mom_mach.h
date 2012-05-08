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
 * Machine-dependent definitions for the Machine Oriented Miniserver
 *
 * Target System: linux
 */



#ifndef MOM_MACH
#define MOM_MACH "linux"
#include "../../lib/Liblog/pbs_log.h"
#include "../../lib/Liblog/log_event.h"
#include "pbs_job.h" /* job */

#define SET_LIMIT_SET   1
#define SET_LIMIT_ALTER 0
#define PBS_CHECKPOINT_MIGRATE 0


#ifdef USEJOBCREATE
#ifndef JOBFAKE
#include <job.h>
#define JOB_FAIL      (jid_t)-1
#else
#include <stdint.h>
typedef uint64_t jid_t;
#define JOB_FAIL      (jid_t)-1

#endif /* JOBFAKE */
#endif /* USEJOBCREATE */


/* struct startjob_rtn = used to pass error/session/other info  */
/*    child back to parent   */

struct startjob_rtn
  {
  int   sj_code; /* error code */
  pid_t sj_session; /* session */
#ifdef USEJOBCREATE
  jid_t sj_jobid;
#else
  int   sj_jobid;
#endif
  int   sj_rsvid;
  };

extern int mom_set_limits(job *, int); /* Set job's limits */
extern int mom_do_poll(job *);  /* Should limits be polled? */
extern int mom_does_checkpoint();                   /* see if mom does checkpoint */
extern int mom_open_poll();  /* Initialize poll ability */
extern int mom_get_sample();  /* Sample kernel poll data */
extern int mom_over_limit(job *);  /* Is polled job over limit? */
extern int mom_set_use(job *);  /* Set resource_used list */
extern int mom_kill(int, int); /* Kill a session */
extern int mom_close_poll();  /* Terminate poll ability */
#ifdef PBS_MOM
extern int mach_checkpoint(struct task *, char *, int);
extern long mach_restart(struct task *, char *); /* Restart checkpointed job */
#endif
#ifdef USEMEMACCTD
extern long get_weighted_memory_size(pid_t pid); /* Return the weighted memory (RSS) size for a pid, in bytes */
#endif


typedef struct proc_stat
  {
  int           session; /* session id */
  char           state; /* one of RSDZTW: Running, Sleeping,
       Sleeping (uninterruptable), Zombie, Paging (W),
       Traced or stopped on signal */
  unsigned long   utime; /* utime this process */
  unsigned long   stime; /* stime this process */
  unsigned long   cutime; /* sum of children's utime */
  unsigned long   cstime; /* sum of children's stime */
  int           pid;  /* process id */
  int             ppid;         /* process id */
  int             pgrp;         /* process group */
  char           *name;  /* name of exec'd command */
  unsigned long long vsize; /* virtual memory size for proc */
  unsigned long long rss; /* resident set size */
  unsigned long   start_time; /* start time of this process */
  unsigned        flags; /* the flags of the process */
  unsigned        uid;  /* uid of the process owner */
  int             processor;    /* CPU number last executed on */
  } proc_stat_t;


typedef struct proc_map
  {
  unsigned long vm_start; /* start of vm for process */
  unsigned long vm_end;  /* end of vm for process */
  unsigned long vm_size; /* vm_end - vm_start */
  unsigned long vm_offset; /* offset into vm? */
  unsigned  inode;  /* inode of region */
  char  *dev;  /* device */
  } proc_map_t;


#endif /* MOM_MACH */
