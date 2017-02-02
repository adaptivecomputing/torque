#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h> /* gid_t, uid_t */
#include <pbs_config.h>

#include "attribute.h" /* attribute_def, pbs_attribute */
#include "list_link.h" /* tlist_head, list_link */
#include "log.h" /* LOG_BUF_SIZE */
#include "pbs_ifl.h" /* MAXPATHLEN */
#include "net_connect.h" /* pbs_net_t */
#include "resource.h" /* resource_def */
#include "server_limits.h" /* pbs_net_t. Also defined in net_connect.h */
#include "pbs_job.h" /* job_file_delete_info */
#include "threadpool.h"
#include "machine.hpp"
#include "mom_func.h"

std::list<mom_job *> alljobs_list;
threadpool_t *request_pool;
int is_login_node = 0;
char *apbasil_path = NULL;
char *apbasil_protocol = NULL;
char *path_jobs; /* mom_main.c */
int multi_mom = 1; /* mom_main.c */
attribute_def job_attr_def[10]; /* job_attr_def.c */
char *path_aux; /* mom_main.c */
gid_t pbsgroup; /* mom_main.c */
int thread_unlink_calls; /* mom_main.c */
uid_t pbsuser; /* mom_main.c */
char tmpdir_basename[MAXPATHLEN]; /* mom_main.c */
char *msg_err_purgejob; /* pbs_messages.c */
int pbs_rm_port; /* mom_main.c */
tlist_head svr_alljobs; /* mom_main.c */
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */ /* mom_main.c/pbsd_main.c */
char log_buffer[LOG_BUF_SIZE]; /* pbs_log.c */
bool exit_called = false;
int svr_resc_size = 0; /* resc_def_all.c */
resource_def *svr_resc_def = NULL; /* resc_def_all.c */
pthread_mutex_t *delete_job_files_mutex;
time_t time_now;
int    saved_job;

#ifdef ENABLE_PMIX
char  mom_alias[PBS_MAXHOSTNAME + 1];

char *get_job_envvar(

  mom_job  *pjob,     /* I */
  const char *variable) /* I */

  {
  return(NULL);
  }

int TTmpDirName(

  mom_job  *pjob,   /* I */
  char *tmpdir, /* O */
  int   tmpdir_size)

  {
  return(0);
  }

void translate_vector_to_range_string(

  std::string            &range_string,
  const std::vector<int> &indices)

  {
  }
#endif


void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  }

pbs_net_t get_hostaddr(int *local_errno, const char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr needs to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  }

void log_record(int eventtype, int objclass, const char *objname, const char *text)
  {
  }

int enqueue_threadpool_request(void *(*func)(void *),void *arg, threadpool_t *tp)
  {
  fprintf(stderr, "The call to enqueue_threadpool_request needs to be mocked!!\n");
  exit(1);
  }

void log_ext(int errnum, const char *routine, const char *text, int severity)
  {
  }

void MOMCheckRestart(void)
  {
  }

int client_to_svr(pbs_net_t hostaddr, unsigned int port, int local_port, char *EMsg)
  {
  fprintf(stderr, "The call to client_to_svr needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  return(NULL);
  }

void nodes_free(mom_job *pj)
  {
  }

void mom_checkpoint_delete_files(job_file_delete_info *jfdi)
  {
  }

void log_err(int errnum, const char *routine, const char *text)
  {
  }

void close_conn(int sd, int has_mutex)
  {
  }

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

int destroy_alps_reservation(

  const char *reservation_id,
  char *apbasil_path,
  char *apbasil_protocol,
  int   retries)

  {
  return(0);
  }

resource_def *find_resc_def(

  resource_def *rscdf, /* address of array of resource_def structs */
  const char  *name, /* name of resource */
  int           limit) /* number of members in resource_def array */
 
  {
  return(NULL);
  } 

resource *find_resc_entry(

  pbs_attribute *pattr,  /* I */
  resource_def  *rscdf)  /* I */

  {
  return(NULL);
  } 

int setuid_ext(uid_t uid, int set_euid)
  {
  return(0);
  }

int delete_cpuset(const char *, bool)
  {
  return 0;
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  exit(1);
  }

void *trq_cg_remove_process_from_accts(void *vp)  
  {
  return(PBSE_NONE);
  }

void log_event(

  int         eventtype,
  int         objclass,
  const char *objname,
  const char *text)
  
  {
  return;
  }

void Machine::free_job_allocation(const char *job_id)
  {
  }

Machine::Machine() {}
Machine::~Machine() {}

PCI_Device::~PCI_Device() {}

Socket::~Socket() {}

Chip::~Chip() {}

Core::~Core() {}

Machine          this_node;

void trq_cg_delete_job_cgroups(
        
  const char *job_id,
  bool        successfully_created)

  {
  }

int unlink_ext(const char *filename, int retry_limit)
  {
  return(0);
  }

int rmdir_ext(const char *dirname, int retry_limit)
  {
  return(0);
  }

void send_update_soon()
  {
  return;
  }

int mom_job_save(

  mom_job *pjob,  /* pointer to job structure */
  int  mom_port)   /* if 0 ignore otherwise append to end of job name. this is for multi-mom mode */

  {
  saved_job++;
  return(0);
  }

job::job() 
  
  {
  memset(&this->ji_qs, 0, sizeof(this->ji_qs));
  }

job::~job() {}

mom_job::mom_job() : ji_nodeid(0), ji_tasks()
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

