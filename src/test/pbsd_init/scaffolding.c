#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h> /* pthread_mutex_t */
#include <limits.h> /* _POSIX_PATH_MAX */

#include "pbs_config.h"
#include "pbs_nodes.h" /* all_nodes, pbsnode */
#include "attribute.h" /* attribute_def, pbs_attribute, batch_op */
#include "pbs_ifl.h" /* PBS_MAXSERVERNAME, MAXPATHLEN */
#include "list_link.h" /* tlist_head */
#include "resource.h" /* resource_def */
#include "threadpool.h" /* threadpool_t */
#include "server.h" /* server */
#include "pbs_job.h" /* all_jobs, job */
#include "work_task.h" /* all_tasks, work_task, work_type */
#include "array.h" /* job_array, ArrayEventsEnum */
#include "batch_request.h" /* batch_request */
#include "net_connect.h" /* pbs_net_t */
#include "queue.h" /* all_queues, pbs_queue */
#include "user_info.h"
#include "id_map.hpp"
#include "mom_hierarchy_handler.h"
#include "machine.hpp"
#include "queue.h"
#include "track_alps_reservations.hpp"

bool  use_path_home = false;
char *path_pbs_environment;
char *path_node_usage;
threadpool_t *task_pool;
int paused;
int scheduler_sock=0;
int scheduler_jobct = 0;
bool auto_send_hierarchy = true;
mom_hierarchy_t *mh;
pthread_mutex_t job_log_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t *reroute_job_mutex;
pthread_mutex_t log_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
const char *msg_init_chdir = "unable to change to directory %s";
char *path_jobs;
const char *msg_err_noqueue = "Unable to requeue job, queue is not defined";
all_nodes allnodes;
char *path_nodestate;
char *path_priv = NULL;
const char *msg_init_exptjobs = "Expected %d, recovered %d jobs";
const char *msg_daemonname = "unset";
char *path_track;
char *path_checkpoint;
char *job_log_file = NULL;
char *plogenv = NULL;
pthread_mutex_t *node_state_mutex = NULL;
pthread_mutex_t *acctfile_mutex = NULL;
pthread_mutex_t *check_tasks_mutex = NULL;
const char *msg_init_queued = "Requeued in queue: ";
char *path_svrlog;
char path_acct[_POSIX_PATH_MAX];
char *path_nodes;
const char *msg_init_recovque = "Recovered queue %s";
attribute_def job_attr_def[10];
const char *msg_init_expctq = "Expected %d, recovered %d queues";
char *path_arrays;
char *log_file = NULL;
const char *msg_script_open = "Unable to open script file";
all_jobs newjobs;
char server_name[PBS_MAXSERVERNAME + 1];
char *path_nodenote;
all_jobs alljobs;
char *path_queues;
char path_log[MAXPATHLEN + 1];
const char *msg_init_nojobs = "No jobs to open";
tlist_head svr_newnodes;
const char *msg_err_malloc = "malloc failed";
int rpp_dbprt = 0;
char *path_credentials;
const char *msg_init_badjob = "Recover of job %s failed";
char *path_nodes_new;
resource_def *svr_resc_def;
tlist_head svr_requests;
attribute_def que_attr_def[10];
int queue_rank = 0;
char *path_spool;
const char *msg_init_abt = "Job aborted on PBS Server initialization";
const char *msg_init_noqueues = "No queues to open";
char *path_nodenote_new;
char *path_svrdb = NULL;
threadpool_t *request_pool;
const char *msg_init_baddb = "Unable to read server database";
struct server server;
const char *msg_init_substate = "Requeueing job, substate: %d ";
const char *msg_init_unkstate = "Unable to recover job in strange substate: %d";
all_jobs array_summary;
attribute_def svr_attr_def[10];
int a_opt_init = -1;
std::list<timed_task>  *task_list_timed;
pthread_mutex_t task_list_timed_mutex;
char *path_jobinfo_log;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
pthread_mutex_t *svr_requests_mutex = NULL;
all_tasks task_list_event;
char *path_svrdb_new;
const char *path_home = PBS_SERVER_HOME;
char *acct_file = NULL;
const char *path_mom_hierarchy = "/dev/null";
pthread_mutex_t *svr_do_schedule_mutex;
pthread_mutex_t *listener_command_mutex;
pthread_mutex_t *retry_routing_mutex;
user_info_holder users;
id_map job_mapper;
threadpool_t *async_pool;
bool exit_called = false;
char *path_nodepowerstate;
struct pbs_queue *allocd_queue = NULL;

int enque_rc;
int evaluated;
int aborted;
int freed_job_allocation;
int job_state;
int locked_job = 0;
int unlocked_job = 0;

bool dont_find_job;
bool dont_find_node;

char global_log_ext_msg[LOCAL_LOG_BUF_SIZE] = { '\0' };

void on_job_rerun_task(struct work_task *ptask)
  {
  }

void initialize_user_info_holder(user_info_holder *uih) {}

job_array *get_jobs_array(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_array needs to be mocked!!\n");
  exit(1);
  }

int set_old_nodes(job *pjob)
  {
  return(0);
  }

char *parse_servername(const char *name, unsigned int *service)
  {
  fprintf(stderr, "The call to parse_servername needs to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save needs to be mocked!!\n");
  exit(1);
  }

int svr_job_purge(job *pjob, int leaveSpoolFiles)
  {
  fprintf(stderr, "The call to job_purge needs to be mocked!!\n");
  exit(1);
  }

int net_move(job *jobp, struct batch_request *req)
  {
  fprintf(stderr, "The call to net_move needs to be mocked!!\n");
  exit(1);
  }

void svr_mailowner(job *pjob, int mailpoint, int force, const char *text)
  {
  fprintf(stderr, "The call to svr_mailowner needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void set_statechar(job *pjob)
  {
  }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr needs to be mocked!!\n");
  exit(1);
  }

pbs_net_t get_hostaddr(int *local_errno, const char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr needs to be mocked!!\n");
  exit(1);
  }

void on_job_exit_task(struct work_task *ptask)
  {
  }

int job_log_open(char *filename, char *directory)
  {
  fprintf(stderr, "The call to job_log_open needs to be mocked!!\n");
  exit(1);
  }

job *find_array_template(char *arrayid)
  {
  fprintf(stderr, "The call to find_array_template needs to be mocked!!\n");
  exit(1);
  }

struct work_task *apply_job_delete_nanny(struct job *pjob, int delay)
  {
  fprintf(stderr, "The call to apply_job_delete_nanny needs to be mocked!!\n");
  exit(1);
  }

void job_clone_wt(void *cloned_id)
  {
  fprintf(stderr, "The call to job_clone_wt needs to be mocked!!\n");
  exit(1);
  }

void start_request_pool(threadpool_t *tp)
  {
  }

char *pbs_get_server_list(void)
  {
  fprintf(stderr, "The call to pbs_get_server_list needs to be mocked!!\n");
  exit(1);
  }

int csv_length(const char *csv_str)
  {
  fprintf(stderr, "The call to csv_length needs to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(struct work_task *), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task needs to be mocked!!\n");
  exit(1);
  }

char *csv_nth(const char *csv_str, int n)
  {
  fprintf(stderr, "The call to csv_nth needs to be mocked!!\n");
  exit(1);
  }

void initialize_all_nodes_array(all_nodes *an)
  {
  fprintf(stderr, "The call to initialize_all_nodes_array needs to be mocked!!\n");
  exit(1);
  }

job_array *next_array(all_arrays_iterator **iter)
  {
  fprintf(stderr, "The call to next_array needs to be mocked!!\n");
  exit(1);
  }

void initialize_all_tasks_array(all_tasks *at)
  {
  fprintf(stderr, "The call to initialize_all_tasks_array needs to be mocked!!\n");
  exit(1);
  }

int insert_job(all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to insert_job needs to be mocked!!\n");
  exit(1);
  }

int unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void depend_clrrdy(job *pjob)
  {
  fprintf(stderr, "The call to depend_clrrdy needs to be mocked!!\n");
  exit(1);
  }

struct pbsnode *next_host(all_nodes *an, all_nodes_iterator **iter, struct pbsnode *held)
  {
  fprintf(stderr, "The call to next_host needs to be mocked!!\n");
  exit(1);
  }

job *next_job(all_jobs *aj, all_jobs_iterator *iter)
  {
  fprintf(stderr, "The call to next_job needs to be mocked!!\n");
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

int set_arst(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_arst needs to be mocked!!\n");
  exit(1);
  }

int svr_recov_xml(char *svrfile,  int read_only)
  {
  fprintf(stderr, "The call to svr_recov_xml needs to be mocked!!\n");
  exit(1);
  }

void initialize_all_jobs_array(all_jobs *aj)
  {
  fprintf(stderr, "The call to initialize_all_jobs_array needs to be mocked!!\n");
  exit(1);
  }

int job_abt(struct job **pjobp, const char *text, bool b=false)
  {
  aborted++;
  return(0);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

int log_open(char *filename, char *directory)
  {
  fprintf(stderr, "The call to log_open needs to be mocked!!\n");
  exit(1);
  }

int svr_enquejob(job *pjob, int has_sv_qs_mutex, const char *prev_id, bool reservation, bool recovery)
  {
  return(enque_rc);
  }

int initialize_threadpool(threadpool_t **pool, int min_threads, int max_threads, int max_idle_time)
  {
  fprintf(stderr, "The call to initialize_threadpool needs to be mocked!!\n");
  exit(1);
  }

void initialize_all_arrays_array()
  {
  fprintf(stderr, "The call to initialize_all_arrays_array needs to be mocked!!\n");
  exit(1);
  }

void set_resc_assigned(job *pjob, enum batch_op op)
  {
  fprintf(stderr, "The call to set_resc_assigned needs to be mocked!!\n");
  exit(1);
  }

int setup_nodes(void)
  {
  fprintf(stderr, "The call to setup_nodes needs to be mocked!!\n");
  exit(1);
  }

job *job_recov(const char *filename)
  {
  fprintf(stderr, "The call to job_recov needs to be mocked!!\n");
  exit(1);
  }

void initialize_recycler()
  {
  fprintf(stderr, "The call to initialize_recycler needs to be mocked!!\n");
  exit(1);
  }

void update_array_values(job_array *pa, int old_state, enum ArrayEventsEnum event, char *job_id, long job_atr_hold, int job_exit_status)
  {
  fprintf(stderr, "The call to update_array_values needs to be mocked!!\n");
  exit(1);
  }

int array_delete(job_array *pa)
  {
  fprintf(stderr, "The call to array_delete needs to be mocked!!\n");
  exit(1);
  }

pbs_queue *que_recov_xml(char *filename)
  {
  fprintf(stderr, "The call to que_recov_xml needs to be mocked!!\n");
  exit(1);
  }

int init_resc_defs(void)
  {
  fprintf(stderr, "The call to init_resc_defs needs to be mocked!!\n");
  exit(1);
  }

void free_arst(struct pbs_attribute *attr)
  {
  fprintf(stderr, "The call to free_arst needs to be mocked!!\n");
  exit(1);
  }

int decode_arst_direct(struct pbs_attribute *patr,  const char *val)
  {
  fprintf(stderr, "The call to decode_arst_direct needs to be mocked!!\n");
  exit(1);
  }

int setup_env(const char *filen)
  {
  fprintf(stderr, "The call to setup_env needs to be mocked!!\n");
  exit(1);
  }

int array_recov(const char *path, job_array **pa)
  {
  fprintf(stderr, "The call to array_recov needs to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  return(0);
  }

void track_save(struct work_task *pwt)
  {
  fprintf(stderr, "The call to track_save needs to be mocked!!\n");
  exit(1);
  }

void acct_close(bool acct_mutex_locked)
  {
  fprintf(stderr, "The call to acct_close needs to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  if (dont_find_job)
    return(NULL);

  job *pjob = (job *)calloc(1, sizeof(job));
  strcpy(pjob->ji_qs.ji_jobid, jobid);
  pjob->ji_qs.ji_state = job_state;
  return(pjob);
  }

int svr_save(struct server *ps, int mode)
  {
  fprintf(stderr, "The call to svr_save needs to be mocked!!\n");
  exit(1);
  }

int acct_open(char *filename, bool acct_mutex_locked)
  {
  fprintf(stderr, "The call to acct_open needs to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job &pjob, int &newstate, int &newsub, int forceeval)
  {
  evaluated++;
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  return(0);
  }


char *threadsafe_tokenizer(char **str, const char *delims)
  {
  fprintf(stderr, "The call to threadsafe_tokenizer needs to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int get_parent_and_child(char *start, char **parent, char **child, char **end)
  {
  return(0);
  }

int create_partial_pbs_node(char *nodename, unsigned long addr, int perms)
  {
  return(0);
  }

struct pbsnode *find_nodebyname(const char *name)
  {
  if (dont_find_node)
    return(NULL);

  pbsnode *pnode = (pbsnode *)calloc(1, sizeof(pbsnode));
  pnode->nd_name = strdup(name);
  return(pnode);
  }

void initialize_queue_recycler() {}

int set_svr_attr(int attr_index, void *val) 
  {
  return(0);
  }

void initialize_task_recycler() {}

char *trim(char *str)
  {
  return(str);
  }

int array_save(job_array *pa)
  {
  return(0);
  }

int enqueue_threadpool_request(

  void *(*func)(void *),
  void *arg,
  threadpool_t *tp)

  {
  return(0);
  }

void initialize_login_holder() {}

void initialize_alps_reservations() {}

int lock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

int unlock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

void initialize_network_info() {}

struct sockaddr_in *get_cached_addrinfo(
    
  char               *hostname)

  {
  return(NULL);
  }

int insert_addr_name_info(
    
  char               *hostname,
  char               *full_hostname,
  struct sockaddr_in *sai)
  
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  unlocked_job++;
  return(0);
  }

int lock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  locked_job++;
  return(0);
  }

int lock_ai_mutex(job_array *pa, const char *func_id, const char *msg, int logging)
  {
  return(0);
  }

int unlock_ai_mutex(job_array *pa, const char *func_id, const char *msg, int logging)
  {
  return(0);
  }

job_array *get_array(
    
  const char *id)

  {
  return(NULL);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

void poll_job_task(struct work_task *wt) {}

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int eventtype, const char *func_name, const char *msg, int level)
  {
  strncpy(global_log_ext_msg, msg, sizeof(global_log_ext_msg));
  }

int pbs_getaddrinfo(const char *pNode, struct addrinfo *pHints, struct addrinfo **ppAddrInfoOut)
  {
  return(0);
  }

struct sockaddr_in *get_cached_addrinfo(const char *hostname)
  {
  return(NULL);
  }

struct addrinfo * insert_addr_name_info(struct addrinfo *pAddrInfo, const char *host)
  {
  return(NULL);
  }

int add_execution_slot(struct pbsnode *pnode)
  {
  return(0);
  }

mom_hierarchy_t *initialize_mom_hierarchy()

  {
  mom_hierarchy_t *nt = (mom_hierarchy_t *)calloc(1, sizeof(mom_hierarchy_t));

  nt->current_path  = -1;
  nt->current_level = -1;
  nt->current_node  = -1;

  return(nt);
  }

void parse_mom_hierarchy(int fds)
  {
  mom_levels lv;
  mh->paths.push_back(lv);
  }

id_map::id_map() {}
id_map::~id_map() {}
int id_map::get_new_id(const char *name)
  {
  return 0;
  }

const char *id_map::get_name(int id)
  {
  return 0;
  }

void rel_resc(job *pjob) {}

void mom_hierarchy_handler::initialLoadHierarchy() {}

mom_hierarchy_handler hierarchy_handler; //The global declaration.

int is_svr_attr_set(int i) {return 0;}

std::string get_path_jobdata(const char *a, const char *b) {return "";}

int is_whitespace(

  char c)

  {
  if ((c == ' ')  ||
      (c == '\n') ||
      (c == '\t') ||
      (c == '\r') ||
      (c == '\f'))
    return(TRUE);
  else
    return(FALSE);
  } /* END is_whitespace */



void move_past_whitespace(

  char **str)

  {
  if ((str == NULL) ||
      (*str == NULL))
    return;

  char *current = *str;

  while (is_whitespace(*current) == TRUE)
    current++;

  *str = current;
  } // END move_past_whitespace()

void translate_vector_to_range_string(

  std::string            &range_string,
  const std::vector<int> &indices)

  {
  return;
  }

void translate_range_string_to_vector(

  const char       *range_string,
  std::vector<int> &indices)

  {
  return;
  }

bool job_id_exists(

  const std::string &job_id_string,
  int   *rcode)

  {
  return(true);
  }

/*
 *  * capture_until_close_character()
 *   */

void capture_until_close_character(

  char        **start,
  std::string  &storage,
  char          end)

  {
  if ((start == NULL) ||
      (*start == NULL))
    return;

  char *val = *start;
  char *ptr = strchr(val, end);

  // Make sure we found a close quote and this wasn't an empty string
  if ((ptr != NULL) &&
      (ptr != val))
    {
    storage = val;
    storage.erase(ptr - val);
    *start = ptr + 1; // add 1 to move past the character
    }
  } // capture_until_close_character()

bool task_hosts_match(const char *one, const char *two)
  {
  return(true);
  }
  

#include "../../lib/Libutils/numa_pci_device.cpp"
#include "../../lib/Libutils/numa_socket.cpp"
#include "../../lib/Libutils/numa_chip.cpp"
#include "../../lib/Libutils/numa_core.cpp"
#include "../../lib/Libutils/allocation.cpp"
#include "../../lib/Libattr/req.cpp"
#include "../../lib/Libattr/complete_req.cpp"

#ifdef NVML_API
#ifdef PENABLE_LINUX_CGROUPS
int Machine::initializeNVIDIADevices(hwloc_obj_t machine_obj, hwloc_topology_t topology)
  {
  return(0);
  }
#endif

#endif

#ifdef PENABLE_LINUX_CGROUPS
Machine::Machine() {}
Machine::~Machine() {}

Machine::Machine(const std::string &layout, std::vector<std::string> &valid_ids) {}

int get_machine_total_memory(hwloc_topology_t topology, hwloc_uint64_t *memory)
  {
  return(0);
  }

void save_node_usage(pbsnode *pnode) {}
    
void Machine::populate_job_ids(std::vector<std::string> &job_ids) const
  {
  job_ids.push_back("1.napali");
  job_ids.push_back("2.napali");
  job_ids.push_back("3.napali");
  }
    
void Machine::free_job_allocation(const char *jobid)
  {
  freed_job_allocation++;
  }
#endif

#ifdef MIC
void PCI_Device::initializeMic(int x, hwloc_topology *fred)
  {
  return;
  }

int Chip::initializeMICDevices(hwloc_obj_t chip_obj, hwloc_topology_t topology)
  {
  return(0);
  }
#endif

#ifdef NVIDIA_GPUS
void PCI_Device::initializeGpu(int idx, hwloc_topology_t topology)
  { 
  return;
  }

#endif



int update_user_acls(pbs_attribute *pattr, batch_op op_type)
  {
  return(0);
  }

int update_group_acls(pbs_attribute *pattr, batch_op op_type)
  {
  return(0);
  }

pbs_queue *find_queuebyname(const char *name)
  {
  return(NULL);
  }

pbs_queue *que_alloc(const char *name, int sv_qs_mutex_held)
  {
  pbs_queue *pq;

  pq = (pbs_queue *)calloc(1, sizeof(pbs_queue));
  sprintf(pq->qu_qs.qu_name, "%s", name);
  allocd_queue = pq;
  return(pq);
  }

reservation_holder::reservation_holder() {}

int set_default_gpu_mode_int(const char *default_gpu_str)
  {
  return(0);
  }

int get_svr_attr_str(
    
  int    attr_index,
  char **str)

  {
  return(0);
  }

