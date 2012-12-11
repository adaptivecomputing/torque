#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pwd.h> /* struct passwd, gid_t */
#include <sys/socket.h> /* sockaddr */
#include <sys/types.h> /* pid_t */

#include "attribute.h" /* attribute_def, pbs_attribute, svrattrl */
#include "pbs_ifl.h" /* PBS_MAXUSER, MAXPATHLEN */
#include "mom_func.h" /* var_table */
#include "resmon.h" /* TMAX_NSDCOUNT */
#include "log.h" /* LOG_BUF_SIZE */
#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job, eventent, hnodent, tm_event_t, tm_task_id, task */
#include "pbs_ifl.h" /* job_file */
#include "tm_.h" /* tm_event_t */
#include "list_link.h" /* list_link, tlist_head */

char *apbasil_protocol;
char *apbasil_path;
struct cphosts *pcphosts = NULL;
char *path_undeliv;
char *msg_err_unlink = "Unlink of %s file %s failed";
time_t time_now;
struct sig_tbl sig_tbl[5];
attribute_def job_attr_def[10];
int exiting_tasks = 0;
char pbs_current_user[PBS_MAXUSER];
char rcp_path[MAXPATHLEN];
char rcp_args[MAXPATHLEN];
char *msg_jobmod = "Job Modified";
int cphosts_num = 0;
struct var_table vtable; 
char mom_host[PBS_MAXHOSTNAME + 1];
int spoolasfinalname = 0;
char *path_spool;
unsigned int pbs_rm_port = 0;
unsigned int alarm_time = 10;
char *TNoSpoolDirList[TMAX_NSDCOUNT];
tlist_head svr_alljobs;
int LOGLEVEL = 0;
char *msg_manager = "%s at request of %s@%s";
int multi_mom = 1;
char MOMUNameMissing[64];


struct batch_request *alloc_br(int type)
  {
  fprintf(stderr, "The call to alloc_br needs to be mocked!!\n");
  exit(1);
  }

char *arst_string(char *str, pbs_attribute *pattr)
  {
  fprintf(stderr, "The call to arst_string needs to be mocked!!\n");
  exit(1);
  }

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

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

pid_t fork_me(int conn)
  {
  fprintf(stderr, "The call to fork_me needs to be mocked!!\n");
  exit(1);
  }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr needs to be mocked!!\n");
  exit(1);
  }

int reply_send_mom(struct batch_request *request)
  {
  fprintf(stderr, "The call to reply_send_mom needs to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack needs to be mocked!!\n");
  exit(1);
  }

int replace_checkpoint_path(char *path)
  {
  fprintf(stderr, "The call to replace_checkpoint_path needs to be mocked!!\n");
  exit(1);
  }

int DIS_reply_read(struct tcp_chan *chan, struct batch_reply *preply)
  {
  fprintf(stderr, "The call to DIS_reply_read needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_JobFile(struct tcp_chan *chan, int seq, char *buf, int len, char *jobid, int which)
  {
  fprintf(stderr, "The call to encode_DIS_JobFile needs to be mocked!!\n");
  exit(1);
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br needs to be mocked!!\n");
  exit(1);
  }

eventent *event_alloc(int command, hnodent *pnode, tm_event_t event, tm_task_id taskid)
  {
  fprintf(stderr, "The call to event_alloc needs to be mocked!!\n");
  exit(1);
  }

int InitUserEnv(job *pjob, task *ptask, char **envp, struct passwd *pwdp, char *shell)
  {
  fprintf(stderr, "The call to InitUserEnv needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int start_checkpoint(job *pjob, int abort, struct batch_request *preq)
  {
  fprintf(stderr, "The call to start_checkpoint needs to be mocked!!\n");
  exit(1);
  }

int in_remote_checkpoint_dir(char *ckpt_path)
  {
  fprintf(stderr, "The call to in_remote_checkpoint_dir needs to be mocked!!\n");
  exit(1);
  }

int mom_open_socket_to_jobs_server(job *pjob, const char *caller_id, void *(*message_handler)(void *))
  {
  fprintf(stderr, "The call to mom_open_socket_to_jobs_server needs to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject needs to be mocked!!\n");
  exit(1);
  }

int kill_task(struct task *task, int sig, int pg)
  {
  fprintf(stderr, "The call to kill_task needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

int attr_atomic_set(struct svrattrl *plist, pbs_attribute *old, pbs_attribute *new_attr, attribute_def *pdef, int limit, int unkn, int privil, int *badattr)
  {
  fprintf(stderr, "The call to attr_atomic_set needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

int diswui(struct tcp_chan *chan, unsigned value)
  {
  fprintf(stderr, "The call to diswui needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_ReqHdr(struct tcp_chan *chan, int reqt, char *user)
  {
  fprintf(stderr, "The call to encode_DIS_ReqHdr needs to be mocked!!\n");
  exit(1);
  }

int mom_set_limits(job *pjob, int set_mode)
  {
  fprintf(stderr, "The call to mom_set_limits needs to be mocked!!\n");
  exit(1);
  }

int open_std_file(job *pjob, enum job_file which, int mode, gid_t exgid)
  {
  fprintf(stderr, "The call to open_std_file needs to be mocked!!\n");
  exit(1);
  }

int mkdirtree(char *dirpath, mode_t mode)
  {
  fprintf(stderr, "The call to mkdirtree needs to be mocked!!\n");
  exit(1);
  }

struct passwd * getpwnam_ext(char * user_name)
  {
  fprintf(stderr, "The call to getpwnam_ext needs to be mocked!!\n");
  exit(1);
  }

int remtree(char *dirname)
  {
  fprintf(stderr, "The call to remtree needs to be mocked!!\n");
  exit(1);
  }

int init_groups(char *pwname, int pwgrp, int groupsize, int *groups)
  {
  fprintf(stderr, "The call to init_groups needs to be mocked!!\n");
  exit(1);
  }

int tcp_connect_sockaddr(struct sockaddr *sa, size_t sa_size)
  {
  fprintf(stderr, "The call to tcp_connect_sockaddr needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

int TTmpDirName(job *pjob, char *tmpdir, int tmpdir_size)
  {
  fprintf(stderr, "The call to TTmpDirName needs to be mocked!!\n");
  exit(1);
  }

int is_joined(job *pjob)
  {
  fprintf(stderr, "The call to is_joined needs to be mocked!!\n");
  exit(1);
  }

int job_unlink_file(job *pjob, const char *name)
  {
  fprintf(stderr, "The call to job_unlink_file needs to be mocked!!\n");
  exit(1);
  }

int MUSleep(long SleepDuration)
  {
  fprintf(stderr, "The call to MUSleep needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }

void mom_deljob(job *pjob)
  {
  fprintf(stderr, "The call to mom_deljob needs to be mocked!!\n");
  exit(1);
  }

job *mom_find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

int im_compose(int stream, char *jobid, char *cookie, int command, tm_event_t event, tm_task_id taskid)
  {
  fprintf(stderr, "The call to im_compose needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_ReqExtend(struct tcp_chan *chan, char *extend)
  {
  fprintf(stderr, "The call to encode_DIS_ReqExtend needs to be mocked!!\n");
  exit(1);
  }

void list_move(tlist_head *from, tlist_head *to)
  {
  fprintf(stderr, "The call to list_move needs to be mocked!!\n");
  exit(1);
  }

int diswsi(int stream, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }

job *job_alloc(void)
  {
  fprintf(stderr, "The call to job_alloc needs to be mocked!!\n");
  exit(1);
  }

int kill_job(job *pjob, int sig, const char *killer_id_name, char *why_killed_reason)
  {
  fprintf(stderr, "The call to kill_job needs to be mocked!!\n");
  exit(1);
  }

void reply_text(struct batch_request *preq, int code, char *text)
  {
  fprintf(stderr, "The call to reply_text needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

int TMOMJobGetStartInfo(

  job         *pjob, /* I */
  pjobexec_t **TJEP) /* O */

  {
  return(0);
  }

int destroy_alps_reservation(

  char *reservation_id,
  char *apbasil_path,
  char *apbasil_protocol)

  {
  return(0);
  }
