#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attribute_def, pbs_attribute */
#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */
#include "array.h" /* job_array.h */
#include "list_link.h" /* list_link */
#include "work_task.h" /* work_task */

int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
attribute_def job_attr_def[10];
const char *msg_jobholdrel = "Holds %s released at request of %s@%s";
const char *msg_mombadhold = "MOM rejected hold request: %d";
const char *msg_postmomnojob = "Job not found after hold reply from MOM";
const char *msg_jobholdset = "Holds %s set at request of %s@%s";

void account_record(int acctype, job *pjob, const char *text)
  {
  fprintf(stderr, "The call to account_record to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save to be mocked!!\n");
  exit(1);
  }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

 int svr_authorize_jobreq(struct batch_request *preq, job *pjob)
  {
  fprintf(stderr, "The call to svr_authorize_jobreq to be mocked!!\n");
  exit(1);
  }

int release_array_range(job_array *pa, struct batch_request *preq, char *range_str)
  {
  fprintf(stderr, "The call to release_array_range to be mocked!!\n");
  exit(1);
  }

job_array *get_array(const char *id)
  {
  fprintf(stderr, "The call to get_array to be mocked!!\n");
  exit(1);
  }

void svr_disconnect(int handle)
  {
  fprintf(stderr, "The call to svr_disconnect to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
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

job *svr_find_job(const char *jobid, int get_subjob)
  {
  fprintf(stderr, "The call to find_job to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job &pjob, int &newstate, int &newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate to be mocked!!\n");
  exit(1);
  }

job *chk_job_request(char *jobid, struct batch_request *preq)
  {
  fprintf(stderr, "The call to chk_job_request to be mocked!!\n");
  exit(1);
  }

int first_job_index(job_array *pa)
  {
  fprintf(stderr, "The call to first_job_index to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

int copy_batchrequest(struct batch_request **newreq, struct batch_request *preq, int type, int jobid)
  {
  return(0);
  }

batch_request *get_remove_batch_request(

  char *br_id)

  {
  return(NULL);
  }

int get_batch_request_id(

  batch_request *preq)

  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int unlock_ai_mutex(job_array *pa, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void free_br(batch_request *preq) {}

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

int relay_to_mom(job **pjob_ptr, batch_request   *request, void (*func)(struct work_task *))
  {
  return(0);
  }

batch_request *duplicate_request(batch_request *preq, int type) 
  {
  return(NULL);
  }

int check_array_slot_limits(job *pjob, job_array *pa)
  {
  return(0);
  }
