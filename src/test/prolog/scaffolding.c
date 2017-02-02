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

void encode_used(mom_job *pjob, int perm, Json::Value *list, tlist_head *phead)

  {
  }

int encode_resc(

  pbs_attribute  *attr,    /* ptr to pbs_attribute to encode */
  tlist_head     *phead,   /* head of attrlist list */
  const char    *atname,  /* pbs_attribute name */
  const char    *rsname,  /* resource name, null on call */
  int             mode,    /* encode mode */
  int             ac_perm) /* access permissions */

  {
  return(0);
  }

int become_the_user(

  mom_job *pjob,
  bool want_effective)
  
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

  mom_job *pjob)   /* I */

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

  mom_job           *pjob,   /* I */
  enum job_file  which,  /* which file */
  int            mode,   /* file mode */
  gid_t          exgid)  /* gid for file */

  {
  return(0);
  }

int TTmpDirName(
    
  mom_job  *pjob,   /* I */
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

  const char *jobid)
  
  {
  return(0);
  }

char *get_job_envvar(

  mom_job  *pjob,     /* I */
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

job::job() 
  
  {
  memset(&this->ji_qs, 0, sizeof(this->ji_qs));
  }

job::~job() {}

mom_job::mom_job() : ji_tasks()
  {
  }
mom_job::~mom_job() {}

void job::set_attr_flag(int index, int flag_val)

  {
  if ((index >= 0) &&
      (index < JOB_ATR_LAST))
    this->ji_wattr[index].at_flags = flag_val; 
  }

int job::get_mom_exitstat() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_exitstat);
  }

void job::set_mom_exitstat(int ev)
  {
  this->ji_qs.ji_un.ji_momt.ji_exitstat = ev;
  }

pbs_net_t job::get_svraddr() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_svraddr);
  }

void job::set_attr_flag_bm(int index, int bm)
  {
  if ((index >= 0) &&
      (index < JOB_ATR_LAST))
    this->ji_wattr[index].at_flags |= bm;
  }

struct timeval *job::get_tv_attr(int index)
  {
  return(&this->ji_wattr[index].at_val.at_timeval);
  }

int job::set_tv_attr(int index, struct timeval *tv)
  {
  memcpy(&(this->ji_wattr[index].at_val.at_timeval), tv, sizeof(struct timeval));
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

int job::set_resc_attr(int index, std::vector<resource> *resources)
  {
  this->ji_wattr[index].at_val.at_ptr = resources;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

void job::set_exec_exitstat(int ev)
  {
  this->ji_qs.ji_un.ji_exect.ji_exitstat = ev;
  }

unsigned short job::get_ji_mom_rmport() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_mom_rmport);
  }

int job::set_creq_attr(int index, complete_req *cr)
  {
  this->ji_wattr[index].at_val.at_ptr = cr;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

void job::set_qs_version(int version)
  {
  this->ji_qs.qs_version = version;
  }

void job::set_queue(const char *queue)
  {
  snprintf(this->ji_qs.ji_queue, sizeof(this->ji_qs.ji_queue), "%s", queue);
  }

int job::get_un_type() const
  {
  return(this->ji_qs.ji_un_type);
  }

void job::set_ji_momaddr(unsigned long momaddr)
  {
  this->ji_qs.ji_un.ji_exect.ji_momaddr = momaddr;
  }

void job::set_ji_mom_rmport(unsigned short mom_rmport)
  {
  this->ji_qs.ji_un.ji_exect.ji_mom_rmport = mom_rmport;
  }

void job::set_ji_momport(unsigned short momport)
  {
  this->ji_qs.ji_un.ji_exect.ji_momport = momport;
  }

const char *job::get_queue() const
  {
  return(this->ji_qs.ji_queue);
  }

void job::set_scriptsz(size_t scriptsz)
  {
  this->ji_qs.ji_un.ji_newt.ji_scriptsz = scriptsz;
  }

size_t job::get_scriptsz() const
  {
  return(this->ji_qs.ji_un.ji_newt.ji_scriptsz);
  }

pbs_net_t job::get_fromaddr() const
  {
  return(this->ji_qs.ji_un.ji_newt.ji_fromaddr);
  }

int job::get_fromsock() const
  {
  return(this->ji_qs.ji_un.ji_newt.ji_fromsock);
  }

void job::set_fromaddr(pbs_net_t fromaddr)
  {
  this->ji_qs.ji_un.ji_newt.ji_fromaddr = fromaddr;
  }

void job::set_fromsock(int sock)
  {
  this->ji_qs.ji_un.ji_newt.ji_fromsock = sock;
  }

int job::get_qs_version() const
  {
  return(this->ji_qs.qs_version);
  }

void job::set_un_type(int type)
  {
  this->ji_qs.ji_un_type = type;
  }

int job::get_exec_exitstat() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_exitstat);
  }

int job::get_svrflags() const
  {
  return(this->ji_qs.ji_svrflags);
  }

void job::set_modified(bool m)
  {
  this->ji_modified = m;
  }

void job::set_attr(int index)
  {
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  }

void job::set_fileprefix(const char *prefix)
  {
  strcpy(this->ji_qs.ji_fileprefix, prefix);
  }

int job::set_char_attr(int index, char c)
  {
  this->ji_wattr[index].at_val.at_char = c;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

void job::set_svrflags(int flags)
  {
  this->ji_qs.ji_svrflags = flags;
  }

const char *job::get_destination() const
  {
  return(this->ji_qs.ji_destin);
  }

void job::free_attr(int index)
  {
  }

void job::set_substate(int substate)
  {
  this->ji_qs.ji_substate = substate;
  }

void job::set_state(int state)
  {
  this->ji_qs.ji_state = state;
  }

void job::set_destination(const char *destination)
  {
  snprintf(this->ji_qs.ji_destin, sizeof(this->ji_qs.ji_destin), "%s", destination);
  }

pbs_net_t job::get_ji_momaddr() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_momaddr);
  }

bool job::has_been_modified() const
  {
  return(this->ji_modified);
  }

tlist_head job::get_list_attr(int index)
  {
  return(this->ji_wattr[index].at_val.at_list);
  }

complete_req *job::get_creq_attr(int index) const
  {
  complete_req *cr = NULL;
  if (this->ji_wattr[index].at_flags & ATR_VFLAG_SET)
    cr = (complete_req *)this->ji_wattr[index].at_val.at_ptr;

  return(cr);
  }

void job::set_exgid(unsigned int gid)
  {
  this->ji_qs.ji_un.ji_momt.ji_exgid = gid;
  }

void job::set_exuid(unsigned int uid)
  {
  this->ji_qs.ji_un.ji_momt.ji_exuid = uid;
  }

unsigned short job::get_ji_momport() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_momport);
  }

void job::set_jobid(const char *jobid)
  {
  strcpy(this->ji_qs.ji_jobid, jobid);
  }

int job::get_attr_flags(int index) const
  {
  return(this->ji_wattr[index].at_flags);
  }

struct jobfix &job::get_jobfix()
  {
  return(this->ji_qs);
  }

int job::set_bool_attr(int index, bool b)
  {
  this->ji_wattr[index].at_val.at_bool = b;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

bool job::get_bool_attr(int index) const
  {
  return(this->ji_wattr[index].at_val.at_bool);
  }

std::vector<resource> *job::get_resc_attr(int index)
  {
  return((std::vector<resource> *)this->ji_wattr[index].at_val.at_ptr);
  }

const char *job::get_str_attr(int index) const
  {
  return(this->ji_wattr[index].at_val.at_str);
  }

const char *job::get_jobid() const
  {
  return(this->ji_qs.ji_jobid);
  }

int job::get_substate() const
  {
  return(this->ji_qs.ji_substate);
  }

int job::get_state() const
  {
  return(this->ji_qs.ji_state);
  }

void job::unset_attr(int index)
  {
  this->ji_wattr[index].at_flags = 0;
  }

bool job::is_attr_set(int index) const
  {
  return((this->ji_wattr[index].at_flags & ATR_VFLAG_SET) != 0);
  }

const char *job::get_fileprefix() const
  {
  return(this->ji_qs.ji_fileprefix);
  }

int job::set_long_attr(int index, long l)
  {
  this->ji_wattr[index].at_val.at_long = l;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

int job::set_str_attr(int index, char *str)
  {
  this->ji_wattr[index].at_val.at_str = str;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

long job::get_long_attr(int index) const
  {
  return(this->ji_wattr[index].at_val.at_long);
  }

time_t job::get_start_time() const
  {
  return(this->ji_qs.ji_stime);
  }

void job::set_start_time(time_t t)
  {
  this->ji_qs.ji_stime = t;
  }

pbs_attribute *job::get_attr(int index)
  {
  return(this->ji_wattr + index);
  }

unsigned int job::get_exgid() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_exgid);
  }

unsigned int job::get_exuid() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_exuid);
  }

void job::set_svraddr(pbs_net_t addr)
  {
  this->ji_qs.ji_un.ji_momt.ji_svraddr = addr;
  }

char job::get_char_attr(int index) const
  {
  return(this->ji_wattr[index].at_val.at_char);
  }

int job::set_arst_attr(int index, struct array_strings *arst)
  {
  this->ji_wattr[index].at_val.at_arst = arst;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

struct array_strings *job::get_arst_attr(int index)
  {
  return(this->ji_wattr[index].at_val.at_arst);
  }
