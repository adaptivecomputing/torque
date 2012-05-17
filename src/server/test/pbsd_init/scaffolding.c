#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h> /* pthread_mutex_t */
#include <limits.h> /* _POSIX_PATH_MAX */

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

int scheduler_sock=0;
int scheduler_jobct = 0;
pthread_mutex_t *job_log_mutex;
pthread_mutex_t *log_mutex;
char *msg_init_chdir = "unable to change to directory %s";
char *path_jobs;
char *msg_err_noqueue = "Unable to requeue job, queue is not defined";
all_nodes allnodes;
char *path_nodestate;
char *path_priv = NULL;
char *msg_init_exptjobs = "Expected %d, recovered %d jobs";
char *msg_daemonname = "unset";
char *path_track;
char *path_checkpoint;
char *job_log_file = NULL;
char *plogenv = NULL;
pthread_mutex_t *node_state_mutex = NULL;
pthread_mutex_t *acctfile_mutex = NULL;
pthread_mutex_t *check_tasks_mutex = NULL;
char *msg_init_queued = "Requeued in queue: ";
char *path_svrlog;
char path_acct[_POSIX_PATH_MAX];
char *path_nodes;
char *msg_init_recovque = "Recovered queue %s";
attribute_def job_attr_def[10];
char *msg_init_expctq = "Expected %d, recovered %d queues";
char *path_arrays;
char *log_file = NULL;
char *msg_script_open = "Unable to open script file";
struct all_jobs newjobs;
char server_name[PBS_MAXSERVERNAME + 1];
char *path_nodenote;
struct all_jobs alljobs;
char *path_queues;
char path_log[MAXPATHLEN + 1];
char *msg_init_nojobs = "No jobs to open";
tlist_head svr_newnodes;
char *msg_err_malloc = "malloc failed";
int rpp_dbprt = 0;
char *path_credentials;
char *msg_init_badjob = "Recover of job %s failed";
char *path_nodes_new;
resource_def *svr_resc_def;
tlist_head svr_requests;
attribute_def que_attr_def[10];
int queue_rank = 0;
char *path_spool;
char *msg_init_abt = "Job aborted on PBS Server initialization";
char *msg_init_noqueues = "No queues to open";
char *path_nodenote_new;
char *path_svrdb = NULL;
threadpool_t *request_pool;
char *msg_init_baddb = "Unable to read server database";
struct server server;
char *msg_init_substate = "Requeueing job, substate: %d ";
char *msg_init_unkstate = "Unable to recover job in strange substate: %d";
struct all_jobs array_summary;
attribute_def svr_attr_def[10];
int a_opt_init = -1;
all_tasks task_list_timed;
char *path_jobinfo_log;
int LOGLEVEL = 0;
pthread_mutex_t *svr_requests_mutex = NULL;
all_tasks task_list_event;
char *path_svrdb_new;
char *path_home = PBS_SERVER_HOME;
char *acct_file = NULL;
char *path_mom_hierarchy = "/dev/null";
pthread_mutex_t *svr_do_schedule_mutex;
pthread_mutex_t *listener_command_mutex;
pthread_mutex_t *acctfile_mutex;
pthread_mutex_t *check_tasks_mutex;
pthread_mutex_t *retry_routing_mutex;



void on_job_rerun(struct work_task *ptask)
  {
  fprintf(stderr, "The call to on_job_rerun needs to be mocked!!\n");
  exit(1);
  }

job_array *get_jobs_array(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_array needs to be mocked!!\n");
  exit(1);
  }

void set_old_nodes(job *pjob)
  {
  fprintf(stderr, "The call to set_old_nodes needs to be mocked!!\n");
  exit(1);
  }

char *parse_servername(char *name, unsigned int *service)
  {
  fprintf(stderr, "The call to parse_servername needs to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save needs to be mocked!!\n");
  exit(1);
  }

int job_purge(job *pjob)
  {
  fprintf(stderr, "The call to job_purge needs to be mocked!!\n");
  exit(1);
  }

int net_move(job *jobp, struct batch_request *req)
  {
  fprintf(stderr, "The call to net_move needs to be mocked!!\n");
  exit(1);
  }

void svr_mailowner(job *pjob, int mailpoint, int force, char *text)
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
  fprintf(stderr, "The call to set_statechar needs to be mocked!!\n");
  exit(1);
  }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr needs to be mocked!!\n");
  exit(1);
  }

pbs_net_t get_hostaddr(int *local_errno, char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr needs to be mocked!!\n");
  exit(1);
  }

void on_job_exit(struct work_task *ptask)
  {
  fprintf(stderr, "The call to on_job_exit needs to be mocked!!\n");
  exit(1);
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

void job_clone_wt(struct work_task *ptask)
  {
  fprintf(stderr, "The call to job_clone_wt needs to be mocked!!\n");
  exit(1);
  }

void start_request_pool()
  {
  fprintf(stderr, "The call to start_request_pool needs to be mocked!!\n");
  exit(1);
  }

char *pbs_get_server_list(void)
  {
  fprintf(stderr, "The call to pbs_get_server_list needs to be mocked!!\n");
  exit(1);
  }

int csv_length(char *csv_str)
  {
  fprintf(stderr, "The call to csv_length needs to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task needs to be mocked!!\n");
  exit(1);
  }

char *csv_nth(char *csv_str, int n)
  {
  fprintf(stderr, "The call to csv_nth needs to be mocked!!\n");
  exit(1);
  }

void initialize_all_nodes_array(all_nodes *an)
  {
  fprintf(stderr, "The call to initialize_all_nodes_array needs to be mocked!!\n");
  exit(1);
  }

job_array *next_array(int *iter)
  {
  fprintf(stderr, "The call to next_array needs to be mocked!!\n");
  exit(1);
  }

void initialize_all_tasks_array(all_tasks *at)
  {
  fprintf(stderr, "The call to initialize_all_tasks_array needs to be mocked!!\n");
  exit(1);
  }

int insert_job(struct all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to insert_job needs to be mocked!!\n");
  exit(1);
  }

int unlock_node(struct pbsnode *the_node, char *id, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_node needs to be mocked!!\n");
  exit(1);
  }

void depend_clrrdy(job *pjob)
  {
  fprintf(stderr, "The call to depend_clrrdy needs to be mocked!!\n");
  exit(1);
  }

struct pbsnode *next_host(all_nodes *an, int *iter, struct pbsnode *held)
  {
  fprintf(stderr, "The call to next_host needs to be mocked!!\n");
  exit(1);
  }

job *next_job(struct all_jobs *aj, int *iter)
  {
  fprintf(stderr, "The call to next_job needs to be mocked!!\n");
  exit(1);
  }

int chk_file_sec(char *path, int isdir, int sticky, int disallow, int fullpath, char *SEMsg)
  {
  fprintf(stderr, "The call to chk_file_sec needs to be mocked!!\n");
  exit(1);
  }

void log_close(int msg)
  {
  fprintf(stderr, "The call to log_close needs to be mocked!!\n");
  exit(1);
  }

int set_arst(struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_arst needs to be mocked!!\n");
  exit(1);
  }

int svr_recov_xml(char *svrfile,  int read_only)
  {
  fprintf(stderr, "The call to svr_recov_xml needs to be mocked!!\n");
  exit(1);
  }

void initialize_all_jobs_array(struct all_jobs *aj)
  {
  fprintf(stderr, "The call to initialize_all_jobs_array needs to be mocked!!\n");
  exit(1);
  }

int job_abt(job **pjobp, char *text)
  {
  fprintf(stderr, "The call to job_abt needs to be mocked!!\n");
  exit(1);
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

int svr_enquejob(job *pjob, int has_sv_qs_mutex, int prev_index)
  {
  fprintf(stderr, "The call to svr_enquejob needs to be mocked!!\n");
  exit(1);
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

void initialize_allques_array(all_queues *aq)
  {
  fprintf(stderr, "The call to initialize_allques_array needs to be mocked!!\n");
  exit(1);
  }

job *job_recov(char *filename)
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

int decode_arst_direct(struct pbs_attribute *patr,  char *val)
  {
  fprintf(stderr, "The call to decode_arst_direct needs to be mocked!!\n");
  exit(1);
  }

int setup_env(char *filen)
  {
  fprintf(stderr, "The call to setup_env needs to be mocked!!\n");
  exit(1);
  }

int array_recov(char *path, job_array **pa)
  {
  fprintf(stderr, "The call to array_recov needs to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate needs to be mocked!!\n");
  exit(1);
  }

void track_save(struct work_task *pwt)
  {
  fprintf(stderr, "The call to track_save needs to be mocked!!\n");
  exit(1);
  }

void acct_close(void)
  {
  fprintf(stderr, "The call to acct_close needs to be mocked!!\n");
  exit(1);
  }

job *find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

int svr_save(struct server *ps, int mode)
  {
  fprintf(stderr, "The call to svr_save needs to be mocked!!\n");
  exit(1);
  }

int acct_open(char *filename)
  {
  fprintf(stderr, "The call to acct_open needs to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job *pjob, int *newstate, int *newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate needs to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue needs to be mocked!!\n");
  exit(1);
  }


char *threadsafe_tokenizer(char **str, char *delims)
  {
  fprintf(stderr, "The call to threadsafe_tokenizer needs to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int append_dynamic_string (dynamic_string *ds, const char *str)
  {
  return(0);
  }

int get_parent_and_child(char *start, char **parent, char **child, char **end)
  {
  return(0);
  }

int add_hello(hello_container *hc, char *node_name)
  {
  return(0);
  }

int create_partial_pbs_node(char *nodename, unsigned long addr, int perms)
  {
  return(0);
  }

dynamic_string *get_dynamic_string(int initial_size, const char *str)
  {
  return(NULL);
  }

struct pbsnode *find_nodebyname(char *name)
  {
  return(NULL);
  }

void initialize_hello_container(hello_container *hc)
  {
  }

void free_dynamic_string(dynamic_string *ds)
  {
  }

int delete_last_word_from_dynamic_string(dynamic_string *ds)
  {
  return(0);
  }

void initialize_queue_recycler() {}

int set_svr_attr(int attr_index, void *val) 
  {
  return(0);
  }

int copy_to_end_of_dynamic_string(dynamic_string *ds, const char *str)
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

int add_hello_after(hello_container *hc, char *node_name, int index)
  {
  fprintf(stderr, "The call to add_hello_after needs to be mocked!!\n");
  exit(1);
  }

int enqueue_threadpool_request(

  void *(*func)(void *),
  void *arg)

  {
  return(0);
  }

void initialize_login_holder() {}

void initialize_alps_reservations() {}
