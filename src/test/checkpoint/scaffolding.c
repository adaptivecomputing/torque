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

int job_save(job *pjob, int updatetype, int mom_port)
 {
 fprintf(stderr, "The call to job_save needs to be mocked!!\n");
 exit(1);
 }

task *task_find(job *pjob, tm_task_id taskid)
 {
 fprintf(stderr, "The call to task_find needs to be mocked!!\n");
 exit(1);
 }

task *pbs_task_create(job *pjob, tm_task_id taskid)
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

int message_job(job *pjob, enum job_file jft, char *text)
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

int write_nodes_to_file(job *pjob)
 {
 fprintf(stderr, "The call to write_nodes_to_file needs to be mocked!!\n");
 exit(1);
 }

int set_job(job *job, struct startjob_rtn *rtn)
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

int write_gpus_to_file(job *pjob)
 {
 fprintf(stderr, "The call to write_gpus_to_file needs to be mocked!!\n");
 exit(1);
 }

int site_mom_prerst(job *pjob)
 {
 fprintf(stderr, "The call to site_mom_prerst needs to be mocked!!\n");
 exit(1);
 }

int site_mom_postchk(job *pjob, int hold_type)
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

int kill_job(job *pjob, int sig, const char *killer_id_name, const char *why_killed_reason)
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

  char       *exec_hosts,
  char       *username,
  char       *jobid,
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

  job  *pjob,
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

int mom_open_socket_to_jobs_server(job *pjob, const char *caller, void *(*message_handler)(void *))
  {
  return(0);
  }

std::string get_frequency_request(struct cpu_frequency_value *pfreq)
  {
  std::string ret = "";
  return ret;
  }


