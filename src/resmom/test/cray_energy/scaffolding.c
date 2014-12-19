#include "resource.h" /* resource_def */
#include "pbs_job.h" /* job_file_delete_info */

resource_def *svr_resc_def;
int           svr_resc_size = 0;


job * alloc_job()
  {
  return (job *)calloc(1,sizeof(job));
  }

resource_def *find_resc_def(resource_def *svr_resc, char const * name, int max_index)
  {
  return(NULL);
  }



#if 0

#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h> /* gid_t, uid_t */

#include "attribute.h" /* attribute_def, pbs_attribute */
#include "list_link.h" /* tlist_head, list_link */
#include "log.h" /* LOG_BUF_SIZE */
#include "pbs_ifl.h" /* MAXPATHLEN */
#include "net_connect.h" /* pbs_net_t */
#include "resource.h" /* resource_def */
#include "server_limits.h" /* pbs_net_t. Also defined in net_connect.h */
#include "pbs_job.h" /* job_file_delete_info */
#include "threadpool.h"

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

int enqueue_threadpool_request(void *(*func)(void *),void *arg, threadpool_t *tp)
  {
  fprintf(stderr, "The call to enqueue_threadpool_request needs to be mocked!!\n");
  exit(1);
  }

void log_ext(int errnum, const char *routine, const char *text, int severity)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void MOMCheckRestart(void)
  {
  fprintf(stderr, "The call to MOMCheckRestart needs to be mocked!!\n");
  exit(1);
  }

int client_to_svr(pbs_net_t hostaddr, unsigned int port, int local_port, char *EMsg)
  {
  fprintf(stderr, "The call to client_to_svr needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

void nodes_free(job *pj)
  {
  fprintf(stderr, "The call to nodes_free needs to be mocked!!\n");
  exit(1);
  }

void mom_checkpoint_delete_files(job_file_delete_info *jfdi)
  {
  fprintf(stderr, "The call to mom_checkpoint_delete_files needs to be mocked!!\n");
  exit(1);
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

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

int destroy_alps_reservation(

  char *reservation_id,
  char *apbasil_path,
  char *apbasil_protocol,
  int   retries)

  {
  return(0);
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

#endif
