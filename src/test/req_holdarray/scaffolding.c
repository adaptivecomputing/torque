#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "batch_request.h" /* batch_request */
#include "list_link.h" /* tlist_head */
#include "array.h" /* job_array */
#include "attribute.h" /* pbs_attribute */
#include "pbs_job.h" /* job */

const char *msg_permlog = "Unauthorized Request, request type: %d, Object: %s, Name: %s, request from: %s@%s";
int LOGLEVEL=0;

int svr_authorize_req(struct batch_request *preq, char *owner, char *submit_host)
  {
  fprintf(stderr, "The call to svr_authorize_req to be mocked!!\n");
  exit(1);
  }

int get_hold(tlist_head *phead, const char **pset, pbs_attribute *temphold)
  {
  fprintf(stderr, "The call to get_hold to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

int hold_array_range(job_array *pa, char *range_str, pbs_attribute *temphold)
  {
  fprintf(stderr, "The call to hold_array_range to be mocked!!\n");
  exit(1);
  }

job_array *get_array(const char *id)
  {
  return(NULL);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

int chk_hold_priv(long val, int perm)
  {
  fprintf(stderr, "The call to chk_hold_priv to be mocked!!\n");
  exit(1);
  }

char * csv_find_string(const char *csv_str, const char *search_str)
  {
  fprintf(stderr, "The call to csv_find_string to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job &pjob, int &newstate, int &newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate to be mocked!!\n");
  exit(1);
  }

void get_jobowner(char *from, char *to)
  {
  fprintf(stderr, "The call to get_jobowner to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  return(NULL);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int unlock_ai_mutex(job_array *pa, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
