#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pwd.h> /* struct password */

#include "pbs_config.h"
#include "attribute.h" /* attribute_def, pbs_attribute, svrattrl */
#include "list_link.h" /* tlist_head */
#include "resource.h" /* resource_def, resource */
#include "log.h" /* LOG_BUF_SIZE */
#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */
#include "libpbs.h" /* job_file */
#include "mom_func.h"

std::list<job *> alljobs_list;
const char *PJobSubState[10];
char *path_jobs;
const char *msg_script_write = "Unable to write script file";
int svr_resc_size = 0;
int multi_mom = 0;
time_t time_now;
attribute_def job_attr_def[10];
const char *msg_script_open = "Unable to open script file";
tlist_head svr_newjobs;
resource_def *svr_resc_def;
char mom_host[PBS_MAXHOSTNAME + 1];
unsigned int pbs_rm_port = 0;
int internal_state = 0;
tlist_head svr_alljobs;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
int PBSNodeCheckProlog = 0;
char log_buffer[LOG_BUF_SIZE];
int reject_job_submit = 0;
int use_nvidia_gpu = TRUE;

// sensing variables
char prefix[PBS_JOBBASE+1];


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

void mom_job_purge(job *pjob)
  {
  fprintf(stderr, "The call to job_purge needs to be mocked!!\n");
  exit(1);
  }

bool check_pwd(job *pjob)
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

void log_record(int eventtype, int objclass, const char *objname, const char *text)
  {
  fprintf(stderr, "The call to log_record needs to be mocked!!\n");
  exit(1);
  }

int decode_arst_merge(struct pbs_attribute *patr, const char *name, const char *rescn, const char *val)
  {
  fprintf(stderr, "The call to decode_arst_merge needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr needs to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject needs to be mocked!!\n");
  exit(1);
  }

int start_exec(job *pjob)
  {
  return(0);
  }

void mom_server_all_update_stat(void)
  {
  fprintf(stderr, "The call to mom_server_all_update_stat needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  return(NULL);
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

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def needs to be mocked!!\n");
  exit(1);
  }

struct passwd * getpwnam_ext(char **user_buf, char * user_name)
  {
  fprintf(stderr, "The call to getpwnam_ext needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  job *pjob = (job *)pobj;
  strcpy(prefix, pjob->ji_qs.ji_fileprefix);
  }

void check_state(int Force)
  {
  fprintf(stderr, "The call to check_state needs to be mocked!!\n");
  exit(1);
  }

int reply_jobid(struct batch_request *preq, char *jobid, int which)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry needs to be mocked!!\n");
  exit(1);
  }

job *mom_find_job(const char *jobid)
  {
  return(NULL);
  }

job *mom_job_alloc(void)
  {
  return((job *)calloc(1, sizeof(job)));
  }

void reply_badattr(int code, int aux, svrattrl *pal, struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_badattr needs to be mocked!!\n");
  exit(1);
  }

void reply_text(struct batch_request *preq, int code, const char *text)
  {
  fprintf(stderr, "The call to reply_text needs to be mocked!!\n");
  exit(1);
  }

pbs_net_t get_connectaddr(int sock, int mutex)
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

void remove_from_job_list(job *pjob) {}

void free_pwnam(struct passwd *pwdp, char *buf)
  {}

void send_update_soon()
  {}
