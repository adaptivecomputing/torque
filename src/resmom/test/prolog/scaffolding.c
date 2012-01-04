#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "resource.h" /* resource_def */
#include "log.h" /* LOG_BUF_SIZE */
#include "attribute.h" /* attribute */
#include "list_link.h" /* tlist_head, list_link */
#include "pbs_job.h" /* job */
#include "libpbs.h" /* job_file */

int lockfds = -1;
int svr_resc_size = 0;
int DEBUGMODE = 0;
char *path_aux;
int reduceprologchecks;
resource_def *svr_resc_def;
char PBSNodeMsgBuf[1024];
int MOMPrologFailureCount;
int LOGLEVEL = 0;
char log_buffer[LOG_BUF_SIZE];
int MOMPrologTimeoutCount;
uid_t pbsuser;
gid_t pbsgroup;

int encode_resc(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int ac_perm)
  {
  fprintf(stderr, "The call to encode_resc needs to be mocked!!\n");
  exit(1);
  }

char *get_job_envvar(job *pjob, char *variable)
  {
  fprintf(stderr, "The call to get_job_envvar needs to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, const char *objname, char *text)
  {
  fprintf(stderr, "The call to log_record needs to be mocked!!\n");
  exit(1);
  }

void encode_used(job *pjob, tlist_head *phead)
  {
  fprintf(stderr, "The call to encode_used needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int pe_input(char *jobid)
  {
  fprintf(stderr, "The call to pe_input needs to be mocked!!\n");
  exit(1);
  }

void net_close(int but)
  {
  fprintf(stderr, "The call to net_close needs to be mocked!!\n");
  exit(1);
  }

void log_ext(int errnum, const char *routine, char *text, int severity)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
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

void free_attrlist(tlist_head *pattrlisthead)
  {
  fprintf(stderr, "The call to free_attrlist needs to be mocked!!\n");
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

int TTmpDirName(job *pjob, char *tmpdir)
  {
  fprintf(stderr, "The call to TTmpDirName needs to be mocked!!\n");
  exit(1);
  }

int is_joined(job *pjob)
  {
  fprintf(stderr, "The call to is_joined needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, const char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry needs to be mocked!!\n");
  exit(1);
  }
