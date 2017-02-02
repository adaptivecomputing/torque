#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "resource.h" /* resource */
#include "log.h" /* LOG_BUF_SIZE */
#include "pbs_job.h" /* job, tm_task_id, task */
#include "pbs_ifl.h" /* attrl */
#include "libpbs.h" /* job_file */
#include "mom_func.h" /* startjob_rtn */


char log_buffer[LOG_BUF_SIZE];
int is_login_node = FALSE;
char *apbasil_path = NULL;
char *apbasil_protocol = NULL;
int lockfds; /* mom_main */
char *path_jobs; /* mom_main.c */
int multi_mom = 1; /* mom_main.c */
int svr_resc_size = 0; /* resc_def_all.c */
char *TRemChkptDirList[1]; /* mom_main.c */
int exiting_tasks; /* mom_main.c */
resource_def *svr_resc_def; /* resc_def_all.c */
char *path_spool; /* mom_main.c */
int pbs_rm_port; /* mom_main.c */
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */ /* mom_main.c/pbsd_main.c */
bool connect_fail;

int mom_job_save(mom_job *pjob, int mom_port)
  {
  return(0);
  }

task *task_find(mom_job *pjob, tm_task_id taskid)
 {
 fprintf(stderr, "The call to task_find needs to be mocked!!\n");
 exit(1);
 }

task *pbs_task_create(mom_job *pjob, tm_task_id taskid)
 {
 fprintf(stderr, "The call to pbs_task_create needs to be mocked!!\n");
 exit(1);
 }

int pbs_connect(char *server_name_ptr)
 {
 if (connect_fail == true)
   return(-1);
 else
   return(1);
 }

int pbs_disconnect(int connect)
 {
 fprintf(stderr, "The call to pbs_disconnect needs to be mocked!!\n");
 exit(1);
 }

pid_t fork_me(int conn)
 {
 fprintf(stderr, "The call to fork_me needs to be mocked!!\n");
 exit(1);
 }

extern "C"
{
void set_attr(struct attrl **attrib, const char *attrib_name, const char *attrib_value)
 {
 fprintf(stderr, "The call to set_attr needs to be mocked!!\n");
 exit(1);
 }
}

int mach_checkpoint(struct task *tsk, char *path, int abt)
 {
 fprintf(stderr, "The call to mach_checkpoint needs to be mocked!!\n");
 exit(1);
 }

char *mk_dirs(const char *base)
 {
 fprintf(stderr, "The call to mk_dirs needs to be mocked!!\n");
 exit(1);
 }

int message_job(mom_job *pjob, enum job_file jft, char *text)
 {
 fprintf(stderr, "The call to message_job needs to be mocked!!\n");
 exit(1);
 }

void reply_ack(struct batch_request *preq)
 {
 fprintf(stderr, "The call to reply_ack needs to be mocked!!\n");
 exit(1);
 }

int mom_does_checkpoint()
 {
 fprintf(stderr, "The call to mom_does_checkpoint needs to be mocked!!\n");
 exit(1);
 }

int decode_str(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
 {
 fprintf(stderr, "The call to decode_str needs to be mocked!!\n");
 exit(1);
 }

int pbs_rlsjob_err(int c, const char *jobid, const char *holdtype, char *extend, int *local_error)
 {
 return(0);
 }


int pbs_rlsjob(int c, char *jobid, char *holdtype, char *extend)
 {
 fprintf(stderr, "The call to pbs_rlsjob needs to be mocked!!\n");
 exit(1);
 }

void free_br(struct batch_request *preq)
 {
 fprintf(stderr, "The call to free_br needs to be mocked!!\n");
 exit(1);
 }

int pbs_alterjob(int c, char *jobid, struct attrl *attrib, char *extend)
 {
 fprintf(stderr, "The call to pbs_alterjob needs to be mocked!!\n");
 exit(1);
 }

char * csv_find_value(const char *csv_str, const char *search_str)
 {
 fprintf(stderr, "The call to csv_find_value needs to be mocked!!\n");
 exit(1);
 }

int write_nodes_to_file(mom_job *pjob)
 {
 fprintf(stderr, "The call to write_nodes_to_file needs to be mocked!!\n");
 exit(1);
 }

int set_job(mom_job *mom_job, struct startjob_rtn *rtn)
 {
 fprintf(stderr, "The call to set_job needs to be mocked!!\n");
 exit(1);
 }

void net_close(int but)
 {
 fprintf(stderr, "The call to net_close needs to be mocked!!\n");
 exit(1);
 }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
 {
 fprintf(stderr, "The call to req_reject needs to be mocked!!\n");
 exit(1);
 }

int chk_file_sec(const char *path, int isdir, int sticky, int disallow, int fullpath, char *SEMsg)
 {
 fprintf(stderr, "The call to chk_file_sec needs to be mocked!!\n");
 exit(1);
 }

void log_close(int msg)
 {
 fprintf(stderr, "The call to log_close needs to be mocked!!\n");
 exit(1);
 }

void *get_next(list_link pl, char *file, int line)
 {
 fprintf(stderr, "The call to get_next needs to be mocked!!\n");
 exit(1);
 }

long mach_restart(struct task *tsk, char *path)
 {
 fprintf(stderr, "The call to mach_restart needs to be mocked!!\n");
 exit(1);
 }

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
 {
 fprintf(stderr, "The call to find_resc_def needs to be mocked!!\n");
 exit(1);
 }

int remtree(char *dirname)
 {
 fprintf(stderr, "The call to remtree needs to be mocked!!\n");
 exit(1);
 }

char * csv_find_string(const char *csv_str, const char *search_str)
 {
 fprintf(stderr, "The call to csv_find_string needs to be mocked!!\n");
 exit(1);
 }

int write_gpus_to_file(mom_job *pjob)
 {
 fprintf(stderr, "The call to write_gpus_to_file needs to be mocked!!\n");
 exit(1);
 }

int site_mom_prerst(mom_job *pjob)
 {
 fprintf(stderr, "The call to site_mom_prerst needs to be mocked!!\n");
 exit(1);
 }

int site_mom_postchk(mom_job *pjob, int hold_type)
 {
 fprintf(stderr, "The call to site_mom_postchk needs to be mocked!!\n");
 exit(1);
 }

int task_save(task *ptask)
 {
 fprintf(stderr, "The call to task_save needs to be mocked!!\n");
 exit(1);
 }

char *pbs_strerror(int err)
 {
 fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
 exit(1);
 }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
 {
 fprintf(stderr, "The call to find_resc_entry needs to be mocked!!\n");
 exit(1);
 }

int kill_job(mom_job *pjob, int sig, const char *killer_id_name, const char *why_killed_reason)
 {
 fprintf(stderr, "The call to kill_job needs to be mocked!!\n");
 exit(1);
 }



int pbs_rlsjob_err(int c, char *jobid, char *holdtype, char *extend, int *local_errno)
 {
 fprintf(stderr, "The call to pbs_rlsjob_err needs to be mocked!!\n");
 exit(1);
 }

int pbs_alterjob_err(int c, char *jobid, struct attrl *attrib, char *extend, int *local_errno)
 {
 fprintf(stderr, "The call to pbs_alterjob_err needs to be mocked!!\n");
 exit(1);
 }

int create_alps_reservation(

  const char *exec_hosts,
  const char *username,
  const char *jobid,
  char       *apbasil_path,
  char       *apbasil_protocol,
  long long   pagg_id_value,
  int         use_nppn,
  int         nppcu,
  int         mppdepth,
  char      **reservation_id,
  const char *mppnodes,
  std::string& cray_frequency)

  {
  return(0);
  }

int write_attr_to_file(

  mom_job  *pjob,
  int   index,
  const char *suffix)

  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int type, const char *func_name, const char *msg, int o) {}

int socket_to_handle(int sock, int *my_err)
  {
  if (connect_fail == true)
    return(-1);
  else
    return(5);
  }

int mom_open_socket_to_jobs_server(mom_job *pjob, const char *caller, void *(*message_handler)(void *))
  {
  return(0);
  }

std::string get_frequency_request(struct cpu_frequency_value *pfreq)
  {
  std::string ret = "";
  return ret;
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

unsigned int job::get_exgid() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_exgid);
  }

unsigned int job::get_exuid() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_exuid);
  }

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


