#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attribute_def, attribute, svrattrl */
#include "pbs_job.h" /* all_jobs, job */
#include "resource.h" /* resource_def */
#include "pbs_ifl.h" /* PBS_MAXSERVERNAME */
#include "server.h" /* server */
#include "log.h" /* LOG_BUF_SIZE */
#include "queue.h" /* pbs_queue */
#include "batch_request.h" /* batch_request */
#include "work_task.h" /* work_task, work_type */

const char *PJobSubState[10];
char *path_jobs;
char *msg_script_write = "Unable to write script file";
int svr_resc_size = 0;
attribute_def job_attr_def[10];
char *msg_script_open = "Unable to open script file";
char *msg_jobnew = "Job Queued at request of %s@%s, owner = %s, job name = %s, queue = %s";
struct all_jobs newjobs;
char server_name[PBS_MAXSERVERNAME + 1];
char *pbs_o_host = "PBS_O_HOST";
resource_def *svr_resc_def;
int queue_rank = 0;
char *path_spool;
struct server server;
int LOGLEVEL = 0;
char log_buffer[LOG_BUF_SIZE];
char *msg_daemonname = "unset";


int setup_array_struct(job *pjob)
  {
  fprintf(stderr, "The call to setup_array_struct to be mocked!!\n");
  exit(1);
  }

pbs_queue *find_queuebyname(char *quename)
  {
  fprintf(stderr, "The call to find_queuebyname to be mocked!!\n");
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

void set_chkpt_deflt(job *pjob, pbs_queue *pque)
  {
  fprintf(stderr, "The call to set_chkpt_deflt to be mocked!!\n");
  exit(1);
  }

void clear_attr(attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_jobs_queue(job *pjob)
  {
  fprintf(stderr, "The call to get_jobs_queue to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

int svr_authorize_jobreq(struct batch_request *preq, job *pjob)
  {
  fprintf(stderr, "The call to svr_authorize_jobreq to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_record to be mocked!!\n");
  exit(1);
  }

char *get_variable(job *pjob, char *variable)
  {
  fprintf(stderr, "The call to get_variable to be mocked!!\n");
  exit(1);
  }

void job_clone_wt(struct work_task *ptask)
  {
  fprintf(stderr, "The call to job_clone_wt to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task to be mocked!!\n");
  exit(1);
  }

char * csv_find_value(char *csv_str, char *search_str)
  {
  fprintf(stderr, "The call to csv_find_value to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket to be mocked!!\n");
  exit(1);
  }

int insert_job(struct all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to insert_job to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr to be mocked!!\n");
  exit(1);
  }

int decode_resc(struct attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_resc to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

job *next_job(struct all_jobs *aj, int *iter)
  {
  fprintf(stderr, "The call to next_job to be mocked!!\n");
  exit(1);
  }

char *prefix_std_file(job *pjob, int key)
  {
  fprintf(stderr, "The call to prefix_std_file to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

int job_route(job *jobp)
  {
  fprintf(stderr, "The call to job_route to be mocked!!\n");
  exit(1);
  }

int svr_enquejob(job *pjob, int has_sv_qs_mutex)
  {
  fprintf(stderr, "The call to svr_enquejob to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

int svr_chkque(job *pjob, pbs_queue *pque, char *hostname, int mtype, char *EMsg)
  {
  fprintf(stderr, "The call to svr_chkque to be mocked!!\n");
  exit(1);
  }

int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg)
  {
  fprintf(stderr, "The call to get_fullhostname to be mocked!!\n");
  exit(1);
  }

int remove_job(struct all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to remove_job to be mocked!!\n");
  exit(1);
  }

int reply_jobid(struct batch_request *preq, char *jobid, int which)
  {
  fprintf(stderr, "The call to reply_jobid to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event to be mocked!!\n");
  exit(1);
  }

void issue_track(job *pjob)
  {
  fprintf(stderr, "The call to issue_track to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry to be mocked!!\n");
  exit(1);
  }

job *find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job to be mocked!!\n");
  exit(1);
  }

int svr_save(struct server *ps, int mode)
  {
  fprintf(stderr, "The call to svr_save to be mocked!!\n");
  exit(1);
  }

void replace_attr_string(struct attribute *attr, char *newval)
  {
  fprintf(stderr, "The call to replace_attr_string to be mocked!!\n");
  exit(1);
  }

job *job_alloc(void)
  {
  fprintf(stderr, "The call to job_alloc to be mocked!!\n");
  exit(1);
  }

char *add_std_filename(job *pjob, char * path, int key)
  {
  fprintf(stderr, "The call to add_std_filename to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, char *method_name, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job *pjob, int *newstate, int *newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_dfltque(void)
  {
  fprintf(stderr, "The call to get_dfltque to be mocked!!\n");
  exit(1);
  }

void reply_badattr(int code, int aux, svrattrl *pal, struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_badattr to be mocked!!\n");
  exit(1);
  }

char *pbs_default(void)
  {
  fprintf(stderr, "The call to pbs_default to be mocked!!\n");
  exit(1);
  }

pbs_net_t get_connectaddr(int sock, int mutex)
  {
  fprintf(stderr, "The call to get_connectaddr to be mocked!!\n");
  exit(1);
  }
