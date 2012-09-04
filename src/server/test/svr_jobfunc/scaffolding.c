#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h> /* pthread */

#include "queue.h" /* all_queues, pbs_queue */
#include "attribute.h" /* pbs_attribute */
#include "pbs_job.h" /* all_jobs, job */
#include "resource.h" /* resouce */
#include "server.h" /* server */
#include "work_task.h" /* work_task */
#include "sched_cmds.h" /* SCH_SCHEDULE_NULL */
#include "list_link.h" /* list_link */


int svr_resc_size = 0;
all_queues svr_queues;
char *msg_daemonname = "unset";
attribute_def job_attr_def[10];
char *msg_badwait = "Invalid time in work task for waiting, job = %s";
struct all_jobs alljobs;
char *pbs_o_host = "PBS_O_HOST";
resource_def *svr_resc_def;
int svr_clnodes = 0;
int comp_resc_gt; 
struct server server;
struct all_jobs array_summary;
int svr_do_schedule = SCH_SCHEDULE_NULL;
int listener_command = SCH_SCHEDULE_NULL;
int LOGLEVEL = 0;
pthread_mutex_t *svr_do_schedule_mutex;
pthread_mutex_t *listener_command_mutex;


resource *add_resource_entry(pbs_attribute *pattr, resource_def *prdef)
  {
  fprintf(stderr, "The call to add_resource_entry to be mocked!!\n");
  exit(1);
  }

pbs_queue *find_queuebyname(char *quename)
  {
  fprintf(stderr, "The call to find_queuebyname to be mocked!!\n");
  exit(1);
  }

void account_record(int acctype, job *pjob, char *text)
  {
  fprintf(stderr, "The call to account_record to be mocked!!\n");
  exit(1);
  }

char *arst_string(char *str, pbs_attribute *pattr)
  {
  fprintf(stderr, "The call to arst_string to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save to be mocked!!\n");
  exit(1);
  }

long attr_ifelse_long(pbs_attribute *attr1, pbs_attribute *attr2, long deflong)
  {
  fprintf(stderr, "The call to attr_ifelse_long to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_jobs_queue(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_queue to be mocked!!\n");
  exit(1);
  }

int procs_available(int proc_ct)
  {
  fprintf(stderr, "The call to procs_available to be mocked!!\n");
  exit(1);
  }

int decode_str(pbs_attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_str to be mocked!!\n");
  exit(1);
  }

int depend_on_que(pbs_attribute *pattr, void *pjob, int mode)
  {
  fprintf(stderr, "The call to depend_on_que to be mocked!!\n");
  exit(1);
  }

int insert_job_after(struct all_jobs *aj, job *already_in, job *pjob)
  {
  fprintf(stderr, "The call to insert_job_after to be mocked!!\n");
  exit(1);
  }

int has_job(struct all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to has_job to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task to be mocked!!\n");
  exit(1);
  }

int insert_job(struct all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to insert_job to be mocked!!\n");
  exit(1);
  }

int procs_requested(char *spec)
  {
  fprintf(stderr, "The call to procs_requested to be mocked!!\n");
  exit(1);
  }

int num_array_jobs(char *req_str)
  {
  fprintf(stderr, "The call to num_array_jobs to be mocked!!\n");
  exit(1);
  }

job *next_job(struct all_jobs *aj, int *iter)
  {
  fprintf(stderr, "The call to next_job to be mocked!!\n");
  exit(1);
  }

job *next_job_from_back(struct all_jobs *aj, int *iter)
  {
  fprintf(stderr, "The call to next_job_from_back to be mocked!!\n");
  exit(1);
  }

int comp_resc2(struct pbs_attribute *attr, struct pbs_attribute *with, int IsQueueCentric, char *EMsg, enum compare_types type)
  {
  fprintf(stderr, "The call to comp_resc2 to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

char * csv_find_string(char *csv_str, char *search_str)
  {
  fprintf(stderr, "The call to csv_find_string to be mocked!!\n");
  exit(1);
  }

int node_avail_complex(char *spec, int *navail, int *nalloc, int *nresvd, int *ndown)
  {
  fprintf(stderr, "The call to node_avail_complex to be mocked!!\n");
  exit(1);
  }

work_task *next_task(all_tasks *at, int *iter)
  {
  fprintf(stderr, "The call to next_task to be mocked!!\n");
  exit(1);
  }

pbs_queue *next_queue(all_queues *aq, int *iter)
  {
  fprintf(stderr, "The call to next_queue to be mocked!!\n");
  exit(1);
  }

int remove_job(struct all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to remove_job to be mocked!!\n");
  exit(1);
  }

int set_jobexid(job *pjob, pbs_attribute *attrry, char *EMsg)
  {
  fprintf(stderr, "The call to set_jobexid to be mocked!!\n");
  exit(1);
  }

int site_acl_check(job *pjob, pbs_queue *pque)
  {
  fprintf(stderr, "The call to site_acl_check to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(char *jobid, int get_subjob)
  {
  fprintf(stderr, "The call to find_job to be mocked!!\n");
  exit(1);
  }

int insert_job_first(struct all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to insert_job_first to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *id, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }

int acl_check(pbs_attribute *pattr, char *name, int type)
  {
  fprintf(stderr, "The call to acl_check to be mocked!!\n");
  exit(1);
  }

int insert_task(all_tasks *at, work_task *wt)
  {
  fprintf(stderr, "The call to insert_task to be mocked!!\n");
  exit(1);
  }

void free_str(struct pbs_attribute *attr)
  {
  fprintf(stderr, "The call to free_str to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

int append_dynamic_string (dynamic_string *ds, const char *str)
  {
  return(0);
  }

int unlock_startup() 
  {
  return(0);
  }

int lock_startup()
  {
  return(0);
  }

int insert_job_after_index(struct all_jobs *aj, int index, job *pjob)
  {
  return(0);
  }

int append_char_to_dynamic_string (dynamic_string *ds, char c)
  {
  return(0);
  }

int unlock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

int lock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

unsigned int get_num_queued(user_info_holder *uih, char *user_name)
  {
  return(0);
  }

int  increment_queued_jobs(
   
  user_info_holder *uih,
  char             *user_name,
  job              *pjob)

  {
  return(0);
  }

int decrement_queued_jobs(
  
  user_info_holder *uih,
  char             *user_name)

  {
  return(0);
  }
