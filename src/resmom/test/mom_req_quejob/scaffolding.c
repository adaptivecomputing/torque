#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pwd.h> /* struct password */

#include "attribute.h" /* attribute_def, attribute, svrattrl */
#include "list_link.h" /* tlist_head */
#include "resource.h" /* resource_def, resource */
#include "log.h" /* LOG_BUF_SIZE */
#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */
#include "libpbs.h" /* job_file */



const char *PJobSubState[10];
char *path_jobs;
char *msg_script_write = "Unable to write script file";
int svr_resc_size = 0;
int multi_mom = 0;
time_t time_now;
attribute_def job_attr_def[10];
char *msg_script_open = "Unable to open script file";
tlist_head svr_newjobs;
resource_def *svr_resc_def;
char mom_host[PBS_MAXHOSTNAME + 1];
unsigned int pbs_rm_port = 0;
int internal_state = 0;
tlist_head svr_alljobs;
int LOGLEVEL = 0;
int PBSNodeCheckProlog = 0;
char log_buffer[LOG_BUF_SIZE];



char *std_file_name(job *pjob, enum job_file which, int *keeping)
  {
  fprintf(stderr, "The call to std_file_name needs to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save needs to be mocked!!\n");
  exit(1);
  }

void job_purge(job *pjob)
  {
  fprintf(stderr, "The call to job_purge needs to be mocked!!\n");
  exit(1);
  }

struct passwd *check_pwd(job *pjob)
  {
  fprintf(stderr, "The call to check_pwd needs to be mocked!!\n");
  exit(1);
  }

int reply_send_mom(struct batch_request *request_mom)
  {
  fprintf(stderr, "The call to reply_send_mom needs to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack needs to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, const char *objname, char *text)
  {
  fprintf(stderr, "The call to log_record needs to be mocked!!\n");
  exit(1);
  }

int decode_arst_merge(struct attribute *patr, char *name, char *rescn, char *val)
  {
  fprintf(stderr, "The call to decode_arst_merge needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr needs to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject needs to be mocked!!\n");
  exit(1);
  }

void start_exec(job *pjob)
  {
  fprintf(stderr, "The call to start_exec needs to be mocked!!\n");
  exit(1);
  }

void mom_server_all_update_stat(void)
  {
  fprintf(stderr, "The call to mom_server_all_update_stat needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

void reply_free(struct batch_reply *prep)
  {
  fprintf(stderr, "The call to reply_free needs to be mocked!!\n");
  exit(1);
  }

int open_std_file(job *pjob, enum job_file which, int mode, gid_t exgid)
  {
  fprintf(stderr, "The call to open_std_file needs to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def needs to be mocked!!\n");
  exit(1);
  }

struct passwd * getpwnam_ext(char * user_name)
  {
  fprintf(stderr, "The call to getpwnam_ext needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

void check_state(int Force)
  {
  fprintf(stderr, "The call to check_state needs to be mocked!!\n");
  exit(1);
  }

int reply_jobid(struct batch_request *preq, char *jobid, int which)
  {
  fprintf(stderr, "The call to reply_jobid needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, const char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry needs to be mocked!!\n");
  exit(1);
  }

job *find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

job *job_alloc(void)
  {
  fprintf(stderr, "The call to job_alloc needs to be mocked!!\n");
  exit(1);
  }

void reply_badattr(int code, int aux, svrattrl *pal, struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_badattr needs to be mocked!!\n");
  exit(1);
  }

void reply_text(struct batch_request *preq, int code, char *text)
  {
  fprintf(stderr, "The call to reply_text needs to be mocked!!\n");
  exit(1);
  }

pbs_net_t get_connectaddr(int sock, int mutex)
  {
  fprintf(stderr, "The call to get_connectaddr needs to be mocked!!\n");
  exit(1);
  }
