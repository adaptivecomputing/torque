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


#include <limits.h>

#include "resmon.h"
#include "pbs_ifl.h"
#include "mom_func.h"


#define PBS_PROLOG_TIME             300
#define MAX_UPDATES_BEFORE_SENDING  20
#define DEFAULT_JOB_EXIT_WAIT_TIME  600
#define DEFAULT_SERVER_STAT_UPDATES 45
#define CHECK_POLL_TIME             45
#define MAX_JOIN_WAIT_TIME          600
#define RESEND_WAIT_TIME            300



extern bool             thread_unlink_calls;
extern int              ignwalltime;
extern int              ignmem;
extern int              igncput;
extern int              ignvmem;
extern int              spoolasfinalname;
extern int              maxupdatesbeforesending;
extern char            *apbasil_path;
extern char            *apbasil_protocol;
extern int              reject_job_submit;
extern int              attempttomakedir;
extern int              reduceprologchecks;
extern int              CheckPollTime;
extern double           cputfactor;
extern float            ideal_load_val;
extern float            max_load_val;
extern char            *auto_ideal_load;
extern char            *auto_max_load;
extern int              exec_with_exec;
extern int              ServerStatUpdateInterval;
extern char            *AllocParCmd;
extern char             PBSNodeCheckPath[];
extern int              PBSNodeCheckProlog;
extern int              PBSNodeCheckEpilog;
extern int              PBSNodeCheckInterval;
extern int              job_oom_score_adjust;  /* no oom score adjust by default */
extern int              mom_oom_immunize;  /* make pbs_mom processes immune? no by default */
extern long             log_file_max_size;
extern long             log_file_roll_depth;
extern int              LOGKEEPDAYS; /* days each log file should be kept before deleting */
extern int              EXTPWDRETRY; /* # of times to try external pwd check */
extern char            *nodefile_suffix;    /* suffix to append to each host listed in job host file */
extern char            *submithost_suffix;  /* suffix to append to submithost for interactive jobs */
extern char             mom_host[PBS_MAXHOSTNAME + 1];
extern int              hostname_specified;
extern int              MOMConfigRReconfig;
extern char            *TNoSpoolDirList[];
extern int              is_reporter_mom;
extern int              is_login_node;
extern int              job_exit_wait_time;
extern char             jobstarter_exe_name[];
extern int              jobstarter_set;
extern int              jobstarter_privileged;
extern char            *server_alias;
extern char            *TRemChkptDirList[TMAX_RCDCOUNT];
extern char             tmpdir_basename[MAXPATHLEN];  /* for $TMPDIR */
extern char             rcp_path[MAXPATHLEN];
extern char             rcp_args[MAXPATHLEN];
extern char             xauth_path[MAXPATHLEN];
extern int              mask_num;
extern int              mask_max;
extern char           **maskclient; /* wildcard connections */
extern char             MOMConfigVersion[64];
extern int              MOMConfigDownOnError;
extern int              MOMConfigRestart;
extern double           wallfactor;
extern std::vector<cphosts> pcphosts;
extern long             pe_alarm_time;
extern char             DEFAULT_UMASK[1024];
extern char             PRE_EXEC[1024];
extern int              src_login_batch;
extern int              src_login_interactive;
extern long             TJobStartBlockTime; /* seconds to wait for job to launch before backgrounding */
extern char             config_file[_POSIX_PATH_MAX];
extern int              config_file_specified;
extern struct config   *config_array;
extern char             extra_parm[];
extern char             no_parm[];
extern int              rm_errno;
extern char             PBSNodeMsgBuf[1024];
extern int              MOMConfigUseSMT; /* 0: off, 1: on */
extern int              memory_pressure_threshold;
extern short            memory_pressure_duration;
extern int              max_join_job_wait_time;
extern int              resend_join_job_wait_time;
extern int              mom_hierarchy_retry_time;
extern int              MOMJobDirStickySet;
extern unsigned long    max_memory;
extern unsigned long    max_swap;

struct specials
  {
  const char     *name;
  u_long(*handler)(const char *);
  }; 

extern struct specials  special[];
extern struct config    common_config[];

unsigned long setstatusupdatetime(const char *value);
unsigned long setcheckpolltime(const char *value);
unsigned long jobstartblocktime(const char *value);
unsigned long setloglevel(const char *value);
unsigned long setdownonerror(const char *value);
unsigned long setenablemomrestart(const char *value);
unsigned long setrcpcmd(const char *value);
unsigned long setjobdirectorysticky(const char *value);
unsigned long setcudavisibledevices(const char *value);

#ifdef PENABLE_LINUX26_CPUSETS
unsigned long setmempressthr(const char *);
unsigned long setmempressdur(const char *);
#endif
int read_config(char *file);
void reset_config_vars();



struct config_list
  {
  struct config       c;

  struct config_list *c_link;
  };
