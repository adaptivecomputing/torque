/* $Id$ */
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

#ifndef GBLXVARS_H_
#define GBLXVARS_H_

#include <signal.h>
#include <time.h>

extern Resources *schd_RsrcsList; /* Execution host resource list */

/* Queue lists */
extern QueueList *schd_SubmitQueue; /* Job submission queue. */
extern QueueList *schd_BatchQueues; /* Job execution queues. */
extern QueueList *schd_DedQueues; /* Dedicated time queues. */

/*
 * The list of jobs on the system.  Jobs will be removed from this
 * list and placed onto the per-queue lists as they are initialized.
 * Anything remaining on this list belongs to a queue in which the
 * scheduler is not interested.
 */

extern Job *schd_AllJobs;

extern Alloc_Group schd_GroupTable[];

/* True if this is the first run since reconfiguration. */
extern int schd_FirstRun;

/* Scheduler Configuration Parameters */
extern int schd_TEST_ONLY;
extern int schd_AVOID_FRAGS;
extern int schd_SORT_BY_PAST_USAGE;
extern time_t schd_ENFORCE_PRIME_TIME;
extern int schd_NONPRIME_DRAIN_SYS;
extern time_t schd_NP_DRAIN_BACKTIME;
extern time_t schd_NP_DRAIN_IDLETIME;
extern time_t schd_PRIME_TIME_START;
extern time_t schd_PRIME_TIME_END;
extern time_t schd_PT_SMALL_NODE_LIMIT;
extern time_t schd_PT_SMALL_WALLT_LIMIT;
extern time_t schd_PT_WALLT_LIMIT;
extern time_t schd_SMALL_JOB_MAX;
extern time_t schd_WALLT_LARGE_LIMIT;
extern time_t schd_WALLT_SMALL_LIMIT;
extern int schd_TARGET_LOAD_PCT;
extern int schd_TARGET_LOAD_MINUS;
extern int schd_TARGET_LOAD_PLUS;
extern int schd_HIGH_SYSTIME;
extern int schd_MAX_JOBS;
extern int schd_MIN_JOBS;
extern int schd_MAX_DEDICATED_JOBS;
extern int schd_MAX_USER_RUN_JOBS;
extern int schd_USAGE_WEIGHT;
extern int schd_NODES_WEIGHT;
extern int schd_TIMEQ_WEIGHT;
extern int schd_MAX_NCPUS;

extern char *schd_SERVER_HOST;
extern char *schd_SCHED_HOST;
extern time_t schd_ENFORCE_ALLOCATION;
extern time_t schd_ENFORCE_DEDTIME;
extern int schd_DEDTIME_CACHE_SECS;
extern char *schd_SCHED_ACCT_DIR;
extern char *schd_DEDTIME_COMMAND;
extern char *schd_SYSTEM_NAME;
extern char *schd_JOB_DUMPFILE;
extern char *schd_SpecialQueue;
extern char *schd_ChallengeQueue;
extern char *schd_BackgroundQueue;

#define SCHD_RESTART_NONE 0
#define SCHD_RESTART_RESUBMIT 1
#define SCHD_RESTART_RERUN 2
extern int schd_SCHED_RESTART_ACTION;

/*
 * Pretend this machine is this many times as large as the real numbers
 * returned by resmom indicate.  I.e. to treat a 4-node box as a 64, set
 * FAKE_MACHINE_MULT to '16'.
 */
extern int schd_FAKE_MACH_MULT;

extern int schd_NeedToGetAllocInfo;
extern int schd_NeedToGetYTDInfo;
extern int schd_NumAllocation;
extern time_t schd_TimeNow;
extern time_t schd_TimeLast;
extern time_t schd_TimeChk;

extern struct tm schd_TmNow;
extern char *schd_CfgFilename;
extern char *schd_AllocFilename;
extern char *schd_CurrentFilename;

/* Global Variables added for User/Job Sorting - JJPJ */
extern int schd_NeedToGetDecayInfo;
extern time_t schd_MAX_QUEUED_TIME;
extern time_t schd_SMALL_QUEUED_TIME;
extern time_t schd_INTERACTIVE_LONG_WAIT;
extern double schd_DECAY_FACTOR;
extern double schd_OA_DECAY_FACTOR;

/*
 * Flags to prevent reconfiguration due to HUP while working.
 */
#define SCHD_SIGFLAGS_BUSY  (1)
#define SCHD_SIGFLAGS_RECONFIG  (1 << 1)
extern volatile sig_atomic_t schd_sigflags;

extern char schd_ThisHost[];

#ifndef ALLOC_1ST
#define ALLOC_1ST 64
#endif /* !ALLOC_1ST */
#ifndef ALLOC_INC
#define ALLOC_INC 32
#endif /* !ALLOC_INC */

#endif /* ! GBLXVARS_H_ */
