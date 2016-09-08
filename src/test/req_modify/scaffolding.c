#include <string>
#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attribute_def, pbs_attribute, svrattrl, compare_types */
#include "resource.h" /* resource_def, resource */
#include "server.h" /* server */
#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* jobi, job_atr */
#include "array.h" /* job_array */
#include "list_link.h" /* list_link, tlist_head */
#include "queue.h" /* pbs_queue */
#include "work_task.h" /* work_task */
#include "threadpool.h"

const char *PJobSubState[10];
int svr_resc_size = 0;
const char *msg_mombadmodify = "MOM rejected modify request, error: %d";
const char *msg_jobmod = "Job Modified";
const char *msg_manager = "%s at request of %s@%s";
attribute_def job_attr_def[10];
resource_def *svr_resc_def;
const char *PJobState[] = {"hi", "hello"};
struct server server;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */

static int acl_check_n = 0;

char *get_correct_jobname_return;

struct batch_request *alloc_br(int type)
  {
  fprintf(stderr, "The call to alloc_br to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save to be mocked!!\n");
  exit(1);
  }

struct batch_request *cpy_checkpoint(struct batch_request *preq, job *pjob, enum job_atr ati, int direction)
  {
  fprintf(stderr, "The call to cpy_checkpoint to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_jobs_queue(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_queue to be mocked!!\n");
  exit(1);
  }

void cleanup_restart_file(job *pjob)
  {
  fprintf(stderr, "The call to cleanup_restart_file to be mocked!!\n");
  exit(1);
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(struct work_task *), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr to be mocked!!\n");
  exit(1);
  }

job_array *get_array(const char *id)
  {
  fprintf(stderr, "The call to get_array to be mocked!!\n");
  exit(1);
  }

void svr_disconnect(int handle)
  {
  fprintf(stderr, "The call to svr_disconnect to be mocked!!\n");
  exit(1);
  }
 
void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

const char *prefix_std_file(job *pjob, std::string& ds, int key) {return "prefix";}

int attr_atomic_set(struct svrattrl *plist, pbs_attribute *old, pbs_attribute *new_attr, attribute_def *pdef, int limit, int unkn, int privil, int *badattr)
  {
  fprintf(stderr, "The call to attr_atomic_set to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  return(NULL);
  }

int chk_hold_priv(long val, int perm)
  {
  fprintf(stderr, "The call to chk_hold_priv to be mocked!!\n");
  exit(1);
  }

int modify_array_range(job_array *pa, char *range, svrattrl *plist, struct batch_request *preq, int checkpoint_req)
  {
  fprintf(stderr, "The call to modify_array_range to be mocked!!\n");
  exit(1);
  }

int comp_resc2(struct pbs_attribute *attr, struct pbs_attribute *with, int IsQueueCentric, char *EMsg, enum compare_types type)
  {
  fprintf(stderr, "The call to comp_resc2 to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

int chk_resc_limits(pbs_attribute *pattr, pbs_queue *pque, char *EMsg)
  {
  fprintf(stderr, "The call to chk_resc_limits to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  fprintf(stderr, "The call to append_link to be mocked!!\n");
  exit(1);
  }

void set_resc_deflt(job *pjob, pbs_attribute *ji_wattr, int has_queue_mutex)
  {
  fprintf(stderr, "The call to set_resc_deflt to be mocked!!\n");
  exit(1);
  }

int set_jobexid(job *pjob, pbs_attribute *attrry, char *EMsg)
  {
  fprintf(stderr, "The call to set_jobexid to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  fprintf(stderr, "The call to find_job to be mocked!!\n");
  exit(1);
  }

void list_move(tlist_head *from, tlist_head *to)
  {
  fprintf(stderr, "The call to list_move to be mocked!!\n");
  exit(1);
  }

void replace_attr_string(struct pbs_attribute *attr, char *newval)
  {
  fprintf(stderr, "The call to replace_attr_string to be mocked!!\n");
  exit(1);
  }

const char *add_std_filename(job *pjob, char *path, int key, std::string& ds) { return "stdfilename"; }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job &pjob, int &newstate, int &newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate to be mocked!!\n");
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

int get_svr_attr_b(int index, bool *b)
  {
  return(0);
  }

batch_request *get_remove_batch_request(

  char *br_id)

  {
  return(NULL);
  }

int get_batch_request_id(

  batch_request *preq)

  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int unlock_ai_mutex(job_array *pa, const char *id, const char *msg, int logging)
  {
  return(0);
  }

job_array *get_jobs_array(job **pjob_ptr)
  {
  return(NULL);
  }

threadpool_t *request_pool;
threadpool_t *async_pool;

int enqueue_threadpool_request(void *(*func)(void *), void *arg, threadpool_t *tp)

  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

int relay_to_mom(job **pjob_ptr, batch_request   *request, void (*func)(struct work_task *))
  {
  return(0);
  }

batch_request *duplicate_request(batch_request *preq, int type) 
  {
  return(NULL);
  }

char *get_correct_jobname(char const *p)
  {
  if (get_correct_jobname_return == NULL)
    return(strdup(p));

  return(get_correct_jobname_return);
  }

job *chk_job_request(char *p, batch_request *b)
  {
  job *jp;

  jp = (job *)calloc(1, sizeof(job));
  return(jp);
  }


void reply_ack(struct batch_request *preq) {}

void overwrite_complete_req(

  pbs_attribute *attr,
  pbs_attribute *new_attr) {}

int acl_check(pbs_attribute *pattr, char *name, int type)
  {
  bool rc;

  switch(acl_check_n)
    {
    case 0:
      rc = true;
      break;

    case 1:
      rc = false;
      break;

    case 2:
      rc = true;
      break;

    default:
      rc = false;
   }

  acl_check_n++;

  return(rc);
  }

void update_slot_held_jobs(job_array *pa, int num_to_release) {}
