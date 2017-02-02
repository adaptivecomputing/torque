#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <unistd.h>
#include <pwd.h> /* struct passwd, gid_t */
#include <sys/socket.h> /* sockaddr */
#include <sys/types.h> /* pid_t */
#include <pwd.h> /* gid_t, uid_t */
#include <string>
#include <ctype.h>
#include <sys/types.h>
#include <grp.h>

#include "attribute.h" /* attribute_def, pbs_attribute, svrattrl */
#include "pbs_ifl.h" /* PBS_MAXUSER, MAXPATHLEN */
#include "mom_func.h" /* var_table */
#include "resmon.h" /* TMAX_NSDCOUNT */
#include "log.h" /* LOG_BUF_SIZE */
#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job, eventent, hnodent, tm_event_t, tm_task_id, task */
#include "pbs_ifl.h" /* job_file */
#include "tm_.h" /* tm_event_t */
#include "list_link.h" /* list_link, tlist_head */
#include "power_state.hpp"
#include "sys_file.hpp"
#include "log.h"

char *apbasil_protocol;
char *apbasil_path;
std::vector<cphosts> pcphosts;
char *path_undeliv;
const char *msg_err_unlink = "Unlink of %s file %s failed";
time_t time_now;
struct sig_tbl sig_tbl[5];
attribute_def job_attr_def[10];
int exiting_tasks = 0;
char pbs_current_user[PBS_MAXUSER];
char rcp_path[MAXPATHLEN];
char rcp_args[MAXPATHLEN];
const char *msg_jobmod = "Job Modified";
int cphosts_num = 0;
struct var_table vtable; 
char mom_host[PBS_MAXHOSTNAME + 1];
int spoolasfinalname = 0;
char *path_spool = strdup("/var/spool/torque/spool/");
unsigned int pbs_rm_port = 0;
unsigned int alarm_time = 10;
char *TNoSpoolDirList[TMAX_NSDCOUNT];
tlist_head svr_alljobs;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
const char *msg_manager = "%s at request of %s@%s";
int multi_mom = 1;
char MOMUNameMissing[64];
char log_buffer[LOG_BUF_SIZE];
char checkpoint_run_exe_name[MAXPATHLEN + 1];
std::list<mom_job *>    alljobs_list;

const char *wdir_ret;

void log_event(int event, int event_class, char *func_name, char *buf)
  {}

int become_the_user(mom_job *pjob, bool want_effective)
  {
  return(0);
  }

struct batch_request *alloc_br(int type)
  {
  fprintf(stderr, "The call to alloc_br needs to be mocked!!\n");
  exit(1);
  }

char *arst_string(const char *str, pbs_attribute *pattr)
  {
  return(strdup(wdir_ret));
  }

char *std_file_name(mom_job *pjob, enum job_file which, int *keeping)
  {
  fprintf(stderr, "The call to std_file_name needs to be mocked!!\n");
  exit(1);
  }

int mom_job_save(mom_job *pjob, int mom_port)
  {
  fprintf(stderr, "The call to job_save needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

pid_t fork_me(int conn)
  {
  fprintf(stderr, "The call to fork_me needs to be mocked!!\n");
  exit(1);
  }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr needs to be mocked!!\n");
  exit(1);
  }

int reply_send_mom(struct batch_request *request)
  {
  fprintf(stderr, "The call to reply_send_mom needs to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack needs to be mocked!!\n");
  exit(1);
  }

int replace_checkpoint_path(char *path)
  {
  fprintf(stderr, "The call to replace_checkpoint_path needs to be mocked!!\n");
  exit(1);
  }

int DIS_reply_read(struct tcp_chan *chan, struct batch_reply *preply)
  {
  fprintf(stderr, "The call to DIS_reply_read needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_JobFile(struct tcp_chan *chan, int seq, char *buf, int len, const char *jobid, int which)
  {
  return(0);
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br needs to be mocked!!\n");
  exit(1);
  }

eventent *event_alloc(int command, hnodent *pnode, tm_event_t event, tm_task_id taskid)
  {
  fprintf(stderr, "The call to event_alloc needs to be mocked!!\n");
  exit(1);
  }

int InitUserEnv(mom_job *pjob, task *ptask, char **envp, struct passwd *pwdp, char *shell)
  {
  fprintf(stderr, "The call to InitUserEnv needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int start_checkpoint(mom_job *pjob, int abort, struct batch_request *preq)
  {
  fprintf(stderr, "The call to start_checkpoint needs to be mocked!!\n");
  exit(1);
  }

int in_remote_checkpoint_dir(char *ckpt_path)
  {
  fprintf(stderr, "The call to in_remote_checkpoint_dir needs to be mocked!!\n");
  exit(1);
  }

int mom_open_socket_to_jobs_server_with_retries(mom_job *pjob, const char *caller_id, void *(*message_handler)(void *), int retry_limit)
  {
  return(0);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject needs to be mocked!!\n");
  exit(1);
  }

int kill_task(mom_job *pjob, struct task *task, int sig, int pg)
  {
  return(0);
  }

int DIS_tcp_wflush(tcp_chan *chan)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

int attr_atomic_set(struct svrattrl *plist, pbs_attribute *old, pbs_attribute *new_attr, attribute_def *pdef, int limit, int unkn, int privil, int *badattr)
  {
  fprintf(stderr, "The call to attr_atomic_set needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

int diswui(struct tcp_chan *chan, unsigned value)
  {
  fprintf(stderr, "The call to diswui needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_ReqHdr(struct tcp_chan *chan, int reqt, char *user)
  {
  fprintf(stderr, "The call to encode_DIS_ReqHdr needs to be mocked!!\n");
  exit(1);
  }

int mom_set_limits(mom_job *pjob, int set_mode)
  {
  fprintf(stderr, "The call to mom_set_limits needs to be mocked!!\n");
  exit(1);
  }

int open_std_file(mom_job *pjob, enum job_file which, int mode, gid_t exgid)
  {
  fprintf(stderr, "The call to open_std_file needs to be mocked!!\n");
  exit(1);
  }

int mkdirtree(char *dirpath, mode_t mode)
  {
  fprintf(stderr, "The call to mkdirtree needs to be mocked!!\n");
  exit(1);
  }

struct passwd * getpwnam_ext(char **user_buf, const char * user_name)
  {
  fprintf(stderr, "The call to getpwnam_ext needs to be mocked!!\n");
  exit(1);
  }

int remtree(char *dirname)
  {
  fprintf(stderr, "The call to remtree needs to be mocked!!\n");
  exit(1);
  }

int init_groups(char *pwname, int pwgrp, int groupsize, int *groups)
  {
  fprintf(stderr, "The call to init_groups needs to be mocked!!\n");
  exit(1);
  }

int tcp_connect_sockaddr(struct sockaddr *sa, size_t sa_size, bool use_log)
  {
  fprintf(stderr, "The call to tcp_connect_sockaddr needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

int TTmpDirName(mom_job *pjob, char *tmpdir, int tmpdir_size)
  {
  fprintf(stderr, "The call to TTmpDirName needs to be mocked!!\n");
  exit(1);
  }

int is_joined(mom_job *pjob)
  {
  fprintf(stderr, "The call to is_joined needs to be mocked!!\n");
  exit(1);
  }

int job_unlink_file(mom_job *pjob, const char *name)
  {
  fprintf(stderr, "The call to job_unlink_file needs to be mocked!!\n");
  exit(1);
  }

int MUSleep(long SleepDuration)
  {
  fprintf(stderr, "The call to MUSleep needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }

void mom_deljob(mom_job *pjob)
  {
  fprintf(stderr, "The call to mom_deljob needs to be mocked!!\n");
  exit(1);
  }

mom_job *mom_find_job(char const *jobid)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

int im_compose(tcp_chan *chan, const char *jobid, const char *cookie, int command, tm_event_t event, tm_task_id taskid)
  {
  fprintf(stderr, "The call to im_compose needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_ReqExtend(struct tcp_chan *chan, char *extend)
  {
  fprintf(stderr, "The call to encode_DIS_ReqExtend needs to be mocked!!\n");
  exit(1);
  }

void list_move(tlist_head *from, tlist_head *to)
  {
  fprintf(stderr, "The call to list_move needs to be mocked!!\n");
  exit(1);
  }

int diswsi(tcp_chan *chan, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }

mom_job *mom_job_alloc(void)
  {
  fprintf(stderr, "The call to job_alloc needs to be mocked!!\n");
  exit(1);
  }

int kill_job(mom_job *pjob, int sig, const char *killer_id_name, const char *why_killed_reason)
  {
  fprintf(stderr, "The call to kill_job needs to be mocked!!\n");
  exit(1);
  }

void reply_text(struct batch_request *preq, int code, const char *text)
  {
  fprintf(stderr, "The call to reply_text needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

int TMOMJobGetStartInfo(

  mom_job         *pjob, /* I */
  pjobexec_t **TJEP) /* O */

  {
  return(0);
  }

int destroy_alps_reservation(

  const char *reservation_id,
  char *apbasil_path,
  char *apbasil_protocol,
  int   retries)

  {
  return(0);
  }

int setuid_ext(uid_t uid, int set_euid)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int type, const char *func_name, const char *msg, int o) {}

bool am_i_mother_superior(const mom_job &pjob)
  {
  return(false);
  }

int put_env_var(const char *name, const char *value)
  {
  return(0);
  }

int attr_to_str(std::string &ds, attribute_def *at_def, pbs_attribute attr, bool XML)
  {
  return(0);
  }

void power_state::set_power_state(int power_state)

  {
  }

power_state::power_state()

  {
  }

bool power_state::is_valid_power_state(int power_state)
  {
  return false;
  }

const char *sys_file::get_last_error_string()
  {
  return NULL;
  }

int terminate_sisters(mom_job *pjob, int sig)
  {
  return 0;
  }

void free_pwnam(struct passwd *pwdp, char *buf)
  {}

void free_grname(

  struct group *grp,
  char         *user_buf)

  {
  if (user_buf)
    {
    free(user_buf);
    user_buf = NULL;
    }

  if (grp)
    {
    free(grp);
    grp = NULL;
    }

  }


struct group *getgrnam_ext(

  char **user_buf,
  const char *grp_name) /* I */

  {
  struct group *grp;
  char  *buf;
  long   bufsize;
  struct group *result;
  int rc;

  *user_buf = NULL;
  if (grp_name == NULL)
    return(NULL);

  bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
  if (bufsize == -1)
    bufsize = 8196;

  buf = (char *)malloc(bufsize);
  if (buf == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "failed to allocate memory");
    return(NULL);
    }

  int alloc_size = sizeof(struct group);
  grp = (struct group *)calloc(1, alloc_size);
  if (grp == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "could not allocate passwd structure");
    free(buf);
    return(NULL);
    }

  rc = getgrnam_r(grp_name, grp, buf, bufsize, &result);
  if ((rc) ||
      (result == NULL))
    {
    /* See if a number was passed in instead of a name */
    if (isdigit(grp_name[0]))
      {
      rc = getgrgid_r(atoi(grp_name), grp, buf, bufsize, &result);
      if ((rc == 0) &&
          (result != NULL))
        {
        *user_buf = buf;
        return(grp);
        }
      }

    sprintf(buf, "getgrnam_r failed: %d", rc);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);

    free(buf);
    free(grp);

    return (NULL);
    }

  *user_buf = buf;
  return(grp);
  } /* END getgrnam_ext() */

int pbs_disconnect_socket(

  int sock)  /* I (socket descriptor) */

  {
  return(0);
  }

void set_jobs_substate(

  mom_job *pjob,
  int  new_substate)

  {
  }

int send_job_obit(mom_job *pjob, int exit_status)
  {
  return(PBSE_NONE);
  }

batch_request::batch_request(int type) : rq_type(type) {}

job::job() 
  
  {
  memset(&this->ji_qs, 0, sizeof(this->ji_qs));
  }

job::~job() {}

mom_job::mom_job() : ji_tasks()
  {
  }
mom_job::~mom_job() {}

unsigned int job::get_exgid() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_exgid);
  }

unsigned int job::get_exuid() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_exuid);
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

struct array_strings *job::get_arst_attr(int index)
  {
  return(this->ji_wattr[index].at_val.at_arst);
  }
