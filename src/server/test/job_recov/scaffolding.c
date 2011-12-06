#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h> /* pthread_mutex_t */

#include "attribute.h" /* attribute_def, attribute */
#include "pbs_job.h" /* job */
#include "array.h" /* job_array */

const char *PJobSubState[10];
char *path_jobs;
pthread_mutex_t *setup_save_mutex = NULL;
attribute_def job_attr_def[10];



ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int save_attr(struct attribute_def *padef, struct attribute *pattr, int numattr, int fds, char *buf, size_t *buf_remaining, size_t buf_size)
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

int recov_attr(int fd, void *parent, struct attribute_def *padef, struct attribute *pattr, int limit, int unknown, int do_actions)
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

void log_event(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

void job_free(job *pj)
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



