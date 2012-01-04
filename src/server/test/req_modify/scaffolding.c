#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attribute_def, attribute, svrattrl, compare_types */
#include "resource.h" /* resource_def, resource */
#include "server.h" /* server */
#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* jobi, job_atr */
#include "array.h" /* job_array */
#include "list_link.h" /* list_link, tlist_head */
#include "queue.h" /* pbs_queue */
#include "work_task.h" /* work_task */
#include "dynamic_string.h"

const char *PJobSubState[10];
int svr_resc_size = 0;
char *msg_mombadmodify = "MOM rejected modify request, error: %d";
char *msg_jobmod = "Job Modified";
char *msg_manager = "%s at request of %s@%s";
attribute_def job_attr_def[10];
resource_def *svr_resc_def;
const char *PJobState[] = {"hi", "hello"};
struct server server;
int LOGLEVEL = 0;

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

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr to be mocked!!\n");
  exit(1);
  }

job_array *get_array(char *id)
  {
  fprintf(stderr, "The call to get_array to be mocked!!\n");
  exit(1);
  }

void svr_disconnect(int handle)
  {
  fprintf(stderr, "The call to svr_disconnect to be mocked!!\n");
  exit(1);
  }
 
void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

char *prefix_std_file(job *pjob, dynamic_string *ds, int key)
  {
  fprintf(stderr, "The call to prefix_std_file to be mocked!!\n");
  exit(1);
  }

int attr_atomic_set(struct svrattrl *plist, attribute *old, attribute *new, attribute_def *pdef, int limit, int unkn, int privil, int *badattr)
  {
  fprintf(stderr, "The call to attr_atomic_set to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
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

int comp_resc2(struct attribute *attr, struct attribute *with, int IsQueueCentric, char *EMsg, enum compare_types type)
  {
  fprintf(stderr, "The call to comp_resc2 to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

int chk_resc_limits(attribute *pattr, pbs_queue *pque, char *EMsg)
  {
  fprintf(stderr, "The call to chk_resc_limits to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new, void *pobj)
  {
  fprintf(stderr, "The call to append_link to be mocked!!\n");
  exit(1);
  }

void set_resc_deflt(job *pjob, attribute *ji_wattr, int has_queue_mutex)
  {
  fprintf(stderr, "The call to set_resc_deflt to be mocked!!\n");
  exit(1);
  }

int relay_to_mom(job **pjob, struct batch_request *request, void (*func)(struct work_task *))
  {
  fprintf(stderr, "The call to relay_to_mom to be mocked!!\n");
  exit(1);
  }

int set_jobexid(job *pjob, attribute *attrry, char *EMsg)
  {
  fprintf(stderr, "The call to set_jobexid to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, const char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, const char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate to be mocked!!\n");
  exit(1);
  }

job *find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job to be mocked!!\n");
  exit(1);
  }

void list_move(tlist_head *from, tlist_head *to)
  {
  fprintf(stderr, "The call to list_move to be mocked!!\n");
  exit(1);
  }

void replace_attr_string(struct attribute *attr, char *newval)
  {
  fprintf(stderr, "The call to replace_attr_string to be mocked!!\n");
  exit(1);
  }

char *add_std_filename(job *pjob, char * path, int key, dynamic_string *ds)
  {
  fprintf(stderr, "The call to add_std_filename to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job *pjob, int *newstate, int *newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate to be mocked!!\n");
  exit(1);
  }

job *chk_job_request(char *jobid, struct batch_request *preq)
  {
  fprintf(stderr, "The call to chk_job_request to be mocked!!\n");
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

dynamic_string *get_dynamic_string(int initial_size, char *str)
  {
  return(NULL);
  }
