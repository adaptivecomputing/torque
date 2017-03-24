#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "server.h" /* server mail_info*/
#include "threadpool.h"
#include "resource.h"
#include "queue.h"
#include "dynamic_string.h" /* dynamic_string */
#include "pbs_job.h"
#include "list_link.h" /* list_link */
#include "mutex_mgr.hpp"
#include "sched_cmds.h" /* SCH_SCHEDULE_NULL */
#include "attribute.h" /* svrattrl */
#include "work_task.h"
#include "mail_throttler.hpp"

extern void *send_the_mail(void *vp);

pthread_mutex_t *scheduler_sock_jobct_mutex;
int scheduler_sock=0;
attribute_def svr_attr_def[10];
int  svr_do_schedule = SCH_SCHEDULE_NULL;
int listener_command = SCH_SCHEDULE_NULL;
pthread_mutex_t *svr_do_schedule_mutex;
pthread_mutex_t *listener_command_mutex;
int listening_socket;
threadpool_t *task_pool;
int called;
bool get_svr_attr_str_return_svr_sm_attr = false;

int LOGLEVEL = 7; /* force logging code to be exercised as tests run */

int get_svr_attr_l(int attr_index, long *l)
  {
	called = 1;
	return 0;
  }  

int enqueue_threadpool_request(void *(*func)(void *), void *arg, threadpool_t *tp)
  {
  send_the_mail((mail_info *)arg);
  return 0;
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf) 
  { 
  fprintf(stderr, "The call to find_ersc_entry need to be mocked!!\n");
  exit(1);
  }

svrattrl *attrlist_create(const char *aname, const char *rname, int vsize)
  {
  fprintf(stderr, "The call to attrlist_create needs to be mocked!!\n");
  exit(1);
  }

char *csv_nth(const char *csv_str, int n)
  {
  fprintf(stderr, "The call to csv_nth to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_jobs_queue(job **pjob)
  {
  return((*pjob)->ji_qhdr);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

int csv_length(const char *csv_str)
  {
  fprintf(stderr, "The call to csv_length to be mocked!!\n");
  exit(1);
  }

resource *add_resource_entry(pbs_attribute *pattr, resource_def *prdef)
  {
  fprintf(stderr, "The call to add_resource_entry needs to be mocked!!\n");
  exit(1);
  }

mutex_mgr::mutex_mgr(pthread_mutex_t *, bool a)
  {
  }

int mutex_mgr::unlock()
  {
  return(0);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(struct work_task *), void *parm, int get_lock)
  {
  return(NULL);
  }

void mutex_mgr::mark_as_locked() {}

int unlock_ji_mutex(

  job        *pjob,
  const char *id,
  const char *msg,
  int        logging)

  {
  return(0);
  }


mutex_mgr::~mutex_mgr() {}

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

bool empty_body = false;

int get_svr_attr_str(
    
  int    index,
  char **str)

  {
  static char *bodyfmt = strdup("Zaphod");

  if (get_svr_attr_str_return_svr_sm_attr)
    {
    *str = server.sv_attr[SRV_ATR_SendmailPath].at_val.at_str;
    return(0);
    }

  if ((index == SRV_ATR_MailBodyFmt) &&
      (empty_body == true))
    {
    *str = bodyfmt;
    }

  return(0);
  }

