#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h> /* pthread_mutex_t */

#include "sched_cmds.h" /* SCH_SCHEDULE_NULL */
#include "server.h" /* server */
#include "resource.h" /* resource */
#include "attribute.h" /* attribute */
#include "pbs_job.h" /* job */
#include "queue.h" /* pbs_queue */
#include "list_link.h" /* list_link */

struct server server;
int scheduler_sock = -1;
int svr_do_schedule = SCH_SCHEDULE_NULL;
int listener_command = SCH_SCHEDULE_NULL;
int LOGLEVEL = 0;
pthread_mutex_t *svr_do_schedule_mutex;
pthread_mutex_t *listener_command_mutex;

resource *add_resource_entry(attribute *pattr, resource_def *prdef)
  {
  fprintf(stderr, "The call to add_resource_entry to be mocked!!\n");
  exit(1);
  }

svrattrl *attrlist_create(char *aname, char *rname, int vsize)
  {
  fprintf(stderr, "The call to attrlist_create to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_jobs_queue(job *pjob)
  {
  fprintf(stderr, "The call to get_jobs_queue to be mocked!!\n");
  exit(1);
  }

int csv_length(char *csv_str)
  {
  fprintf(stderr, "The call to csv_length to be mocked!!\n");
  exit(1);
  }

char *csv_nth(char *csv_str, int n)
  {
  fprintf(stderr, "The call to csv_nth to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new, void *pobj)
  {
  fprintf(stderr, "The call to append_link to be mocked!!\n");
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

int unlock_queue(struct pbs_queue *the_queue, char *id, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }
