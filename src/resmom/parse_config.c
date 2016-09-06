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


// including this file first is important to avoid bugs. This fixed a crash of the size of 
// the job struct changing. For real.
#include "pbs_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sstream>

#include "log.h"
#include "mom_config.h"
#include "dis.h"
#include "mom_func.h"
#include "u_tree.h"
#include "csv.h"

void encode_used(job *pjob, int perm, std::stringstream *list, tlist_head *phead);
void encode_flagged_attrs(job *pjob, int perm, std::stringstream *list, tlist_head *phead);

/* these are the global variables we set or don't set as a result of the config file.
 * They should be externed in mom_config.h */
bool             thread_unlink_calls = true;
/* by default, enforce these policies */
int              ignwalltime = 0; 
int              ignmem = 0;
int              igncput = 0;
int              ignvmem = 0; 
/* end policies */
bool             check_rur = true; /* on by default */
bool             force_file_overwrite = false;
int              spoolasfinalname = 0;
int              maxupdatesbeforesending = MAX_UPDATES_BEFORE_SENDING;
char            *apbasil_path     = NULL;
char            *apbasil_protocol = NULL;
int              reject_job_submit = 0;
int              attempttomakedir = 0;
int              reduceprologchecks;
int              CheckPollTime            = CHECK_POLL_TIME;
double           cputfactor = 1.00;
float            ideal_load_val = -1.0;
int              exec_with_exec = 0;
int              ServerStatUpdateInterval = DEFAULT_SERVER_STAT_UPDATES;
float            max_load_val = -1.0;
char            *auto_ideal_load = NULL;
char            *auto_max_load   = NULL;
char            *AllocParCmd = NULL;  /* (alloc) */
char             PBSNodeCheckPath[MAXLINE];
int              PBSNodeCheckProlog = 0;
int              PBSNodeCheckEpilog = 0;
int              PBSNodeCheckInterval = 1;
int              job_oom_score_adjust = 0;  /* no oom score adjust by default */
int              mom_oom_immunize = 1;  /* make pbs_mom processes immune? no by default */
long             log_file_max_size = 0;
long             log_file_roll_depth = 1;
int              LOGKEEPDAYS = 0; /* days each log file should be kept before deleting */
int              EXTPWDRETRY = 3; /* # of times to try external pwd check */
char            *nodefile_suffix = NULL;    /* suffix to append to each host listed in job host file */
char            *submithost_suffix = NULL;  /* suffix to append to submithost for interactive jobs */
char             mom_host[PBS_MAXHOSTNAME + 1];
int              hostname_specified = 0;
int              MOMConfigRReconfig        = 0;
char            *TNoSpoolDirList[TMAX_NSDCOUNT];
int              is_reporter_mom = FALSE;
int              is_login_node   = FALSE;
int              job_exit_wait_time = DEFAULT_JOB_EXIT_WAIT_TIME;
char             jobstarter_exe_name[MAXPATHLEN + 1];
int              jobstarter_set = 0;
int              jobstarter_privileged = 0;
char            *server_alias = NULL;
char            *TRemChkptDirList[TMAX_RCDCOUNT];
char             tmpdir_basename[MAXPATHLEN];  /* for $TMPDIR */
char             rcp_path[MAXPATHLEN];
char             rcp_args[MAXPATHLEN];
char             xauth_path[MAXPATHLEN];
int              mask_num = 0;
int              mask_max = 0;
char           **maskclient = NULL; /* wildcard connections */
char             MOMConfigVersion[64];
int              MOMConfigDownOnError      = 0;
int              MOMConfigRestart          = 0;
int              MOMCudaVisibleDevices     = 1;
double           wallfactor = 1.00;
std::vector<cphosts> pcphosts;
long             pe_alarm_time = PBS_PROLOG_TIME;
char             DEFAULT_UMASK[1024];
char             PRE_EXEC[1024];
int              src_login_batch = TRUE;
int              src_login_interactive = TRUE;
long             TJobStartBlockTime = 5; /* seconds to wait for job to launch before backgrounding */
char             config_file[_POSIX_PATH_MAX] = "config";
int              config_file_specified = 0;
struct config   *config_array = NULL;
char             extra_parm[] = "extra parameter(s)";
char             no_parm[]    = "required parameter not found";
int              rm_errno;
#ifdef PENABLE_LINUX26_CPUSETS
int              MOMConfigUseSMT = 1; /* 0: off, 1: on */
int              memory_pressure_threshold = 0; /* 0: off, >0: check and kill */
short            memory_pressure_duration  = 0; /* 0: off, >0: check and kill */
#endif
int              max_join_job_wait_time = MAX_JOIN_WAIT_TIME;
int              resend_join_job_wait_time = RESEND_WAIT_TIME;
int              mom_hierarchy_retry_time = NODE_COMM_RETRY_TIME;




/* other globals */
static int          nconfig;
struct config_list *config_list = NULL;
char                varattr_delimiter[] = ";";

extern char         PBSNodeMsgBuf[MAXLINE];
extern long         MaxConnectTimeout;
extern char        *path_log;
extern tlist_head   mom_varattrs; /* variable attributes */
extern AvlTree      okclients;  /* accept connections from */
extern int          mom_server_count;
extern time_t       time_now;
extern int          internal_state;
extern int          MOMJobDirStickySet;

#ifdef NUMA_SUPPORT
extern int            numa_index;
#endif

/* external functions */
int      mom_server_add(const char *name);
ssize_t read_ac_socket(int fd, void *buf, ssize_t count);
extern void free_pwnam(struct passwd *pwdp, char *buf);
struct passwd *getpwnam_ext(char **user_buffer, char *user_name);

/* NOTE:  must adjust RM_NPARM in resmom.h to be larger than number of parameters
          specified below */
unsigned long setforceoverwrite(const char*);
unsigned long setrur(const char *);
unsigned long setxauthpath(const char *);
unsigned long setrcpcmd(const char *);
unsigned long setpbsclient(const char *);
unsigned long configversion(const char *);
unsigned long cputmult(const char *);
unsigned long setallocparcmd(const char *);
unsigned long setidealload(const char *);
unsigned long setignwalltime(const char *);
unsigned long setignmem(const char *);
unsigned long setigncput(const char *);
unsigned long setignvmem(const char *);
unsigned long setlogevent(const char *);
unsigned long setloglevel(const char *);
unsigned long setumask(const char *);
unsigned long setpreexec(const char *);
unsigned long setmaxload(const char *);
unsigned long setenablemomrestart(const char *);
unsigned long prologalarm(const char *);
unsigned long restricted(const char *);
unsigned long jobstartblocktime(const char *);
unsigned long usecp(const char *);
unsigned long wallmult(const char *);
unsigned long setpbsserver(const char *);
unsigned long setnodecheckscript(const char *);
unsigned long setnodecheckinterval(const char *);
unsigned long settimeout(const char *);
unsigned long mom_checkpoint_set_checkpoint_interval(const char *);
unsigned long mom_checkpoint_set_checkpoint_script(const char *);
unsigned long mom_checkpoint_set_restart_script(const char *);
unsigned long mom_checkpoint_set_checkpoint_run_exe_name(const char *);
unsigned long setdownonerror(const char *);
unsigned long setstatusupdatetime(const char *);
unsigned long setcheckpolltime(const char *);
unsigned long settmpdir(const char *);
unsigned long setlogfilemaxsize(const char *);
unsigned long setlogfilerolldepth(const char *);
unsigned long setlogfilesuffix(const char *);
unsigned long setlogdirectory(const char *);
unsigned long setlogkeepdays(const char *);
unsigned long setvarattr(const char *);
unsigned long setautoidealload(const char *);
unsigned long setautomaxload(const char *);
unsigned long setnodefilesuffix(const char *);
unsigned long setnospooldirlist(const char *);
unsigned long setmomhost(const char *);
unsigned long setrreconfig(const char *);
unsigned long setsourceloginbatch(const char *);
unsigned long setsourcelogininteractive(const char *);
unsigned long setspoolasfinalname(const char *);
unsigned long setattempttomakedir(const char *);
unsigned long setremchkptdirlist(const char *);
unsigned long setmaxconnecttimeout(const char *);
unsigned long aliasservername(const char *);
unsigned long jobstarter(const char *value);
unsigned long setjobstarterprivileged(const char *);
#ifdef PENABLE_LINUX26_CPUSETS
unsigned long setusesmt(const char *);
unsigned long setmempressthr(const char *);
unsigned long setmempressdur(const char *);
#endif
unsigned long setreduceprologchecks(const char *);
unsigned long setextpwdretry(const char *);
unsigned long setexecwithexec(const char *);
unsigned long setmaxupdatesbeforesending(const char *);
unsigned long setthreadunlinkcalls(const char *);
unsigned long setapbasilpath(const char *);
unsigned long setapbasilprotocol(const char *);
unsigned long setreportermom(const char *);
unsigned long setloginnode(const char *);
unsigned long setrejectjobsubmission(const char *);
unsigned long setjoboomscoreadjust(const char *);
unsigned long setmomoomimmunize(const char *);
unsigned long setjobexitwaittime(const char *);
unsigned long setmaxjoinjobwaittime(const char *);
unsigned long setresendjoinjobwaittime(const char *);
unsigned long setmomhierarchyretrytime(const char *);
unsigned long setjobdirectorysticky(const char *);
unsigned long setcudavisibledevices(const char *);

struct specials special[] = {
  { "force_overwrite",     setforceoverwrite}, 
  { "alloc_par_cmd",       setallocparcmd },
  { "auto_ideal_load",     setautoidealload },
  { "auto_max_load",       setautomaxload },
  { "xauthpath",           setxauthpath },
  { "rcpcmd",              setrcpcmd },
  { "rcp_cmd",             setrcpcmd },
  { "pbsclient",           setpbsclient },
  { "configversion",       configversion },
  { "cputmult",            cputmult },
  { "ideal_load",          setidealload },
  { "ignwalltime",         setignwalltime },
  { "ignmem",              setignmem },
  { "igncput",             setigncput },
  { "ignvmem",             setignvmem },
  { "logevent",            setlogevent },
  { "loglevel",            setloglevel },
  { "max_load",            setmaxload },
  { "enablemomrestart",    setenablemomrestart },
  { "prologalarm",         prologalarm },
  { "restricted",          restricted },
  { "jobstartblocktime",   jobstartblocktime },
  { "usecp",               usecp },
  { "wallmult",            wallmult },
  { "clienthost",          setpbsserver },  /* deprecated - use pbsserver */
  { "pbsserver",           setpbsserver },
  { "node_check_script",   setnodecheckscript },
  { "node_check_interval", setnodecheckinterval },
  { "timeout",             settimeout },
  { "checkpoint_interval", mom_checkpoint_set_checkpoint_interval },
  { "checkpoint_script",   mom_checkpoint_set_checkpoint_script },
  { "restart_script",      mom_checkpoint_set_restart_script },
  { "checkpoint_run_exe",  mom_checkpoint_set_checkpoint_run_exe_name },
  { "down_on_error",       setdownonerror },
  { "status_update_time",  setstatusupdatetime },
  { "check_poll_time",     setcheckpolltime },
  { "tmpdir",              settmpdir },
  { "log_directory",       setlogdirectory },
  { "log_file_max_size",   setlogfilemaxsize },
  { "log_file_roll_depth", setlogfilerolldepth },
  { "log_file_suffix",     setlogfilesuffix },
  { "log_keep_days",       setlogkeepdays },
  { "varattr",             setvarattr },
  { "nodefile_suffix",     setnodefilesuffix },
  { "nospool_dir_list",    setnospooldirlist },
  { "mom_host",            setmomhost },
  { "remote_reconfig",     setrreconfig },
  { "job_output_file_umask", setumask },
  { "preexec",             setpreexec },
  { "source_login_batch",  setsourceloginbatch },
  { "source_login_interactive", setsourcelogininteractive },
  { "spool_as_final_name", setspoolasfinalname },
  { "remote_checkpoint_dirs", setremchkptdirlist },
  { "max_conn_timeout_micro_sec",   setmaxconnecttimeout },
  { "alias_server_name", aliasservername },
  { "job_starter", jobstarter},
  { "job_starter_run_privileged", setjobstarterprivileged},
#ifdef PENABLE_LINUX26_CPUSETS
  { "use_smt",                      setusesmt      },
  { "memory_pressure_threshold",    setmempressthr },
  { "memory_pressure_duration",     setmempressdur },
#endif
  { "reduce_prolog_checks",         setreduceprologchecks},
  { "thread_unlink_calls", setthreadunlinkcalls },
  { "attempt_to_make_dir", setattempttomakedir },
  { "ext_pwd_retry",       setextpwdretry },
  { "exec_with_exec",      setexecwithexec },
  { "max_updates_before_sending", setmaxupdatesbeforesending },
  { "apbasil_path",        setapbasilpath },
  { "reject_job_submission", setrejectjobsubmission },
  { "apbasil_protocol",      setapbasilprotocol },
  { "reporter_mom",          setreportermom },
  { "login_node",            setloginnode },
  { "job_oom_score_adjust",  setjoboomscoreadjust },
  { "mom_oom_immunize",      setmomoomimmunize },
  { "job_exit_wait_time",    setjobexitwaittime },
  { "max_join_job_wait_time", setmaxjoinjobwaittime},
  { "resend_join_job_wait_time", setresendjoinjobwaittime},
  { "mom_hierarchy_retry_time",  setmomhierarchyretrytime},
  { "jobdirectory_sticky", setjobdirectorysticky},
  { "cuda_visible_devices", setcudavisibledevices},
  { "cray_check_rur",       setrur },
  { NULL,                  NULL }
  };


const char *arch(struct rm_attribute *);
const char *opsys(struct rm_attribute *);
const char *requname(struct rm_attribute *);
const char *validuser(struct rm_attribute *);
const char *reqmsg(struct rm_attribute *);
const char         *reqgres(struct rm_attribute *);
const char *reqstate(struct rm_attribute *);
const char *getjoblist(struct rm_attribute *);
const char *reqvarattr(struct rm_attribute *);


struct config common_config[] =
  {
  { "arch",      {arch} },             /* machine architecture           */
  { "opsys",     {opsys} },            /* operating system               */
  { "uname",     {requname} },         /* user name     ???              */
  { "validuser", {validuser} },        /* valid user    ???              */
  { "message",   {reqmsg} },           /* message       ???              */
  { "gres",      {reqgres} },          /* generic resource (licenses...) */
  { "state",     {reqstate} },         /* state of pbs_mom               */
  { "jobs",      {getjoblist} },       /* job list this pbs_mom          */
  { "varattr",   {reqvarattr} },       /* ???                            */
  { NULL,        {NULL} }
  };



/*
 * Parse a boolean config option value.
 * Return 1 (true), 0 (false), -1 (error).
 * Accepts: "true", "yes", "on", "1", "false", "no", "off", "0"
 */

int setbool(

  const char *value) /* I */

  {
  int enable = -1;

  if (value != NULL)
    {

    switch (value[0])
      {

      case 't':
      case 'T':
      case 'y':
      case 'Y':
      case '1':
        enable = 1;
        break;

      case 'f':
      case 'F':
      case 'n':
      case 'N':
      case '0':
        enable = 0;
        break;

      case 'o':
      case 'O':
        if ((strcasecmp(value,"on") == 0))
          enable = 1;
        else if ((strcasecmp(value,"off") == 0))
          enable = 0;
        break;

      }

    }

  return(enable);
  }



char *tokcpy(

  char *str,
  char *tok)

  {
  for (;*str;str++, tok++)
    {
    if (!isalnum(*str) && *str != ':' && *str != '_')
      break;

    *tok = *str;
    }

  *tok = '\0';

  return(str);
  }  /* END tokcpy() */

unsigned long setforceoverwrite(
  
  const char *value)

  {
  int enable;

  if ((enable = setbool(value)) != -1)
    {
    force_file_overwrite = enable;
    return(1);
    }

  return(0); /* error */
  }
unsigned long setrur(

  const char *value)
  
  {
  int enable;

  if ((enable = setbool(value)) != -1)
    {
    check_rur = enable;
    return(1);
    }
    
  return(0); /* error */
  }/* end setrur() */


unsigned long setidealload(

  const char *value)

  {
  float  val;

  val = atof(value);

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (val < 0.0)
    {
    return(0); /* error */
    }

  ideal_load_val = val;

  if (max_load_val < 0.0)
    max_load_val = val; /* set a default */

  return(1);
  }  /* END setidealload() */




unsigned long setignwalltime(

  const char *value)  /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((enable = setbool(value)) != -1)
    ignwalltime = enable;

  return(1);
  }  /* END setignwalltime() */



unsigned long setignmem(

  const char *value)  /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((enable = setbool(value)) != -1)
    ignmem = enable;

  return(1);
  } /* END setignmem() */



unsigned long setigncput(

  const char *value) /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((enable = setbool(value)) != -1)
    igncput = enable;

  return(1);
  }


unsigned long setignvmem(

  const char *value)  /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((enable = setbool(value)) != -1)
    ignvmem = enable;

  return(1);
  }  /* END setignvmem() */


unsigned long setautoidealload(

  const char *value)

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  auto_ideal_load = strdup(value);

  return(1);
  }  /* END setautoidealload() */





unsigned long setallocparcmd(

  const char *value)  /* I */

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  AllocParCmd = strdup(value);

  return(1);
  }  /* END setallocparcmd() */





unsigned long setautomaxload(

  const char *value)

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  auto_max_load = strdup(value);

  return(1);
  }  /* END setautomaxload() */





unsigned long setmaxconnecttimeout(

  const char *value)  /* I */

  {
  MaxConnectTimeout = strtol(value, NULL, 10);

  if (MaxConnectTimeout < 0)
    {
    MaxConnectTimeout = 10000;

    return(0);
    }

  return(1);
  }



unsigned long setreduceprologchecks(

  const char *value)

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);
  
  if (!strncasecmp(value,"t",1) ||
      (value[0] == '1') ||
      (!strcasecmp(value,"on")))
    reduceprologchecks = TRUE;
  else
    reduceprologchecks = FALSE;

  return(1);
  } /* END setreduceprologchecks() */



unsigned long setnodecheckscript(

  const char *value)

  {
  struct stat sbuf;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((stat(value, &sbuf) == -1) ||
      !(sbuf.st_mode & S_IXUSR))
    {
    /* FAILURE */

    /* file does not exist or is not executable */

    return(0);
    }

  snprintf(PBSNodeCheckPath, sizeof(PBSNodeCheckPath), "%s", value);

  /* SUCCESS */

  return(1);
  }  /* END setnodecheckscript() */





unsigned long setnodecheckinterval(

  const char *value)

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  PBSNodeCheckInterval = (int)strtol(value, NULL, 10);

  if (strstr(value, "jobstart"))
    PBSNodeCheckProlog = 1;

  if (strstr(value, "jobend"))
    PBSNodeCheckEpilog = 1;

  return(1);
  }  /* END setnodecheckinterval() */



unsigned long settimeout(

  const char *value)

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  DIS_tcp_settimeout(strtol(value, NULL, 10));

  return(1);
  }  /* END settimeout() */





unsigned long setmaxload(

  const char *value)  /* I */

  {
  float  val;

  val = atof(value);

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (val < 0.0)
    {
    return(0); /* error */
    }

  max_load_val = val;

  if (ideal_load_val < 0.0)
    ideal_load_val = val;

  return(1);
  }  /* END max_load() */





unsigned long setlogfilemaxsize(

  const char *value)  /* I */

  {
  log_file_max_size = strtol(value, NULL, 10);

  if (log_file_max_size < 0)
    {
    log_file_max_size = 0;

    return(0);
    }

  return(1);
  }




unsigned long setlogfilerolldepth(

  const char *value)  /* I */

  {
  log_file_roll_depth = strtol(value, NULL, 10);

  if (log_file_roll_depth < 1)
    {
    log_file_roll_depth = 1;

    return(0);
    }

  return(1);
  }



unsigned long setlogdirectory(

  const char *value)  /* I */

  {
  path_log = strdup(value);

  return(1);
  }




unsigned long setlogfilesuffix(

  const char *value)  /* I */

  {
  log_init(value, NULL);

  return(1);
  } /* END setlogfilesuffix() */



unsigned long setlogkeepdays(

  const char *value)  /* I */

  {
  int i;

  i = (int)atoi(value);

  if (i < 0)
    {
    return(0);  /* error */
    }

  LOGKEEPDAYS = i;

  return(1);
  } /* END setlogkeepdays() */


unsigned long setextpwdretry(

  const char *value)  /* I */

  {
  int i;

  i = (int)atoi(value);

  if (i < 0)
    {
    return(0);  /* error */
    }

  EXTPWDRETRY = i;

  return(1);
  } /* END setextpwdretry() */



u_long setvarattr(

  const char *value)  /* I */

  {
  struct varattr *pva;
  const char    *ptr;

  pva = (struct varattr *)calloc(1, sizeof(struct varattr));

  if (pva == NULL)
    {
    /* FAILURE */

    log_err(errno, __func__, "no memory");

    return(0);
    }

  CLEAR_LINK(pva->va_link);

  /* FORMAT:  <TTL> <PATH> */
  /* extract TTL */

  ptr = value;

  pva->va_ttl = strtol(ptr, NULL, 10);

  /* step forward to end of TTL */

  while ((!isspace(*ptr)) &&
         (*ptr != '\0'))
    ptr++;

  if (*ptr == '\0')
    {
    free(pva);

    return(0);
    }

  /* skip white space */

  while (isspace(*ptr))
    ptr++;

  if (*ptr == '\0')
    {
    free(pva);

    return(0);
    }

  /* preserve command and args */

  pva->va_cmd = strdup(ptr);

  append_link(&mom_varattrs, &pva->va_link, pva);

  /* SUCCESS */

  return(1);
  }  /* END setvarattr() */




unsigned long setthreadunlinkcalls(

  const char *value) /* I */

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (!strncasecmp(value,"t",1) ||
      (value[0] == '1') ||
      (!strcasecmp(value,"on")))
    thread_unlink_calls = true;
  else
    thread_unlink_calls = false;

  return(1);
  } /* END setthreadunlinkcalls() */





unsigned long setnodefilesuffix(

  const char *value)  /* I */

  {
  char *ptr;
  char *cpy = strdup(value);
  
  if (cpy == NULL)
    return (-1);

  ptr = strtok(cpy, ",");

  nodefile_suffix = strdup(ptr);

  ptr = strtok(NULL, ",");

  if (ptr != NULL)
    submithost_suffix = strdup(ptr);

  /* SUCCESS */

  free(cpy);
  return(1);
  }  /* END setnodexfilesuffix() */




unsigned long setmomhost(

  const char *value)  /* I */

  {
  hostname_specified = 1;

  snprintf(mom_host, sizeof(mom_host), "%s", value);

  /* SUCCESS */

  return(1);
  }  /* END setmomhost() */


u_long setrreconfig(

  const char *value)  /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((enable = setbool(value)) != -1)
    MOMConfigRReconfig = enable;

  return(1);
  }  /* END setrreconfig() */


unsigned long setnospooldirlist(

  const char *value)  /* I */

  {
  char *TokPtr;
  char *ptr;

  int   index = 0;

  char  tmpLine[1024];

  ptr = strtok_r((char *)value, " \t\n:,", &TokPtr);

  while (ptr != NULL)
    {
    TNoSpoolDirList[index] = strdup(ptr);

    snprintf(tmpLine, sizeof(tmpLine), "added NoSpoolDir[%d] '%s'",
      index, ptr);

    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, tmpLine);

    index++;

    if (index >= TMAX_NSDCOUNT)
      break;

    ptr = strtok_r(NULL, " \t\n:,", &TokPtr);
    }  /* END while (ptr != NULL) */

  /* SUCCESS */

  return(1);
  }  /* END setnospooldirlist() */


unsigned long aliasservername(const char *value)
  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (value)
    {
    server_alias = (char *)calloc(1, strlen(value)+1);
    if (server_alias)
      {
      strcpy(server_alias, value);
      }
    }
  
  return(1);
  } /* END aliasservername() */




unsigned long setspoolasfinalname(

  const char *value)  /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((enable = setbool(value)) != -1)
    spoolasfinalname = enable;

  return(1);
  }  /* END setspoolasfinalname() */



unsigned long setapbasilpath(

  const char *value)

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (value != NULL)
    {
    if (value[0] == '/')
      apbasil_path = strdup(value);
    else
      {
      snprintf(log_buffer, sizeof(log_buffer),
        "Path must be an absolute path, but is %s", value);
      log_err(-1, __func__, log_buffer);
      }
    }

  return(1);
  } /* END setapbasilpath() */



unsigned long setapbasilprotocol(

  const char *value)

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (value != NULL)
    {
    /* only versions 1.0 to 1.4 are supported for now. update later */
    if ((strlen(value) != 3) ||
        (value[0] != '1') ||
        (value[1] != '.') ||
        ((value[2] != '0') &&
         (value[2] != '1') &&
         (value[2] != '2') &&
         (value[2] != '3') &&
         (value[2] != '4') &&
         (value[2] != '5') &&
         (value[2] != '6') &&
         (value[2] != '7')))
      {
      snprintf(log_buffer, sizeof(log_buffer), 
        "Value must be 1.[0-4] but is %s", value);
      log_err(-1, __func__, log_buffer);
      }
    else
      apbasil_protocol = strdup(value);
    }

  return(1);
  } /* END setapbasilprotocol() */




unsigned long setreportermom(

  const char *value)

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (value != NULL)
    {
    if ((value[0] == 't') ||
        (value[0] == 'T') ||
        (value[0] == 'y') ||
        (value[0] == 'Y'))
      is_reporter_mom = TRUE;
    else
      is_reporter_mom = FALSE;
    }

  return(1);
  } /* END setreportermom() */




unsigned long setloginnode(

  const char *value)

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (value != NULL)
    {
    if ((value[0] == 't') ||
        (value[0] == 'T') ||
        (value[0] == 'y') ||
        (value[0] == 'Y'))
      is_login_node = TRUE;
    else
      is_login_node = FALSE;
    }

  return(1);
  } /* END setloginnode() */




unsigned long setjobexitwaittime(
    
  const char *value)

  {
  int tmp;
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (value != NULL)
    {
    tmp = strtol(value, NULL, 10);
    if (tmp != 0)
      job_exit_wait_time = tmp;
    }

  return(1);
  } /* END setjobexitwaittime() */




unsigned long setrejectjobsubmission(

  const char *value)

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (!strncasecmp(value,"t",1) ||
      (value[0] == '1') ||
      (!strcasecmp(value,"on")))
    reject_job_submit = TRUE;
  else
    reject_job_submit = FALSE;

  return(1);
  } /* END setrejectjobsubmission() */




unsigned long setattempttomakedir(

  const char *value)

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (!strncasecmp(value,"t",1) || (value[0] == '1') || !strcasecmp(value,"on") )
    attempttomakedir = TRUE;
  else
    attempttomakedir = 0;

  return(1);
  } /* END setattempttomakedir() */


/********************************************************
 *  jobstarter - set the name of the job starter program
 *  to the value given in the mom config file
 *
 *  Return: 1 success
 *          0 file named by value does not exist. fail
 *******************************************************/
unsigned long jobstarter(const char *value)  /* I */
  {
  struct stat sbuf;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((stat(value, &sbuf) == -1) ||
      !(sbuf.st_mode & S_IXUSR))
    {
    /* file does not exist or is not executable */

    return(0);  /* error */
    }

  snprintf(jobstarter_exe_name, sizeof(jobstarter_exe_name), "%s", value);

  jobstarter_set = 1;

  return(1);
  }  /* END jobstarter() */



/********************************************************
 *  setjobstarterprivileged - enable/disable the jobstarter
 *  to run with elevated privileges
 *
 *  Returns: 1
 *******************************************************/
unsigned long setjobstarterprivileged(

  const char *value)  /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((enable = setbool(value)) != -1)
    jobstarter_privileged = enable;

  return(1);
  }  /* END setjobstarter_privileged() */



unsigned long setremchkptdirlist(

  const char *value)  /* I */

  {
  char *TokPtr;
  char *ptr;

  int   index = 0;
  char  tmpLine[1024];

  while ((index < TMAX_RCDCOUNT) &&
         (TRemChkptDirList[index] != NULL))
    {
    index++;
    }

  if (index >= TMAX_RCDCOUNT)
    return (1);

  ptr = strtok_r((char *)value, " \t\n:,", &TokPtr);

  while (ptr != NULL)
    {
    TRemChkptDirList[index] = strdup(ptr);

    snprintf(tmpLine, sizeof(tmpLine), "added RemChkptDir[%d] '%s'",
      index, ptr);

    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, tmpLine);

    index++;

    if (index >= TMAX_RCDCOUNT)
      break;

    ptr = strtok_r(NULL, " \t\n:,", &TokPtr);
    }  /* END while (ptr != NULL) */

  /* SUCCESS */

  return (1);
  }  /* END setremchkptdirlist() */




u_long setjobdirectorysticky(

  const char *value)  /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((enable = setbool(value)) != -1)
    MOMJobDirStickySet = enable;

  return(1);
  }  /* END setjobdirectorysticky() */




u_long addclient(

  const char *name)  /* I */

  {
  struct addrinfo *addr_info;
  struct in_addr   saddr;
  u_long           ipaddr;

  /* FIXME: must be able to retry failed lookups later */

  if (getaddrinfo(name, NULL, NULL, &addr_info) != 0)
    {
    sprintf(log_buffer, "host %s not found", name);

    log_err(-1, __func__, log_buffer);

    return(0);
    }

  saddr = ((struct sockaddr_in *)addr_info->ai_addr)->sin_addr;
  freeaddrinfo(addr_info);

  ipaddr = ntohl(saddr.s_addr);

  okclients = AVL_insert(ipaddr, 0, NULL, okclients);
  
  return(ipaddr);
  }  /* END addclient() */





u_long setpbsclient(

  const char *value)  /* I */

  {
  u_long rc;

  if ((value == NULL) ||
      (value[0] == '\0'))
    {
    /* FAILURE */

    return(1);
    }

  rc = addclient(value);

  if (rc != 0)
    {
    /* FAILURE */

    return(1);
    }

  return(0);
  }  /* END setpbsclient() */



u_long setjoboomscoreadjust(

  const char *value)  /* I */

  {
  int v = 0;
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  v = atoi(value);

  /* check for allowed value range */
  if( v >= -1000 && v <= 1000 ) 
    {
    job_oom_score_adjust = v;
    /* ok */
    return 1; 
    }
  else 
    {
    log_record(PBSEVENT_SYSTEM,
      PBS_EVENTCLASS_SERVER,
      __func__,
      "value is out of valid range <-17,15>. Using defaults");
    /* error */
    return 0;
    }
  }  /* END setjoboomscoreadjust() */



u_long setmomoomimmunize(

  const char *value)  /* I */

  {
  int           enable = -1;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (value == NULL)
    {
    /* FAILURE */

    return(0);
    }

  /* accept various forms of "true", "yes", and "1" */
  if ((enable = setbool(value)) != -1)
    mom_oom_immunize = enable;

  return(1);
  }  /* END setmomoomimmunize() */



/* FIXME: we need to handle a non-default port number */

u_long setpbsserver(

  const char *value)  /* I */

  {
  if ((value == NULL) || (*value == '\0'))
    {
    return(1);    /* FAILURE - nothing specified */
    }

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  return(mom_server_add(value));
  }  /* END setpbsserver() */




u_long settmpdir(

  const char *Value)

  {
  struct stat tmpdir_stat;
  int rc;
  
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, Value);

  if (*Value != '/')
    {
    log_err(-1, __func__, "tmpdir must be a full path");

    return(0);
    }

  snprintf(tmpdir_basename, sizeof(tmpdir_basename), "%s", Value);

  /* Make sure the tmpdir exists */
  rc = stat(tmpdir_basename, &tmpdir_stat);
  if (rc < 0)
    {
    if ((errno == ENOENT) || (errno == ENOTDIR))
      {
      sprintf(log_buffer,  "$tmpdir option is set to %s in mom_priv/config file. This directory does not exist. \nPlease correct this problem and try starting pbs_mom again.", tmpdir_basename);
      log_err(rc, __func__, log_buffer);
      }
    else
      {
      sprintf(log_buffer, "Failed to stat %s. %s\npbs_mom did not start.", tmpdir_basename, strerror(errno));
      log_err(rc, __func__, log_buffer);
      }

    exit(rc);
    }


  return(1);
  } /* END settmpdir() */




u_long setexecwithexec(

  const char *value)

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (!strncasecmp(value, "t", 1) ||
      (value[0] == '1') ||
      !strcasecmp(value, "on") )
    exec_with_exec = 1;
  else
    exec_with_exec = 0;

  return(1);
  } /* END setexecwithexec() */




u_long setxauthpath(

  const char *Value)

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, Value);

  if (*Value != '/')
    {
    log_err(-1, __func__, "xauthpath must be a full path");

    return(0);
    }

  snprintf(xauth_path, sizeof(xauth_path), "%s", Value);

  return(1);
  }




u_long setrcpcmd(

  const char *Value)  /* I */

  {
  static char *ptr;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, Value);

  if (*Value != '/')
    {
    log_err(-1, __func__, "rcpcmd must be a full path");

    /* FAILURE */

    return(0);
    }

  snprintf(rcp_path, sizeof(rcp_path), "%s", Value);

  strcpy(rcp_args, "");

  if ((ptr = strchr(rcp_path, ' ')) != NULL)
    {
    *ptr = '\0';

    if (*(ptr + 1) != '\0')
      {
      snprintf(rcp_args, sizeof(rcp_args), "%s", ptr + 1);
      }
    }

  /* SUCCESS */

  return(1);
  }  /* END setrcpcmd() */





u_long setlogevent(

  const char *value)

  {
  char *bad;

  *log_event_mask = strtol(value, &bad, 0);

  if ((*bad == '\0') ||
      isspace((int)*bad))
    {
    return(1);
    }

  return(0);
  }  /* END setlogevent() */





/* NOTE:  maskclient is global */

u_long restricted(

  const char *name)

  {
  char **tmpMaskClient;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, name);

  if (mask_max == 0)
    {
    if ((maskclient = (char **)calloc(4, sizeof(char *))) == NULL)
      {
      /* FAILURE - cannot alloc memory */

      log_err(errno, __func__, "cannot alloc memory");

      return(-1);
      }

    mask_max = 4;
    }

  maskclient[mask_num] = strdup(name);

  if (maskclient[mask_num] == NULL)
    {
    /* FAILURE - cannot alloc memory */

    log_err(errno, __func__, "cannot alloc memory");

    return(-1);
    }

  mask_num++;

  if (mask_num == mask_max)
    {
    mask_max *= 2;

    tmpMaskClient = (char **)realloc(
      maskclient,
      mask_max * sizeof(char *));

    if (tmpMaskClient == NULL)
      {
      /* FAILURE - cannot alloc memory */

      log_err(errno, __func__, "cannot alloc memory");

      return(-1);
      }

    maskclient = tmpMaskClient;
    }

  /* SUCCESS */

  return(1);
  }  /* END restricted() */





u_long configversion(

  const char *Value)  /* I */

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, Value);

  if (Value == NULL)
    {
    /* FAILURE */

    return(0);
    }

  snprintf(MOMConfigVersion, sizeof(MOMConfigVersion), "%s", Value);

  /* SUCCESS */

  return(1);
  }  /* END configversion() */





unsigned long setdownonerror(

  const char *value)  /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((enable = setbool(value)) != -1)
    MOMConfigDownOnError = enable;

  return(1);
  }  /* END setdownonerror() */



unsigned long setenablemomrestart(

  const char *value)  /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((enable = setbool(value)) != -1)
    MOMConfigRestart = enable;

  return(1);
  }  /* END setenablemomrestart() */




u_long cputmult(

  const char *value)  /* I */

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((cputfactor = atof(value)) == 0.0)
    {
    return(0); /* error */
    }

  return(1);
  }  /* END cputmult() */





u_long wallmult(

  const char *value)

  {
  double tmpD;

  if (value == NULL)
    {
    /* FAILURE */

    return(0);
    }

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  tmpD = atof(value);

  if ((tmpD == 0.0) && (value[0] != '\0'))
    {
    /* FAILURE */

    return(0);
    }

  /* SUCCESS */

  wallfactor = tmpD;

  return(1);
  }  /* END wallmult() */




u_long usecp(

  const char *value)  /* I */

  {
  char        *pnxt;

  cphosts cph;

  /* FORMAT:  <HOST>:<FROM> <TO> */

  /*
   * HvB and Willem added this for logging purpose
   */

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  pnxt = strchr((char *)value, (int)':');

  if (pnxt == NULL)
    {
    /* request failed */

    sprintf(log_buffer, "invalid host specification: %s",
      value);

    log_err(-1, __func__, log_buffer);

    return(0);
    }

  *pnxt++ = '\0';

  cph.cph_hosts = value;

  value = pnxt; /* now ptr to path */

  while (!isspace(*pnxt))
    {
    if (*pnxt == '\0')
      {
      sprintf(log_buffer, "invalid '%s' specification %s: "
        "missing destination path",
        __func__,
        value);

      log_err(-1, __func__, log_buffer);

      return(0);
      }

    pnxt++;
    }

  *pnxt++ = '\0';

  cph.cph_from = value;
  cph.cph_to = skipwhite(pnxt);

  pcphosts.push_back(cph);

  return(1);
  }  /* END usecp() */



/*
 * prologalarm()
 */

unsigned long prologalarm(

  const char *value)  /* I */

  {
  long i;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  i = strtol(value, NULL, 10);

  if (i <= 0)
    {
    return(0); /* error */
    }

  pe_alarm_time = (long)i;

  return(1);
  }  /* END prologalarm() */





unsigned long setloglevel(

  const char *value)  /* I */

  {
  int i;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  i = (int)atoi(value);

  if (i < 0)
    {
    return(0);  /* error */
    }

  LOGLEVEL = (unsigned int)i;

  return(1);
  }  /* END setloglevel() */




unsigned long setmaxupdatesbeforesending(
    
  const char *value)

  {
  int i;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  i = (int)atoi(value);

  if (i < 0)
    return(0); /* error */

  maxupdatesbeforesending = i;

  /* SUCCESS */
  return(1);
  } /* END setmaxupdatesbeforesending() */





unsigned long setumask(

  const char *value)  /* I */

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  snprintf(DEFAULT_UMASK, sizeof(DEFAULT_UMASK), "%s", value);

  return(1);
  }  /* END setumask() */




unsigned long setpreexec(

  const char *value)  /* I */

  {
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  snprintf(PRE_EXEC, sizeof(PRE_EXEC), "%s", value);

#if SHELL_USE_ARGV == 0
  log_err(0, __func__, "pbs_mom not configured with enable-shell-user-argv option");
#endif

  return(1);
  }  /* END setpreexec() */


unsigned long setsourceloginbatch(

  const char *value)  /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((enable = setbool(value)) != -1)
    src_login_batch = enable;

  return(1);
  }  /* END setsourceloginbatch() */


unsigned long setsourcelogininteractive(

  const char *value)  /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((enable = setbool(value)) != -1)
    src_login_interactive = enable;

  return(1);
  }  /* END setsourcelogininteractive() */



unsigned long jobstartblocktime(

  const char *value)  /* I */

  {
  int i;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  i = (int)strtol(value, NULL, 10);

  if ((i < 0) || ((i == 0) && (value[0] != '0')))
    {
    return(0);  /* error */
    }

  TJobStartBlockTime = i;

  return(1);
  }  /* END jobstartblocktime() */





unsigned long setstatusupdatetime(

  const char *value)  /* I */

  {
  int i;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  i = (int)strtol(value, NULL, 10);

  if (i < 1)
    {
    return(0);  /* error */
    }

  ServerStatUpdateInterval = (unsigned int)i;

  return(1);
  }  /* END setstatusupdatetime() */





unsigned long setcheckpolltime(

  const char *value)  /* I */

  {
  int i;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  i = (int)strtol(value, NULL, 10);

  if (i < 1)
    {
    return(0);  /* error */
    }

  CheckPollTime = (unsigned int)i;

  return(1);
  }  /* END setcheckpolltime() */




/*
** Add static resource or shell escape line from config file.
** This is a support routine for read_config().
*/

void add_static(

  char *str,     /* I */
  char *file,    /* I */
  int   linenum) /* I */

  {
  int  i;
  char  name[50];

  struct config_list *cp;

  str = tokcpy(str, name); /* resource name */
  str = skipwhite(str);   /* resource value */

  /* FORMAT:  <ATTR> [!]<VALUE> */

  if (*str == '!') /* shell escape command */
    {
    /* remove trailing newline */

    rmnl(str);
    }
  else
    {
    /* get the value */
    i = strlen(str);

    while (--i)
      {
      /* strip trailing blanks */

      if (!isspace((int)*(str + i)))
        break;

      *(str + i) = '\0';
      }
    }

  cp = (struct config_list *)calloc(1, sizeof(struct config_list));

  memcheck((char *)cp);

  cp->c_link = config_list;
  cp->c.c_name = strdup(name);

  memcheck(cp->c.c_name);

  cp->c.c_u.c_value = strdup(str);

  memcheck(cp->c.c_u.c_value);

  sprintf(log_buffer, "%s[%d] add name %s value %s",
    file,
    linenum,
    name,
    str);

  log_record(PBSEVENT_DEBUG, 0, __func__, log_buffer);

  config_list = cp;

  return;
  }  /* END add_static() */



/*
 * reset_config_vars() - reset all config variables to defaults
 *
 * When pbs_mom receives a HUP signal, the configuration file
 * needs to be reloaded. In order for that to be successful,
 * all the global variables need to be reset to their default state.
 */
void reset_config_vars()
  {
  thread_unlink_calls = FALSE;
  /* by default, enforce these policies */
  ignwalltime = 0;
  ignmem = 0;
  igncput = 0;
  ignvmem = 0; 
  /* end policies */
  spoolasfinalname = 0;
  maxupdatesbeforesending = MAX_UPDATES_BEFORE_SENDING;
  apbasil_path     = NULL;
  apbasil_protocol = NULL;
  reject_job_submit = 0;
  attempttomakedir = 0;
  reduceprologchecks = 0;
  CheckPollTime = CHECK_POLL_TIME;
  cputfactor = 1.00;
  ideal_load_val = -1.0;
  exec_with_exec = 0;
  ServerStatUpdateInterval = DEFAULT_SERVER_STAT_UPDATES;
  max_load_val = -1.0;
  auto_ideal_load = NULL;
  auto_max_load   = NULL;
  AllocParCmd = NULL;  /* (alloc) */
  PBSNodeCheckPath[0] = '\0';
  PBSNodeCheckProlog = 0;
  PBSNodeCheckEpilog = 0;
  PBSNodeCheckInterval = 1;
  job_oom_score_adjust = 0;  /* no oom score adjust by default */
  mom_oom_immunize = 1;  /* make pbs_mom processes immune? no by default */
  log_file_max_size = 0;
  log_file_roll_depth = 1;
  LOGKEEPDAYS = 0; /* days each log file should be kept before deleting */
  EXTPWDRETRY = 3; /* # of times to try external pwd check */
  nodefile_suffix = NULL;    /* suffix to append to each host listed in job host file */
  submithost_suffix = NULL;  /* suffix to append to submithost for interactive jobs */
  mom_host[0] = '\0';
  hostname_specified = 0;
  MOMConfigRReconfig = 0;
  TNoSpoolDirList[0] = '\0';
  is_reporter_mom = FALSE;
  is_login_node = FALSE;
  job_exit_wait_time = DEFAULT_JOB_EXIT_WAIT_TIME;
  jobstarter_exe_name[0] = '\0';
  jobstarter_set = 0;
  server_alias = NULL;
  TRemChkptDirList[0] = '\0';
  tmpdir_basename[0] = '\0';  /* for $TMPDIR */
  rcp_path[0] = '\0';
  rcp_args[0] = '\0';
  xauth_path[0] = '\0';
  mask_num = 0;
  mask_max = 0;
  maskclient = NULL; /* wildcard connections */
  memset(MOMConfigVersion, 0, sizeof(MOMConfigVersion));
  MOMConfigDownOnError = 0;
  MOMConfigRestart = 0;
  MOMCudaVisibleDevices = 1;
  wallfactor = 1.00;
  pcphosts.clear();
  pe_alarm_time = PBS_PROLOG_TIME;
  DEFAULT_UMASK[0] = '\0';
  PRE_EXEC[0] = '\0';
  src_login_batch = TRUE;
  src_login_interactive = TRUE;
  TJobStartBlockTime = 5; /* seconds to wait for job to launch before backgrounding */
  strncpy(config_file, "config", sizeof(config_file));
  config_file_specified = 0;
  config_array = NULL;
  #ifdef PENABLE_LINUX26_CPUSETS
  MOMConfigUseSMT = 1; /* 0: off, 1: on */
  memory_pressure_threshold = 0; /* 0: off, >0: check and kill */
  memory_pressure_duration  = 0; /* 0: off, >0: check and kill */
  #endif
  max_join_job_wait_time = MAX_JOIN_WAIT_TIME;
  resend_join_job_wait_time = RESEND_WAIT_TIME;
  mom_hierarchy_retry_time = NODE_COMM_RETRY_TIME;
  LOGLEVEL = 0;
  }



/*
** Open and read the config file.  Save information in a linked
** list.  After reading the file, create an array, copy the list
** elements to the array and free the list.
*/

/* NOTE:  add new mom config parameters to 'special[]' */

int read_config(

  char *file)  /* I */

  {
  FILE                 *conf;

  struct stat            sb;

  struct config_list *cp;

  struct config  *ap;
  char                   line[120];
  char                   name[50];
  char                  *str;
  char                  *ptr;

  int                    linenum;
  int                    i;

  int                    IgnConfig = 0;

  int                    rc;

  int n, list_len;
  char *server_list_ptr;
  char *tp;


  if (LOGLEVEL >= 3)
    {
    sprintf(log_buffer, "updating configuration using file '%s'",
            (file != NULL) ? file : "NULL");

    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
    }

  for (i = 0;i < mask_num;i++)
    {
    free(maskclient[i]);
    }

  mask_num = 0;

  if (file == NULL)
    file = config_file;

  rc = 0;

  if (file[0] == '\0')
    {
    log_record(
      PBSEVENT_SYSTEM,
      PBS_EVENTCLASS_SERVER,
      __func__,
      "ALERT:  no config file specified");

    IgnConfig = 1;  /* no config file */
    }

  if ((IgnConfig == 0) && (stat(file, &sb) == -1))
    {
    IgnConfig = 1;

    if (config_file_specified != 0)
      {
      /* file specified and not there, return failure */

      log_record(
        PBSEVENT_SYSTEM,
        PBS_EVENTCLASS_SERVER,
        __func__,
        "ALERT:  cannot open config file - no file");

      rc = 1;
      }
    else
      {
      /* "config" file not located, return success */
      if (LOGLEVEL >= 3)
        {
        sprintf(log_buffer, "cannot open file '%s'", file);

        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
        }

      rc = 0;
      }
    }  /* END if ((IgnConfig == 0) && (stat(file,&sb) == -1)) */

  if (IgnConfig == 0)
    {
#if !defined(DEBUG) && !defined(NO_SECURITY_CHECK)

    if (chk_file_sec(file, 0, 0, S_IWGRP | S_IWOTH, 1, NULL))
      {
      /* not authorized to access specified file, return failure */

      log_record(
        PBSEVENT_SYSTEM,
        PBS_EVENTCLASS_SERVER,
        __func__,
        "ALERT:  cannot open config file - permissions");

      IgnConfig = 1;

      rc = 1;
      }

#endif  /* NO_SECURITY_CHECK */
    }    /* END if (ignConfig == 0) */

  if (IgnConfig == 0)
    {
    if ((conf = fopen(file, "r")) == NULL)
      {
      sprintf(log_buffer, "fopen: %s",
              file);

      log_err(errno, __func__, log_buffer);

      IgnConfig = 1;

      rc = 1;
      }
    }    /* END if (IgnConfig == 0) */

  if (IgnConfig == 0)
    {
    nconfig = 0;
    linenum = 0;

    memset(line, 0, sizeof(line));
    memset(name, 0, sizeof(name));

    while (fgets(line, sizeof(line) - 1, conf))
      {
      linenum++;

      if (line[0] == '#') /* comment */
        {
        memset(line, 0, sizeof(line));
        continue;
        }

      if ((ptr = strchr(line, '#')) != NULL)
        {
        /* allow inline comments */

        *ptr = '\0';
        }

      str = skipwhite(line); /* pass over initial whitespace */

      if (*str == '\0')
        {
        memset(line, 0, sizeof(line));
        continue;
        }

      if (LOGLEVEL >= 6)
        {
        sprintf(log_buffer, "processing config line '%.64s'", str);

        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
        }

      if (*str == '$')
        {
        /* special command */

        str = tokcpy(++str, name); /* resource name */

        for (i = 0;special[i].name;i++)
          {
          if (strcasecmp(name, special[i].name) == 0)
            break;
          }  /* END for (i) */

        if (special[i].name == NULL)
          {
          /* didn't find it */

          sprintf(log_buffer, "special command name %s not found (ignoring line)",
                  name);

          log_err(-1, __func__, log_buffer);
    
          memset(line, 0, sizeof(line));

          continue;
          }

        str = skipwhite(str);  /* command param */

        rmnl(str);

        if (special[i].handler(str) == 0)
          {
          sprintf(log_buffer, "%s[%d] special command %s failed with %s",
            file,
            linenum,
            name,
            str);

          log_err(-1, __func__, log_buffer);
          }
    
        memset(line, 0, sizeof(line));

        continue;
        }

      add_static(str, file, linenum);

      nconfig++;
    
      memset(line, 0, sizeof(line));
      }  /* END while (fgets()) */

    /*
    ** Create a new array.
    */

    if (config_array != NULL)
      {
      for (ap = config_array;ap->c_name != NULL;ap++)
        {
        free((void *)ap->c_name);
        free((void *)ap->c_u.c_value);
        }

      free(config_array);
      }

    config_array = (struct config *)calloc(nconfig + 1, sizeof(struct config));

    memcheck((const char *)config_array);

    /*
    ** Copy in the new information saved from the file.
    */

    for (i = 0, ap = config_array;i < nconfig;i++, ap++)
      {
      *ap = config_list->c;
      cp = config_list->c_link;

      free(config_list); /* don't free name and value strings */
      config_list = cp; /* they carry over from the list */
      }

    ap->c_name = NULL;  /* one extra */

    fclose(conf);
    }  /* END if (IgnConfig == 0) */

  if (mom_server_count == 0)
    {
    /* No server names in torque/mom_priv/config.  Get names from torque/server_name. */

    server_list_ptr = pbs_get_server_list();
    list_len = csv_length(server_list_ptr);

    for (n = 0; n < list_len; n++)
      {
      tp = csv_nth(server_list_ptr, n);

      if (tp)
        {
        /* Trim any leading space */
        while(isspace(*tp)) tp++;

        setpbsserver(tp);
        }
      }
    }

  return(rc);
  }  /* END read_config() */



#ifdef PENABLE_LINUX26_CPUSETS
u_long setusesmt(

    const char *value)  /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((enable = setbool(value)) != -1)
    MOMConfigUseSMT = enable;

  return(1);
  }  /* END setusesmt() */


u_long setmempressthr(

  const char *value)

  {
  long long val;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((val = atoll(value)) < 0)
    return(0);  /* error, leave as is */

  if (val > INT_MAX)
    val = INT_MAX;

  memory_pressure_threshold = (int)val;

  return(1);
  }

u_long setmempressdur(

  const char *value)

  {
  int val;

  val = atoi(value);

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if ((val = atoi(value)) < 0)
    return(0);  /* error, leave as is */

  if (val > SHRT_MAX)
    val = SHRT_MAX;

  memory_pressure_duration = (short)val;

  return(1);
  }
#endif



unsigned long setmaxjoinjobwaittime(

  const char *value)

  {
  int tmp;
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (value != NULL)
    {
    tmp = strtol(value, NULL, 10);
    if (tmp != 0)
      max_join_job_wait_time = tmp;
    }

  return(1);
  } /* END setmaxjoinjobwaittime() */



unsigned long setresendjoinjobwaittime(

  const char *value)

  {
  int tmp;
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (value != NULL)
    {
    tmp = strtol(value, NULL, 10);
    if (tmp != 0)
      resend_join_job_wait_time = tmp;
    }

  return(1);
  } /* END setresendjoinjobwaittime() */



unsigned long setmomhierarchyretrytime(

  const char *value)

  {
  int tmp;
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, value);

  if (value != NULL)
    {
    tmp = strtol(value, NULL, 10);
    if (tmp != 0)
      mom_hierarchy_retry_time = tmp;
    }

  return(1);
  }



const char *arch(

  struct rm_attribute *attrib)  /* I */

  {
  struct config *cp;

  if (attrib != NULL)
    {
    log_err(-1, __func__, extra_parm);

    rm_errno = RM_ERR_BADPARAM;

    return(NULL);
    }

  if (config_array == NULL)
    {
    return(PBS_MACH);
    }

  /* locate arch string */

  for (cp = config_array;cp->c_name != NULL;cp++)
    {
    if (cp->c_u.c_value == NULL)
      continue;

    if (strcmp(cp->c_name, "arch"))
      continue;

    return(cp->c_u.c_value);
    }  /* END for (cp) */

  return(PBS_MACH);
  }  /* END arch() */




const char *opsys(

  struct rm_attribute *attrib)  /* I */

  {
  struct config *cp;

  if (attrib != NULL)
    {
    log_err(-1, __func__, extra_parm);

    rm_errno = RM_ERR_BADPARAM;

    return(NULL);
    }

  if (config_array == NULL)
    {
    return(PBS_MACH);
    }

  /* locate opsys string */

  for (cp = config_array;cp->c_name != NULL;cp++)
    {
    if (cp->c_u.c_value == NULL)
      continue;

    if (strcmp(cp->c_name, "opsys"))
      continue;

    return(cp->c_u.c_value);
    }  /* END for (cp) */

  return(PBS_MACH);
  }  /* END opsys() */



const char *reqmsg(

  struct rm_attribute *attrib)

  {
  if (attrib != NULL)
    {
    log_err(-1, __func__, extra_parm);

    rm_errno = RM_ERR_BADPARAM;

    return(NULL);
    }

  return(PBSNodeMsgBuf);
  }  /* END reqmsg() */
  


void add_job_status_information(

  job               &pjob,
  std::stringstream &list)

  {
  list << "(";
  encode_used(&pjob, ATR_DFLAG_MGRD, &list, NULL); /* adds resources_used attr */

  encode_flagged_attrs(&pjob, ATR_DFLAG_MGRD, &list, NULL); /* adds other flagged attrs */
  list << ")";
  } /* END add_job_status_information() */



const char *getjoblist(

  struct rm_attribute *attrib) /* I */

  {
  std::stringstream  list;
  job               *pjob;
  bool               firstjob = true;

#ifdef NUMA_SUPPORT
  char  mom_check_name[PBS_MAXSERVERNAME];
  char *dot;
#endif 

  // reset the job list
  list.clear();

  if (alljobs_list.size() == 0)
    {
    /* no jobs - return space character */

    return(" ");
    }

#ifdef NUMA_SUPPORT 
  /* initialize the name to check for for this numa mom */
  strcpy(mom_check_name,mom_host);

  if ((dot = strchr(mom_check_name,'.')) != NULL)
    *dot = '\0';

  sprintf(mom_check_name + strlen(mom_check_name),"-%d/",numa_index);
#endif

  std::list<job *>::iterator iter;

  for (iter = alljobs_list.begin(); iter != alljobs_list.end(); iter++)
    {
    pjob = *iter;

#ifdef NUMA_SUPPORT
    /* skip over jobs that aren't on this node */
    if (strstr(pjob->ji_wattr[JOB_ATR_exec_host].at_val.at_str,mom_check_name) == NULL)
      continue;
#endif

    if (!firstjob)
      list << " ";
    
    firstjob = false;

    list << pjob->ji_qs.ji_jobid;

    if (am_i_mother_superior(*pjob) == true)
      {
      add_job_status_information(*pjob, list);
      }
    }  /* END for (pjob) */

#ifdef NUMA_SUPPORT
  if (firstjob == true)
    list << " ";
#endif

  return(strdup(list.str().c_str()));
  }  /* END getjoblist() */




#define TMAX_VARBUF   65536

const char *reqvarattr(

  struct rm_attribute *attrib)  /* I */

  {
  static char    *list = NULL;
  char           *child_spot;
  static int      listlen = 0;

  struct varattr *pva;
  int             fd, len, child_len;
  int             first_line;
  FILE           *child;

  char           *ptr;
  char           *ptr2;

  char            tmpBuf[TMAX_VARBUF + 1];

  if (list == NULL)
    {
    list = (char *)calloc(BUFSIZ + 1024, sizeof(char));

    if (list == NULL)
      {
      /* FAILURE - cannot alloc memory */

      log_err(errno, __func__, "cannot alloc memory");

      return((char *)" ");
      }

    listlen = BUFSIZ;
    }

  *list = '\0'; /* reset the list */

  if ((pva = (struct varattr *)GET_NEXT(mom_varattrs)) == NULL)
    {
    return((char *)" ");
    }

  for (;pva != NULL;pva = (struct varattr *)GET_NEXT(pva->va_link))
    {
    /* loop for each $varattr parameter */

    if ((pva->va_lasttime == 0) || (time_now >= (pva->va_ttl + pva->va_lasttime)))
      {
      if ((pva->va_ttl == -1) && (pva->va_lasttime != 0))
        {
        if (pva->va_value[0] != '\0')
          {
          if (*list != '\0')
            strcat(list, varattr_delimiter);

          strcat(list, pva->va_value);
          }

        if ((int)strlen(list) >= listlen)
          {
          listlen += BUFSIZ;

          list = (char *)realloc(list, listlen);

          if (list == NULL)
            {
            log_err(errno, __func__, "cannot alloc memory");

            return((char *)" ");
            }
          }

        continue;  /* ttl of -1 is only run once */
        }

      /* TTL is satisfied, reload value */

      pva->va_lasttime = time_now;

      if (pva->va_value == NULL)
        pva->va_value = (char *)calloc(TMAX_VARBUF, sizeof(char));

      /* execute script and get a new value */

      if ((child = popen(pva->va_cmd, "r")) == NULL)
        {
        sprintf(pva->va_value, "error: %d %s",
          errno,
          strerror(errno));
        }
      else
        {
        fd = fileno(child);
        fcntl(fd, F_SETFL, O_NONBLOCK);

        child_spot = tmpBuf;
        child_len  = 0;
        child_spot[0] = '\0';

        while (child_len < TMAX_VARBUF)
          {
          len = read_ac_socket(fd, child_spot, TMAX_VARBUF - child_len);

          if ((len <= 0) &&
              (errno != EINTR))
            break;
          else if (len < 0)
            continue;

          child_len  += len;
          child_spot += len;
          }

        if (len == -1)
          {
          /* FAILURE - cannot read var script output */
          log_err(errno, __func__, "Cannot read pipe for reqvarattr script. Please check!");

          sprintf(pva->va_value, "? %d",
            RM_ERR_SYSTEM);

          // Do not plose here as it will hang until the child exits.

          continue;
          }

        /* SUCCESS */

        pclose(child);

        tmpBuf[child_len] = '\0';

        /* Transfer returned data into var value field */

        first_line = TRUE;

        ptr = strtok(tmpBuf,"\n;");

        ptr2 = pva->va_value;

        ptr2[0] = '\0';

        /*
         * OUTPUT FORMAT:  Take what script gives us.
         * Script should output 1 or more lines of Name=value1+value2+...
         */

        while (ptr != NULL)
          {
          if (!first_line)
            strcat(ptr2,varattr_delimiter);

          strcat(ptr2,ptr);

          first_line = FALSE;

          ptr = strtok(NULL,"\n;");
          }  /* END while (ptr != NULL) */
        }    /* END else ((child = popen(pva->va_cmd,"r")) == NULL) */
      }      /* END if ((pva->va_lasttime == 0) || ...) */

    if (pva->va_value[0] != '\0')
      {
      if (*list != '\0')
        strcat(list, varattr_delimiter);

      strcat(list, pva->va_value);
      }

    if ((int)strlen(list) >= listlen)
      {
      listlen += BUFSIZ;
      list = (char *)realloc(list, listlen);

      if (list == NULL)
        {
        log_err(errno, __func__, "cannot alloc memory");

        return((char *)" ");
        }
      }
    }    /* END for (pva) */

  if (list[0] == '\0')
    strcat(list, " ");

  return(list);
  }  /* END reqvarattr() */



const char *reqgres(

  struct rm_attribute *attrib)  /* I (ignored) */

  {
  struct config *cp;

  static char   GResBuf[1024];
  char          tmpLine[1024];

  int           sindex;

  if (attrib != NULL)
    {
    log_err(-1, __func__, extra_parm);

    rm_errno = RM_ERR_BADPARAM;

    return(NULL);
    }

  /* build gres string */

  /* FORMAT:  <GRES>:<VALUE>[+<GRES>:<VALUE>]... */

  GResBuf[0] = '\0';

  if (config_array == NULL)
    {
    return(GResBuf);
    }

  for (cp = config_array; cp->c_name != NULL; cp++)
    {
    if (cp->c_u.c_value == NULL)
      continue;

    /* verify parameter is not special */

    for (sindex = 0;sindex < RM_NPARM;sindex++)
      {
      if (special[sindex].name == NULL)
        break;

      if (!strcmp(special[sindex].name, cp->c_name))
        break;
      }  /* END for (sindex) */

    if ((sindex < RM_NPARM) &&
        (special[sindex].name != NULL) &&
        (!strcmp(special[sindex].name, cp->c_name)))
      {
      /* specified parameter is special parameter */

      continue;
      }

    /* verify parameter is not common */
    /* Coverity reports (correctly) that RM_NPARM == 32, but common_config
     * only has 10 elements, so this loop's upper bound is dangerous.
     * However, the final element in common_config has c_name == NULL,
     * so the first test inside this loop safely breaks.
     */
    for (sindex = 0;sindex < RM_NPARM;sindex++)
      {
      if (common_config[sindex].c_name == NULL)
        break;

      if (!strcmp(common_config[sindex].c_name, cp->c_name))
        break;
      }  /* END for (sindex) */

    if ((sindex < RM_NPARM) &&
        (common_config[sindex].c_name != NULL) &&
        !strcmp(common_config[sindex].c_name, cp->c_name) &&
        strcmp(common_config[sindex].c_name, "gres"))
      {
      /* specified parameter is common parameter */

      continue;
      }

    if (!strncmp(cp->c_name, "size", strlen("size")))
      continue;

    if (GResBuf[0] != '\0')
      {
      strncat(GResBuf, "+", sizeof(GResBuf) - 1 - strlen(GResBuf));
      }

    {
      // check if shell escape needed
      char *p = conf_res(cp->c_u.c_value, NULL);
      snprintf(tmpLine, 1024, "%s:%s",
               cp->c_name, p);
    }

    strncat(GResBuf, tmpLine, (sizeof(GResBuf) - strlen(GResBuf) - 1));
    }  /* END for (cp) */

  return(GResBuf);
  }  /* END reqgres() */




const char *reqstate(

  struct rm_attribute *attrib)  /* I (ignored) */

  {
  static char state[1024];

  if ((internal_state & INUSE_DOWN) && (MOMConfigDownOnError != 0))
    strcpy(state, "down");
  else if (internal_state & INUSE_BUSY)
    strcpy(state, "busy");
  else
    strcpy(state, "free");

  return(state);
  }  /* END reqstate() */




const char *requname(

  struct rm_attribute *attrib)

  {
  const char *cp;

  if (attrib != NULL)
    {
    log_err(-1, __func__, extra_parm);

    rm_errno = RM_ERR_BADPARAM;

    return(NULL);
    }

  cp = getuname();

  return(cp);
  }  /* END requname() */





const char *validuser(

  struct rm_attribute *attrib)

  {
  struct passwd *p;
  char          *buf;

  if ((attrib == NULL) || (attrib->a_value == NULL))
    {
    log_err(-1, __func__, no_parm);
    rm_errno = RM_ERR_NOPARAM;

    return(NULL);
    }

  p = getpwnam_ext(&buf, attrib->a_value);

  if (p != NULL)
    {
    free_pwnam(p, buf);
    return("yes");
    }

  return("no");
  }    /* END validuser() */

u_long setcudavisibledevices(

  const char *value)  /* I */

  {
  int enable;

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, "cudavisibledevices", value);

  if ((enable = setbool(value)) != -1)
    MOMCudaVisibleDevices = enable;

  return(1);
  }  /* END setcudavisibledevices() */

