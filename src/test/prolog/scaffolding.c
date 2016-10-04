#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pwd.h> /* gid_t, uid_t */
#include <sstream>
#include <map>

#include "resource.h" /* resource_def */
#include "log.h" /* LOG_BUF_SIZE */
#include "attribute.h" /* pbs_attribute */
#include "list_link.h" /* tlist_head, list_link */
#include "pbs_job.h" /* job */
#include "libpbs.h" /* job_file */
#include "mom_config.h"
#include "mom_mach.h"
#include "json/json.h"

#define MAXLINE 1024

int lockfds = -1;
int svr_resc_size = 0;
int DEBUGMODE = 0;
char *path_aux = strdup("/tmp");
int reduceprologchecks;
resource_def *svr_resc_def;
char PBSNodeMsgBuf[MAXLINE];
int MOMPrologFailureCount;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
char log_buffer[LOG_BUF_SIZE];
int MOMPrologTimeoutCount;
uid_t pbsuser;
gid_t pbsgroup;
long     pe_alarm_time = PBS_PROLOG_TIME;
char *path_epilogp;
long TJobStartTimeout = 300;
std::map<std::string, std::string> env_map;
char *path_jobs;
int multi_mom;
unsigned int pbs_rm_port = 0;

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

int setuid_ext(uid_t uid, int set_euid)
  {
  return(0);
  }

proc_stat_t *get_proc_stat(int pid)
  {
  return(NULL);
  }

int put_env_var(const char *name, const char *value)
  {
  std::pair<std::string, std::string> p(name, value);
  env_map.insert(p);
  return(0);
  }

void encode_used(job *pjob, int perm, Json::Value *list, tlist_head *phead)

  {
  }

int become_the_user(

  job *pjob)
  
  {
  return(0);
  }

void log_ext(

  int         errnum,   /* I (errno or PBSErrno) */
  const char *routine,  /* I */
  const char *text,     /* I */
  int         severity) /* I */

  {
  }

int is_joined(

  job *pjob)   /* I */

  {
  return(0);
  }

void log_record(

  int         eventtype,  /* I */
  int         objclass,   /* I */
  const char *objname,    /* I */
  const char *text)       /* I */

  {
  }

void net_close(

  int but)  /* I */

  {
  }


void log_close(

  int msg)  /* BOOLEAN - write close message */ {}

int open_std_file(

  job           *pjob,   /* I */
  enum job_file  which,  /* which file */
  int            mode,   /* file mode */
  gid_t          exgid)  /* gid for file */

  {
  return(0);
  }

int TTmpDirName(
    
  job  *pjob,   /* I */
  char *tmpdir, /* O */
  int   tmpdir_size)
  
  {
  snprintf(tmpdir, tmpdir_size, "/home/dbeer/scratch/");
  return(1);
  }

ssize_t write_ac_socket(

  int         fd,    /* I */
  const void *buf,   /* I */
  ssize_t     count) /* I */
  
  {
  return(0);
  }

int pe_input(

  char *jobid)
  
  {
  return(0);
  }

char *get_job_envvar(

  job  *pjob,     /* I */
  const char *variable) /* I */
  
  {
  return(NULL);
  }

void log_err(

  int         errnum,  /* I (errno or PBSErrno) */
  const char *routine, /* I */
  const char *text)    /* I */

  {
  }
  
resource *find_resc_entry(

  pbs_attribute *pattr,  /* I */
  resource_def  *rscdf)  /* I */


  {
  static resource r;

  r.rs_value.at_val.at_str = strdup("5");
  return(&r);
  }


resource_def *find_resc_def(

  resource_def *rscdf, /* address of array of resource_def structs */
  const char   *name, /* name of resource */
  int           limit) /* number of members in resource_def array */

  {
  return(NULL);
  }

char *arst_string(

  const char   *str,
  pbs_attribute *pattr)

  {
  return(NULL);
  }
