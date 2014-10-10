#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "queue.h" /* all_queues */
#include "server.h" /* server */
#include "attribute.h" /* pbs_attribute */
#include "pbs_job.h" /* job */
#include "user_info.h"

const char *msg_err_unlink = "Unlink of %s file %s failed";
all_queues svr_queues;
attribute_def que_attr_def[10];
struct server server;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
char *path_queues;
bool exit_called = false;

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr needs to be mocked!!\n");
  exit(1);
  }

int save_acl(pbs_attribute *attr, attribute_def *pdef, const char *subdir, const char *name)
  {
  fprintf(stderr, "The call to save_acl needs to be mocked!!\n");
  exit(1);
  }

int insert_into_queue_recycler(pbs_queue *pq)
  {
  return(0);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

int unlock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

int get_svr_attr_str(int index, char **str)
  {
  return(0);
  }

int lock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void initialize_user_info_holder(user_info_holder *uih) {}

void free_user_info_holder(user_info_holder *uih) {} 

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
