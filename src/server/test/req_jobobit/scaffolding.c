#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <time.h> /* timeval */
#include <pthread.h> /* pthread_mutex_t */

#include "attribute.h" /* attribute_def, svrattrl, pbs_attribute */
#include "net_connect.h" /* pbs_net_t, conn_type */
#include "server_limits.h" /* RECOV_WARM */
#include "work_task.h" /* work _task, work_type, all_tasks */
#include "server.h" /* server */
#include "sched_cmds.h" /* SCH_SCHEDULE_NULL */
#include "batch_request.h" /* batch_request */
#include "array.h" /* job_array */
#include "pbs_job.h" /* job */
#include "node_func.h" /* node_info */
#include "list_link.h" /* list_link */
#include "pbs_ifl.h" /* batch_op */
#include "array.h" /* ArrayEventsEnum */
#include "pbs_nodes.h" /* pbsnode */
#include "queue.h" /* pbs_queue */

char *msg_momnoexec2 = "Job cannot be executed\nSee job standard error file";
char *msg_job_end_sig = "Terminated on signal %d";
  char *msg_obitnojob  = "Job Obit notice received from %s has error %d";
attribute_def job_attr_def[10];
char *msg_obitnocpy = "Post job file processing error; job %s on host %s";
int   server_init_type = RECOV_WARM;
char *path_spool;
char *msg_init_abt = "Job aborted on PBS Server initialization";
pbs_net_t pbs_server_addr;
const char *PJobState[] = {"hi", "hello"};
struct server server;
int  svr_do_schedule = SCH_SCHEDULE_NULL;
int listener_command = SCH_SCHEDULE_NULL;
int LOGLEVEL = 0;
char *msg_obitnodel = "Unable to delete files for job %s, on host %s";
char *msg_momnoexec1 = "Job cannot be executed\nSee Administrator for help";
char *msg_job_end_stat = "Exit_status=%d";
char *msg_momjoboverlimit = "Job exceeded some resource limit (walltime, mem, etc.). Job was aborted\nSee Administrator for help";
pthread_mutex_t *svr_do_schedule_mutex;
pthread_mutex_t *listener_command_mutex;




struct batch_request *alloc_br(int type)
  {
  fprintf(stderr, "The call to alloc_br to be mocked!!\n");
  exit(1);
  }

job_array *get_jobs_array(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_array to be mocked!!\n");
  exit(1);
  }

char *parse_servername(char *name, unsigned int *service)
  {
  fprintf(stderr, "The call to parse_servername to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save to be mocked!!\n");
  exit(1);
  }

void job_purge(job *pjob)
  {
  fprintf(stderr, "The call to job_purge to be mocked!!\n");
  exit(1);
  }

void svr_mailowner(job *pjob, int mailpoint, int force, char *text)
  {
  fprintf(stderr, "The call to svr_mailowner to be mocked!!\n");
  exit(1);
  }

int modify_job_attr(job *pjob, svrattrl *plist, int perm, int *bad)
  {
  fprintf(stderr, "The call to modify_job_attr to be mocked!!\n");
  exit(1);
  }

pbs_net_t get_hostaddr(int *local_errno, char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr to be mocked!!\n");
  exit(1);
  }

long attr_ifelse_long(pbs_attribute *attr1, pbs_attribute *attr2, long deflong)
  {
  fprintf(stderr, "The call to attr_ifelse_long to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_jobs_queue(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_queue to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

void cleanup_restart_file( job *pjob)  
  {
  fprintf(stderr, "The call to cleanup_restart_file to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req to be mocked!!\n");
  exit(1);
  }

void *process_Dreply(void *new_sock)
  {
  fprintf(stderr, "The call to process_Dreply to be mocked!!\n");
  exit(1);
  }

void free_nodes(node_info **ninfo_arr)
  {
  fprintf(stderr, "The call to free_nodes to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, const char *objname, char *text)
  {
  fprintf(stderr, "The call to log_record to be mocked!!\n");
  exit(1);
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br to be mocked!!\n");
  exit(1);
  }

void remove_job_delete_nanny( struct job *pjob)
  {
  fprintf(stderr, "The call to remove_job_delete_nanny to be mocked!!\n");
  exit(1);
  }

void account_jobend( job *pjob, char *used) 
  {
  fprintf(stderr, "The call to account_jobend to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task to be mocked!!\n");
  exit(1);
  }

void svr_disconnect(int handle)
  {
  fprintf(stderr, "The call to svr_disconnect to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

int depend_on_term(job *pjob)
  {
  fprintf(stderr, "The call to depend_on_term to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

int issue_Drequest(int conn, struct batch_request *request, void (*func)(struct work_task *), struct work_task **ppwt)
  {
  fprintf(stderr, "The call to issue_Drequest to be mocked!!\n");
  exit(1);
  }

void set_resc_assigned(job *pjob, enum batch_op op)
  {
  fprintf(stderr, "The call to set_resc_assigned to be mocked!!\n");
  exit(1);
  }

void update_array_values(job_array *pa, void *j, int old_state, enum ArrayEventsEnum event)
  {
  fprintf(stderr, "The call to update_array_values to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new, void *pobj)
  {
  fprintf(stderr, "The call to append_link to be mocked!!\n");
  exit(1);
  }

int svr_connect(pbs_net_t hostaddr, unsigned int port, struct pbsnode *pnode, void *(*func)(void *), enum conn_type cntype)
  {
  fprintf(stderr, "The call to svr_connect to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, const char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event to be mocked!!\n");
  exit(1);
  }

void issue_track(job *pjob)
  {
  fprintf(stderr, "The call to issue_track to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, const char *routine, char *text) 
  {
  fprintf(stderr, "The call to log_err to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate to be mocked!!\n");
  exit(1);
  }

job *find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job to be mocked!!\n");
  exit(1);
  }

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
  {
  fprintf(stderr, "The call to timeval_subtract to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job *pjob, int *newstate, int *newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate to be mocked!!\n");
  exit(1);
  }

int insert_task(all_tasks *at, work_task *wt) 
  {
  fprintf(stderr, "The call to insert_task to be mocked!!\n");
  exit(1);
  }

void get_jobowner(char *from, char *to)
  {
  fprintf(stderr, "The call to get_jobowner to be mocked!!\n");
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

int safe_strncat(char *str, char *to_append, size_t space_remaining)
  {
  return(0);
  }

void close_conn(int sd, int mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

