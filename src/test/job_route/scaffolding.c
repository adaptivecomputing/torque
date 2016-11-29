#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "pbs_job.h" /* job */
#include "list_link.h" /* list_link, tlist_head */
#include "queue.h" /* pbs_queue */
#include "batch_request.h" /* batach_request */
#include "resource.h"
#include "log.h"
#include "mutex_mgr.hpp"

const char *msg_err_noqueue = "Unable to requeue job, queue is not defined";
const char *msg_err_malloc = "malloc failed";
const char *msg_routexceed = "Route queue lifetime exceeded";
int svr_resc_size = 0;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
resource_def *svr_resc_def;
pthread_mutex_t *reroute_job_mutex;
int                     route_retry_interval = 5; /* time in seconds to check routing queues */
int pbs_errno;
char log_buffer[LOG_BUF_SIZE];
bool exit_called;


#ifndef NDEBUG

void *get_next(

  list_link  pl,   /* I */
  char     *file, /* I */
  int      line) /* I */

  {
  if ((pl.ll_next == NULL) ||
      ((pl.ll_next == &pl) && (pl.ll_struct != NULL)))
    {
    fprintf(stderr, "Assertion failed, bad pointer in link: file \"%s\", line %d\n",
            file,
            line);

    return NULL;
    }

  return(pl.ll_next->ll_struct);
  }  /* END get_next() */

#else
#endif


void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
#ifndef NDEBUG
  /* first make sure unlinked entries are pointing to themselves     */

  if ((pobj == NULL) ||
      (head->ll_prior == NULL) ||
      (head->ll_next  == NULL) ||
      (new_link->ll_prior  != (list_link *)new_link) ||
      (new_link->ll_next   != (list_link *)new_link))
    {
    if (pobj == NULL)
      fprintf(stderr, "ERROR:  bad pobj pointer in append_link\n");

    if (head->ll_prior == NULL)
      fprintf(stderr, "ERROR:  bad head->ll_prior pointer in append_link\n");

    if (head->ll_next == NULL)
      fprintf(stderr, "ERROR:  bad head->ll_next pointer in append_link\n");

    if (new_link->ll_prior == NULL)
      fprintf(stderr, "ERROR:  bad new->ll_prior pointer in append_link\n");

    if (new_link->ll_next == NULL)
      fprintf(stderr, "ERROR:  bad new->ll_next pointer in append_link\n");

    abort();
    }  /* END if ((pobj == NULL) || ...) */


#endif  /* NDEBUG */

  /*
   * its big trouble if ll_struct is null, it would make this
   * entry appear to be the head, so we never let that happen
   */

  if (pobj != NULL)
    {
    new_link->ll_struct = pobj;
    }
  else
    {
    /* WARNING: This mixes list_link pointers and ll_struct
         pointers, and may break if the list_link we are operating
         on is not the first embeded list_link in the surrounding
         structure, e.g. work_task.wt_link_obj */

    new_link->ll_struct = (void *)new_link;
    }

  new_link->ll_prior = head->ll_prior;

  new_link->ll_next  = head;
  head->ll_prior = new_link;
  new_link->ll_prior->ll_next = new_link; /* now visible to forward iteration */
  }

resource *add_resource_entry(pbs_attribute *pattr, resource_def *prdef)
  {
  return(NULL);
  }

long count_proc(const char *param_spec)

  {
  return(0);
  }

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  return(NULL);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  return(NULL);
  }

int lock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

pbs_queue *find_queuebyname(const char *quename)
  {
  return(NULL);
  }

void log_err(int errnum, const char *routine, const char *text)
  {
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

pbs_queue *get_jobs_queue(job **pjob_ptr)
  {
  return(NULL);
  }

job *next_job(all_jobs *aj, all_jobs_iterator *iter)
  {
  fprintf(stderr, "The call to next_job to be mocked!!\n");
  exit(1);
  }

int job_abt(struct job **pjobp, const char *text, bool b=false)
  {
  fprintf(stderr, "The call to job_abt needs to be mocked!!\n");
  exit(1);
  }


