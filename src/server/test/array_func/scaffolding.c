#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "resizable_array.h" /* resizable_array */
#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */
#include "attribute.h" /* pbs_attribute */
#include "list_link.h" /* list_link */
#include "work_task.h" /* work_task */
#include "array.h" /* job_array */
#include "server.h" /* server */



char *path_arrays;
char *pbs_o_host = "PBS_O_HOST";
struct server server;
int LOGLEVEL = 0;
int array_259_upgrade = 0;

int insert_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to insert_thing needs to be mocked!!\n");
  exit(1);
  }

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

int remove_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to remove_thing needs to be mocked!!\n");
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

int release_job(struct batch_request *preq, void *j)
  {
  fprintf(stderr, "The call to release_job needs to be mocked!!\n");
  exit(1);
  }

void post_modify_arrayreq(struct work_task *pwt)
  {
  fprintf(stderr, "The call to post_modify_arrayreq needs to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  }

char *get_variable(job *pjob, char *variable)
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

job *next_job(struct all_jobs *aj, int *iter)
  {
  fprintf(stderr, "The call to next_job needs to be mocked!!\n");
  exit(1);
  }

resizable_array *initialize_resizable_array(int size)
  {
  fprintf(stderr, "The call to initialize_resizable_array needs to be mocked!!\n");
  exit(1);
  }

int attempt_delete(void *j)
  {
  fprintf(stderr, "The call to attempt_delete needs to be mocked!!\n");
  exit(1);
  }

void *next_thing(resizable_array *ra, int *iter)
  {
  fprintf(stderr, "The call to next_thing needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

void set_array_depend_holds(job_array *pa)
  {
  fprintf(stderr, "The call to set_array_depend_holds needs to be mocked!!\n");
  exit(1);
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

void svr_evaljobstate(job *pjob, int *newstate, int *newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate needs to be mocked!!\n");
  exit(1);
  }

char *get_correct_jobname(const char *jobid)
  {
  fprintf(stderr, "The call to get_correct_jobname needs to be mocked!!\n");
  exit(1);
  }

void *get_prior(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_prior needs to be mocked!!\n");
  exit(1);
  }

void insert_link(struct list_link *old, struct list_link *new, void *pobj, int position)
  {
  fprintf(stderr, "The call to insert_link needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

char *threadsafe_tokenizer(char **str, char *delims)
  {
  fprintf(stderr, "The call to threadsafe_tokenizer needs to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int attr_index, long *l)
  {
  return(0);
  }

int is_svr_attr_set(int attr_index)
  {
  return(0);
  }

job *svr_find_job(char *name, int get_subjob)
  {
  return(NULL);
  }

int svr_job_purge(job *pjob)
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
  {
  return(0);
  }

int unlock_ai_mutex(

  job_array  *pa,
  const char *id,
  char       *msg,
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
  char       *msg,
  int        logging)

  {
  return(0);
  }

