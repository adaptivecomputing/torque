#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <limits.h> /* _POSIX_PATH_MAX */

#include "attribute.h" /* attribute_def */
#include "dynamic_string.h" /* dynamic_string */
#include "pbs_job.h"
#include "queue.h"


char path_acct[_POSIX_PATH_MAX];
attribute_def job_attr_def[10];
int LOGLEVEL=0;

int log_remove_old(char *DirPath, unsigned long ExpireTime) 
  {
  fprintf(stderr, "The call to log_remove_old needs to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

int append_dynamic_string(dynamic_string *ds, const char *to_append)
  {
  fprintf(stderr, "The call to append_dynamic_string needs to be mocked!!\n");
  exit(1);
  }

dynamic_string *get_dynamic_string(int initial_size, const char *str)
  {
  fprintf(stderr, "The call to attr_to_str needs to be mocked!!\n");
  exit(1);
  }

void free_dynamic_string(dynamic_string *ds)
  {
  fprintf(stderr, "The call to attr_to_str needs to be mocked!!\n");
  exit(1);
  }

int unlock_queue(pbs_queue *the_queue, const char *id, char *msg, int logging)
  {
  return(0);
  }

pbs_queue *get_jobs_queue(job **pjob)
  {
  return((*pjob)->ji_qhdr);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

