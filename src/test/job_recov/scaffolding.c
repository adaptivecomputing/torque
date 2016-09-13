#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h> /* pthread_mutex_t */

#include "attribute.h" /* attribute_def, pbs_attribute */
#include "pbs_job.h" /* job */
#include "array.h" /* job_array */
#include "mutex_mgr.hpp"
#include "net_connect.h" /* pbs_net_t */
#include "user_info.h"
#include "server.h" /* server */
#include "sched_cmds.h"
#include "threadpool.h"
#include "id_map.hpp"
#include "completed_jobs_map.h"
#include "pbs_nodes.h"

const char *text_name              = "text";
const char *PJobSubState[10];
const char *PJobState[] = {"hi", "hello"};
const char *path_jobs = "";
pthread_mutex_t *setup_save_mutex = NULL;
int LOGLEVEL=0;
bool exit_called = false;
pthread_mutex_t job_log_mutex = PTHREAD_MUTEX_INITIALIZER;
all_jobs array_summary;
const char *msg_daemonname = "unset";
char path_checkpoint[MAXPATHLEN + 1];
user_info_holder users;
char *job_log_file = NULL;
all_jobs newjobs;
const char *pbs_o_host = "PBS_O_HOST";
pthread_mutex_t *scheduler_sock_jobct_mutex;
int queue_rank = 0;
char *path_spool;
struct server server;
int scheduler_sock=0;
int  svr_do_schedule = SCH_SCHEDULE_NULL;
int listener_command = SCH_SCHEDULE_NULL;
char *path_jobinfo_log;
pthread_mutex_t *svr_do_schedule_mutex;
pthread_mutex_t *listener_command_mutex;
threadpool_t    *task_pool;
bool ghost_array_recovery = false;
bool cray_enabled = false;

completed_jobs_map_class completed_jobs_map;

int aborted;


ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  return(PBSE_NONE);
  }

int save_attr(struct attribute_def *padef, struct pbs_attribute *pattr, int numattr, int fds, char *buf, size_t *buf_remaining, size_t buf_size)
  {
  return(PBSE_NONE);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  return(PBSE_NONE);
  }

job_array *get_array(const char *id)
  {
  return(NULL);
  }

int job_qs_upgrade(job *pj, int fds, char *path, int version)
  {
  return(PBSE_NONE);
  }


void array_get_parent_id(char *job_id, char *parent_id)
  {
  strcpy(parent_id, "4[].napali");
  }

int lock_ss()
  {
  return(0);
  }

int unlock_ss()
  {
  return(0);
  }

int write_buffer(char *buf, int len, int fds)
  {
  return(0);
  }

int add_to_ms_list(char *node_id, job *pjob)
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int unlock_ai_mutex(job_array *pa, const char *func_id, const char *msg, int logging)
  {
  return(0);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

int enqueue_threadpool_request(void *(*func)(void *),void *arg, threadpool_t *tp)
  {
  return(0);
  }

mutex_mgr::mutex_mgr(pthread_mutex_t *, bool a)
  {
  }

int mutex_mgr::unlock()
  {
  return(0);
  }

void mutex_mgr::mark_as_locked() {}

mutex_mgr::~mutex_mgr() {}

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  return(PBSE_NONE);
  }

int svr_enquejob(job *pjob, int has_sv_qs_mutex, const char *prev_id, bool reservation, bool recov)
  {
  return(PBSE_NONE);
  }

char *get_variable(job *pjob, const char *variable)
  {
  return(strdup("napali"));
  }

int safe_strncat(

  char   *str,
  const char   *to_append,
  size_t  space_remaining)

  {
  size_t len = strlen(to_append);

  /* not enough space */
  if (space_remaining < len)
    return(-1);
  else
    strcat(str, to_append);

  return(PBSE_NONE);
  } /* END safe_strncat() */

void free_server_attrs(tlist_head *attrl_ptr) {}
struct batch_request *setup_cpyfiles(struct batch_request *preq, job *pjob, char *from, char *to, int direction, int tflag) {return NULL;}
char *pbs_default(void) {return NULL;}
pbs_net_t get_connectaddr(int sock, int mutex) {return -1;}
void set_chkpt_deflt(job *pjob, pbs_queue *pque) {}

int attr_to_str(std::string& ds, attribute_def *attr_def,struct pbs_attribute attr, bool XML)
  {
  if (attr_def->at_type == ATR_TYPE_STR)
    ds = attr.at_val.at_str;
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int errnum, const char *routine, const char *text, int severity){}
void account_record(int acctype, job *pjob, const char *text){}
const char *prefix_std_file(job *pjob, std::string& ds, int key) {return "";}
job *svr_find_job(const char *jobid, int get_subjob) {return NULL;}
const char *add_std_filename(job *pjob, char *path, int key, std::string& ds) { return ""; }
int lock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string) {return(0);}
struct pbs_queue *lock_queue_with_job_held(struct pbs_queue  *pque, job       **pjob_ptr){return(NULL);}
pbs_net_t get_hostaddr(int *local_errno, const char *hostname) {return 0;}
void svr_mailowner(job *pjob, int mailpoint, int force, const char *text) {}
pbs_queue *get_dfltque(void) {return NULL;}
int log_job_record(const char *buf){return 0;}
int comp_size(struct pbs_attribute *attr, struct pbs_attribute *with) {return 0;}
int comp_l(struct pbs_attribute *attr, struct pbs_attribute *with) {return 0;}
void svr_evaljobstate(job &pjob, int &newstate, int &newsub, int forceeval) {}
int encode_unkn(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm) {return 0;}
int set_unkn(struct pbs_attribute *old, struct pbs_attribute *new_attr, enum batch_op op) {return 0;}
int decode_time(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm) {return 0;}
int comp_b(struct pbs_attribute *attr, struct pbs_attribute *with) {return 0;}
void issue_track(job *pjob) {}
int unlock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string) {return(0);}
int decode_size(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm) {return 0;}
int set_size(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op){return 0;}
pbs_queue *find_queuebyname(const char *quename) {return NULL;}
void check_job_log(struct work_task *ptask) {}
int comp_unkn(struct pbs_attribute *attr, struct pbs_attribute *with) {return 0;}
int unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging){return 0;}
int svr_chkque(job *pjob, pbs_queue *pque, char *hostname, int mtype, char *EMsg) {return 0;}
int lock_ji_mutex(job *pjob, const char *id, const char *msg, int logging) {return 0;}
int setup_array_struct(job *pjob) {return 0;}
int remove_job(all_jobs *aj, job *pjob, bool b) {return 0;}
job *next_job(all_jobs *aj, all_jobs_iterator *iter) {return NULL;}
int  can_queue_new_job(char *user_name, job *pjob) {return 0;}
struct work_task *set_task(enum work_type type, long event_id, void (*func)(work_task *), void *parm, int get_lock) {return NULL;}
void reply_ack(struct batch_request *preq) {}
int job_log_open(char *filename, char *directory) {return 0;}
char *threadsafe_tokenizer(char **str, const char *delims) {return NULL;}
int set_ll(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op) {return 0;}
int set_l(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op) {return 0;}

int array_delete(const char *array_id) 
  {
  return(PBSE_NONE);
  }

int array_save(job_array *pa) {return 0;}
int reply_jobid(struct batch_request *preq, char *jobid, int which) {return 0;}
void mutex_mgr::set_unlock_on_exit(bool val) {}
int client_to_svr(pbs_net_t hostaddr, unsigned int port, int local_port, char *EMsg) {return 0;}
int issue_signal(job **pjob_ptr, const char *signame, void (*func)(struct batch_request *), void *extra, char *extend) {return 0;}
int get_jobs_index(all_jobs *aj, job *pjob) {return(0);}
int insert_job(all_jobs *aj, job *pjob) {return 0;}
int encode_time(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm) {return 0;}
int svr_authorize_jobreq(struct batch_request *preq, job *pjob) {return 0;}
int decode_ll(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm) {return(0);}
struct pbsnode *find_nodebyname(const char *nodename) {return(NULL);}
void free_br(struct batch_request *preq) {}
int job_route(job *jobp) {return 0;}
int svr_dequejob(job *pjob, int val) {return 0;}
int encode_ll(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm) {return 0;}
int set_b(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op) {return 0;}
int insert_into_recycler(job *pjob) {return 0;}
int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg) {return 0;}
int svr_save(struct server *ps, int mode) {return 0;}
int encode_l(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm) {return 0;}
int mutex_mgr::lock(){return 0;}
int  increment_queued_jobs(user_info_holder *uih, char *user_name, job *pjob) {return 0;}
int relay_to_mom(job **pjob_ptr, batch_request   *request, void (*func)(struct work_task *)) {return 0;}
int  decrement_queued_jobs(user_info_holder *uih, char *user_name, job *pjob) {return 0;}
int req_runjob(batch_request *preq) {return(0);}
void reply_badattr(int code, int aux, svrattrl *pal, struct batch_request *preq) {}
void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg) {}
void free_unkn(pbs_attribute *pattr) {}
int encode_b(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm) {return 0;}
int decode_tokens(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm) {return 0;}
int encode_size(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm) {return 0;}
int set_hostacl(pbs_attribute *attr, pbs_attribute *new_host, enum batch_op  op) {return 0;}
int set_rcost (pbs_attribute * attr, pbs_attribute * new_attr, enum batch_op){return 0;}
void free_rcost (pbs_attribute * attr) {}
int servername_chk(pbs_attribute *pattr, void *pobject, int actmode) {return 0;}
int set_uacl(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op) {return 0;}
int extra_resc_chk(pbs_attribute *pattr, void *pobject, int actmode) {return 0;}
int decode_rcost (pbs_attribute * patr, const char *name, const char *rn, const char *val, int perm) {return 0;}
int decode_unkn(pbs_attribute *patr, const char *name, const char *rescn, const char *value, int perm) {return 0;}
int token_chk(pbs_attribute *pattr, void *pobject, int actmode) {return 0;}
int schiter_chk(pbs_attribute *pattr, void *pobject, int actmode) {return 0;}
int encode_rcost(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm) {return 0;}
int manager_oper_chk(pbs_attribute *pattr, void *pobject, int actmode) {return 0;}
void restore_attr_default(struct pbs_attribute *attr) {}
int set_nextjobnum(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op) {return 0;}
int  decode_l(pbs_attribute *patr, const char *name, const char *rn, const char *val, int) {return 0;}
void free_extraresc (pbs_attribute * attr){}
int set_tokens(pbs_attribute *attr, pbs_attribute *newAttr, enum batch_op op){return 0;}
int comp_ll(struct pbs_attribute *attr, struct pbs_attribute *with) {return 0;}
int  decode_b(pbs_attribute *patr, const char *name, const char *rn, const char *val, int) {return 0;}
int nextjobnum_chk(pbs_attribute *pattr, void *pobject, int actmode) {return 0;}
struct batch_request *alloc_br(int type) {return NULL;}
int svr_chk_owner(struct batch_request *preq, job *pjob) {return 0;}
int comp_checkpoint(pbs_attribute *attr, pbs_attribute *with) {return 0;}
batch_request *get_remove_batch_request(char *br_id) {return NULL;}
long calc_job_cost(job *pjob) {return(0);}
int issue_to_svr(const char *servern, struct batch_request **preq, void (*replyfunc)(struct work_task *)) {return 0;}
int que_to_local_svr(struct batch_request *preq) {return 0;}
int job_set_wait(pbs_attribute *pattr, void *pjob, int mode) {return 0;}
int get_batch_request_id(batch_request *preq) {return 0;}
int encode_inter(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm) {return 0;}


job *find_job_by_array(all_jobs *aj, const char *job_id, int get_subjob, bool locked)
  {
  return(NULL);
  }

id_map::id_map() 
  {
  }

id_map::~id_map() {}

int id_map::get_new_id(const char *job_name)
  {
  static int id = 0;

  return(id++);
  }

const char *id_map::get_name(int internal_job_id)
  {
  return("1.napali");
  }

id_map job_mapper;

char *get_correct_jobname(const char *id)
  {
  return(strdup(id));
  }

int encode_complete_req(
    
  pbs_attribute *attr,
  tlist_head    *phead,
  const char    *atname,
  const char    *rsname,
  int            mode,
  int            perm)

  {
  return(0);
  }

int  decode_complete_req(
    
  pbs_attribute *patr,
  const char    *name,
  const char    *rescn,
  const char    *val,
  int            perm)

  {
  return(0);
  }

int comp_complete_req(
   
  pbs_attribute *attr,
  pbs_attribute *with)

  {
  return(0);
  } // END comp_complete_req()

void free_complete_req(

  pbs_attribute *patr) {}

int set_complete_req(
    
  pbs_attribute *attr,
  pbs_attribute *new_attr,
  enum batch_op  op)
  
  {
  return(0);
  }
void handle_complete_second_time(struct work_task *ptask)
  {
  }

completed_jobs_map_class::completed_jobs_map_class() {}
completed_jobs_map_class::~completed_jobs_map_class() {}
bool completed_jobs_map_class::add_job(char const* s, time_t t) {return false;}

std::string get_path_jobdata(const char *a, const char *b) {return "";}

void add_to_completed_jobs(work_task *wt) {}

int pbsnode::unlock_node(const char *id, const char *msg, int level)
  {
  return(0);
  }

int update_user_acls(

  pbs_attribute *pattr,
  void          *pobject,
  int            actmode)

  {
  return(0);
  }

const char *pbsnode::get_name() const
  {
  return(this->nd_name.c_str());
  }

int update_group_acls(

  pbs_attribute *pattr,
  void          *pobj,
  int            actmode)

  {
  return(0);
  }

int node_exception_check(

  pbs_attribute *pattr,
  void          *pobject,
  int            actmode)

  {
  return(0);
  }

job::job() 
  {
  memset(this->ji_wattr, 0, sizeof(this->ji_wattr));
  }

job::~job() {}

int node_avail_complex(

  char *spec,   /* I - node spec */
  int  *navail, /* O - number available */
  int  *nalloc, /* O - number allocated */
  int  *nresvd, /* O - number reserved  */
  int  *ndown)  /* O - number down      */

  {
  return(0);
  }

int lock_ai_mutex(

  job_array  *pa,
  const char *id,
  const char *msg,
  int        logging)

  {
  return(0);
  }

int insert_array(

  job_array *pa)

  {
  return(0);
  }

void clear_attr(

  pbs_attribute *pattr, /* O */
  attribute_def *pdef)  /* I */

  {
  memset(pattr, 0, sizeof(pbs_attribute));

  pattr->at_type = pdef->at_type;

  if ((pattr->at_type == ATR_TYPE_RESC) ||
      (pattr->at_type == ATR_TYPE_LIST))
    {
    CLEAR_HEAD(pattr->at_val.at_list);
    }

  return;
  }  /*END clear_attr() */

array_info::array_info() {}

job_array::job_array() : job_ids(NULL), jobs_recovered(0), ai_ghost_recovered(false), uncreated_ids(),
                         ai_mutex(NULL), ai_qs()

  {
  this->ai_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(this->ai_mutex, NULL);
  }

void job_array::update_array_values(

  int                   old_state, /* I */
  enum ArrayEventsEnum  event,     /* I */
  const char           *job_id,
  int                   job_exit_status)

  {
  }

void job_array::set_array_id(

  const char *array_id)

  {
  snprintf(this->ai_qs.parent_id, sizeof(this->ai_qs.parent_id), "%s", array_id);
  }

void job_array::set_arrays_fileprefix(

  const char *file_prefix)

  {
  snprintf(this->ai_qs.fileprefix, sizeof(this->ai_qs.fileprefix), "%s", file_prefix);
  }

void job_array::set_owner(

  const char *owner)

  {
  snprintf(this->ai_qs.owner, sizeof(this->ai_qs.owner), "%s", owner);
  }

bool job_array::is_deleted() const
  {
  return(this->being_deleted);
  }
