#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h> /* pthread_mutex_t */

#include "attribute.h" /* attribute_def, pbs_attribute */
#include "pbs_job.h" /* job */
#include "array.h" /* job_array */

const char *PJobSubState[10];
char *path_jobs;
pthread_mutex_t *setup_save_mutex = NULL;
attribute_def job_attr_def[10];
int LOGLEVEL=0;



ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int save_attr(struct attribute_def *padef, struct pbs_attribute *pattr, int numattr, int fds, char *buf, size_t *buf_remaining, size_t buf_size)
  {
  fprintf(stderr, "The call to save_attr needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

job_array *get_array(char *id)
  {
  fprintf(stderr, "The call to get_array needs to be mocked!!\n");
  exit(1);
  }

int job_qs_upgrade(job *pj, int fds, char *path, int version)
  {
  fprintf(stderr, "The call to job_qs_upgrade needs to be mocked!!\n");
  exit(1);
  }

int recov_attr(int fd, void *parent, struct attribute_def *padef, struct pbs_attribute *pattr, int limit, int unknown, int do_actions)
  {
  fprintf(stderr, "The call to recov_attr needs to be mocked!!\n");
  exit(1);
  }

int job_abt(job **pjobp, char *text)
  {
  fprintf(stderr, "The call to job_abt needs to be mocked!!\n");
  exit(1);
  }

void array_get_parent_id(char *job_id, char *parent_id)
  {
  fprintf(stderr, "The call to array_get_parent_id needs to be mocked!!\n");
  exit(1);
  }

void job_free(job *pj, int use_recycle)
  {
  fprintf(stderr, "The call to job_free needs to be mocked!!\n");
  exit(1);
  }

job *job_alloc(void)
  {
  fprintf(stderr, "The call to job_alloc needs to be mocked!!\n");
  exit(1);
  }

int save_struct(char *pobj, unsigned int objsize, int fds, char *buf_ptr, size_t *space_remaining, size_t buf_size)
  {
  fprintf(stderr, "The call to save_struct needs to be mocked!!\n");
  exit(1);
  }


int lock_ss()
  {
  return(0);
  }

int unlock_ss()
  {
  return(0);
  }

int write_buffer(char *buf, int len, int fds)
  {
  return(0);
  }

int add_to_ms_list(char *node_id, job *pjob)
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
  {
  return(0);
  }
