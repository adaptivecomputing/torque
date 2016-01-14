#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h> /* pthread_mutex_t */
#include <unistd.h>


#include "attribute.h" /* attribute_def, pbs_attribute */
#include "queue.h" /* pbs_queue */

pthread_mutex_t *setup_save_mutex = NULL;
char *path_queues;
attribute_def que_attr_def[10];

void recov_acl(pbs_attribute *pattr, attribute_def *pdef, const char *subdir, const char *name)
  {
  fprintf(stderr, "The call to recov_acl needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int is_whitespace(

  char c)

  {
  if ((c == ' ')  ||
      (c == '\n') ||
      (c == '\t') ||
      (c == '\r') ||
      (c == '\f'))
    return(TRUE);
  else
    return(FALSE);
  } /* END is_whitespace */


int write_buffer(char *buf, int len, int fds)
  {
  int l = write(fds,buf,len);
  if(l == len) return 0;
  return -1;
  }

pbs_queue *que_alloc(const char *name, int sv_qs_mutex_held)
  {
  pbs_queue *pq;

  pq = (pbs_queue *)calloc(1, sizeof(pbs_queue));
  snprintf(pq->qu_qs.qu_name, sizeof(pq->qu_qs.qu_name), "%s", name);

  return pq;
  }

int recov_attr(int fd, void *parent, struct attribute_def *padef, struct pbs_attribute *pattr, int limit, int unknown, int do_actions)
  {
  fprintf(stderr, "The call to recov_attr needs to be mocked!!\n");
  exit(1);
  }

int str_to_attr(const char *name, char *val, struct pbs_attribute *attr, struct attribute_def *padef, int limit)
  {
  return(0);
  }

void que_free(pbs_queue *pq, int sv_qs_mutex_held)
  {
  fprintf(stderr, "The call to que_free needs to be mocked!!\n");
  exit(1);
  }

int save_attr_xml(struct attribute_def *padef, struct pbs_attribute *pattr, int numattr, int fds)
  {
  const char *p = "<attributes>\n</attributes>\n";
  write(fds,p,strlen(p));
  return 0;
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return read(fd,buf,count);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

char *trim(

  char *str) /* M */

  {
  char *front_ptr;
  char *end_ptr;

  if (str == NULL)
    return(NULL);

   front_ptr = str;
   end_ptr = str + strlen(str) - 1;

   while ((is_whitespace(*front_ptr) == TRUE) &&
          (front_ptr <= end_ptr))
    {
    front_ptr++;
    }

  while ((is_whitespace(*end_ptr) == TRUE) &&
         (end_ptr > front_ptr))
    {
    *end_ptr = '\0';
    end_ptr--;
    }

  return(front_ptr);
  } /* END trim() */


