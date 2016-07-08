#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attribute_def, pbs_attribute, svrattrl */
#include "pbs_job.h" /* all_jobs */
#include "resource.h" /* resource_def */
#include "server.h" /* server */
#include "queue.h" /* pbs_queue */
#include "list_link.h" /* list_link */
#include "batch_request.h" /* batch_request */

#include "svrfunc.h" /* stat_cntl */

int svr_resc_size = 0;
attribute_def job_attr_def[10];
all_jobs alljobs;
resource_def *svr_resc_def;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
struct server server;
all_jobs array_summary;
bool exit_called = false;

pbs_queue *find_queuebyname(const char *quename)
  {
  fprintf(stderr, "The call to find_queuebyname to be mocked!!\n");
  exit(1);
  }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr to be mocked!!\n");
  exit(1);
  }

int reply_send_svr(struct batch_request *request)
  {
  fprintf(stderr, "The call to reply_send_svr to be mocked!!\n");
  exit(1);
  }

int encode_str(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_str to be mocked!!\n");
  exit(1);
  }

int svr_authorize_jobreq(struct batch_request *preq, job *pjob)
  {
  fprintf(stderr, "The call to svr_authorize_jobreq to be mocked!!\n");
  exit(1);
  }

int decode_str(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_str to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr to be mocked!!\n");
  exit(1);
  }

int set_str(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_str to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

job *next_job(all_jobs *aj, all_jobs_iterator *iter)
  {
  fprintf(stderr, "The call to next_job to be mocked!!\n");
  exit(1);
  }

void free_str(struct pbs_attribute *attr)
  {
  fprintf(stderr, "The call to free_str to be mocked!!\n");
  exit(1);
  }

int status_job(

  job           *pjob, /* ptr to job to status */
  batch_request *preq,
  svrattrl      *pal, /* specific attributes to status */
  tlist_head    *pstathd, /* RETURN: head of list to append status to */
  bool           condensed,
  int           *bad) /* RETURN: index of first bad pbs_attribute */

  {
  fprintf(stderr, "The call to status_job to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

int stat_to_mom(char *jobid, struct stat_cntl *cntl)
  {
  fprintf(stderr, "The call to stat_to_mom to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  fprintf(stderr, "The call to find_job to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }

int acl_check(pbs_attribute *pattr, char *name, int type)
  {
  fprintf(stderr, "The call to acl_check to be mocked!!\n");
  exit(1);
  }

void reply_badattr(int code, int aux, svrattrl *pal, struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_badattr to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }


int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

void set_reply_type(struct batch_reply *preply, int type)
  {
  preply->brp_choice = type;
  }
