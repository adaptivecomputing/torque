#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */
#include "attribute.h" /* pbs_attribute */
#include "list_link.h" /* list_link */
#include "work_task.h" /* work_task */
#include "array.h" /* job_array */
#include "server.h" /* server */

const char *text_name              = "text";

bool exit_called = false;

char *path_arrays;
const char *pbs_o_host = "PBS_O_HOST";
struct server server;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
int array_259_upgrade = 0;
bool place_hold;
int  unlocked;

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int copy_batchrequest(struct batch_request **newreq, struct batch_request *preq, int type, int jobid)
  {
  fprintf(stderr, "The call to copy_batchrequest needs to be mocked!!\n");
  exit(1);
  }

void hold_job(pbs_attribute *temphold, void *j)
  {
  fprintf(stderr, "The call to hold_job needs to be mocked!!\n");
  exit(1);
  }

int release_job(struct batch_request *preq, void *j, job_array *pa)
  {
  return(0);
  }

void post_modify_arrayreq(batch_request *br)
  {
  fprintf(stderr, "The call to post_modify_arrayreq needs to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  if(old->ll_prior == NULL) return;
  old->ll_prior->ll_next = old->ll_next;
  if(old->ll_next == NULL) return;
  old->ll_next->ll_prior = old->ll_prior;
  }

char *get_variable(job *pjob, const char *variable)
  {
  fprintf(stderr, "The call to get_variable needs to be mocked!!\n");
  exit(1);
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int modify_job(void **j, svrattrl *plist, struct batch_request *preq, int checkpoint_req, int flag)
  {
  fprintf(stderr, "The call to modify_job needs to be mocked!!\n");
  exit(1);
  }

job *next_job(all_jobs *aj, all_jobs_iterator *iter)
  {
  fprintf(stderr, "The call to next_job needs to be mocked!!\n");
  exit(1);
  }

int attempt_delete(void *j)
  {
  fprintf(stderr, "The call to attempt_delete needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  }

bool set_array_depend_holds(job_array *pa)
  {
  return(false);
  }

int array_upgrade(job_array *pa, int fds, int version, int *old_version)
  {
  fprintf(stderr, "The call to array_upgrade needs to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate needs to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job &pjob, int &newstate, int &newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate needs to be mocked!!\n");
  exit(1);
  }

const char *get_correct_jobname(const char *jobid, std::string &correct)
  {
  correct = jobid;
  return(correct.c_str());
  }

void *get_prior(list_link pl, char *file, int line)
  {
  return(NULL);
  }

void insert_link(struct list_link *old, struct list_link *new_link, void *pobj, int position)
  {
  return;
  }

void *get_next(list_link pl, char *file, int line)
  {
  if (pl.ll_next == NULL)
    return(NULL);

  return(pl.ll_next->ll_struct);
  }

char *threadsafe_tokenizer(char **str, const char *delims)
  {
  char *current_char;
  char *start;

  if ((str == NULL) ||
      (*str == NULL))
    return(NULL);

  /* save start position */
  start = *str;

  /* return NULL at the end of the string */
  if (*start == '\0')
    return(NULL);

  /* begin at the start */
  current_char = start;

  /* advance to the end of the string or until you find a delimiter */
  while ((*current_char != '\0') &&
         (!strchr(delims, *current_char)))
    current_char++;

  /* advance str */
  if (*current_char != '\0')
    {
    /* not at the end of the string */
    *str = current_char + 1;
    *current_char = '\0';
    }
  else
    {
    /* at the end of the string */
    *str = current_char;
    }

  return(start);
  }

int get_svr_attr_l(int attr_index, long *l)
  {
  static int count = 0;

  if (attr_index == SRV_ATR_MaxSlotLimit)
    {
    count++;

    if (count == 1)
      return(-1);
    
    *l = 5;
    }

  return(0);
  }

int is_svr_attr_set(int attr_index)
  {
  return(0);
  }

job *svr_find_job(const char *name, int get_subjob)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  strcpy(pjob->ji_qs.ji_jobid, name);

  if (place_hold)
    pjob->ji_wattr[JOB_ATR_hold].at_val.at_long = HOLD_l;

  return(pjob);
  }

int svr_job_purge(job *pjob, int leaveSpoolFiles)
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  unlocked++;
  return(0);
  }

int unlock_ai_mutex(

  job_array  *pa,
  const char *id,
  const char       *msg,
  int        logging)

  {
  return(0);
  }

job_array *get_jobs_array(

  job **pjob_ptr)

  {
  return(NULL);
  }

int lock_ai_mutex(

  job_array  *pa,
  const char *id,
  const char       *msg,
  int        logging)

  {
  return(0);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
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

batch_request *duplicate_request(batch_request *preq, int job_index)
  {
  return NULL;
  }

int lock_ss()
    {
    return(0);
    }

int unlock_ss()
    {
    return(0);
    }

std::string get_path_jobdata(const char *a, const char *b) {return ""; }
