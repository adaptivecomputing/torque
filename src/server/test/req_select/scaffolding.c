#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attribute_def, attribute, svrattrl */
#include "pbs_job.h" /* all_jobs */
#include "resource.h" /* resource_def */
#include "server.h" /* server */
#include "queue.h" /* pbs_queue */
#include "list_link.h" /* list_link */
#include "batch_request.h" /* batch_request */

#define STAT_CNTL
#include "svrfunc.h" /* stat_cntl */

int svr_resc_size = 0;
attribute_def job_attr_def[10];
struct all_jobs alljobs;
resource_def *svr_resc_def;
int LOGLEVEL = 0;
struct server server;
struct all_jobs array_summary;


pbs_queue *find_queuebyname(char *quename)
  {
  fprintf(stderr, "The call to find_queuebyname to be mocked!!\n");
  exit(1);
  }

void clear_attr(attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr to be mocked!!\n");
  exit(1);
  }

int reply_send(struct batch_request *request)
  {
  fprintf(stderr, "The call to reply_send to be mocked!!\n");
  exit(1);
  }

int encode_str(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_str to be mocked!!\n");
  exit(1);
  }

int svr_authorize_jobreq(struct batch_request *preq, job *pjob)
  {
  fprintf(stderr, "The call to svr_authorize_jobreq to be mocked!!\n");
  exit(1);
  }

int decode_str(attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_str to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr to be mocked!!\n");
  exit(1);
  }

int set_str(struct attribute *attr, struct attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_str to be mocked!!\n");
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

void free_str(struct attribute *attr)
  {
  fprintf(stderr, "The call to free_str to be mocked!!\n");
  exit(1);
  }

int status_job(job *pjob, struct batch_request *preq, svrattrl *pal, tlist_head *pstathd, int *bad)
  {
  fprintf(stderr, "The call to status_job to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

int stat_to_mom(job *pjob, struct stat_cntl *cntl)
  {
  fprintf(stderr, "The call to stat_to_mom to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err to be mocked!!\n");
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

int unlock_queue(struct pbs_queue *the_queue, char *method_name, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }

int acl_check(attribute *pattr, char *name, int type)
  {
  fprintf(stderr, "The call to acl_check to be mocked!!\n");
  exit(1);
  }

void reply_badattr(int code, int aux, svrattrl *pal, struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_badattr to be mocked!!\n");
  exit(1);
  }
