#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h> /* pthread_mutex_t */


#include "attribute.h" /* attribute_def, pbs_attribute */
#include "queue.h" /* pbs_queue */

pthread_mutex_t *setup_save_mutex = NULL;
char *path_queues;
attribute_def que_attr_def[10];

void recov_acl(pbs_attribute *pattr, attribute_def *pdef, char *subdir, char *name)
  {
  fprintf(stderr, "The call to recov_acl needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int get_parent_and_child(char *start, char **parent, char **child, char **end)
  {
  fprintf(stderr, "The call to get_parent_and_child needs to be mocked!!\n");
  exit(1);
  }

int write_buffer(char *buf, int len, int fds)
  {
  fprintf(stderr, "The call to write_buffer needs to be mocked!!\n");
  exit(1);
  }

pbs_queue *que_alloc(char *name, int sv_qs_mutex_held)
  {
  fprintf(stderr, "The call to que_alloc needs to be mocked!!\n");
  exit(1);
  }

int recov_attr(int fd, void *parent, struct attribute_def *padef, struct pbs_attribute *pattr, int limit, int unknown, int do_actions)
  {
  fprintf(stderr, "The call to recov_attr needs to be mocked!!\n");
  exit(1);
  }

int str_to_attr(char *name, char *val, struct pbs_attribute *attr, struct attribute_def *padef)
  {
  fprintf(stderr, "The call to str_to_attr needs to be mocked!!\n");
  exit(1);
  }

void que_free(pbs_queue *pq, int sv_qs_mutex_held)
  {
  fprintf(stderr, "The call to que_free needs to be mocked!!\n");
  exit(1);
  }

int save_attr_xml(struct attribute_def *padef, struct pbs_attribute *pattr, int numattr, int fds)
  {
  fprintf(stderr, "The call to save_attr_xml needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, const char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

 void log_err(int errnum, const char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }


