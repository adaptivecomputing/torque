#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */
#include "work_task.h" /* work_task */

const char *msg_messagejob = "Message request to job status %d";
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

void svr_disconnect(int handle)
  {
  fprintf(stderr, "The call to svr_disconnect to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

job *chk_job_request(char *jobid, struct batch_request *preq)
  {
  fprintf(stderr, "The call to chk_job_request to be mocked!!\n");
  exit(1);
  }

int copy_batchrequest(struct batch_request **newreq, struct batch_request *preq, int type, int jobid)
  {
  return(0);
  }

void free_br(struct batch_request *preq) {}

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

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

int relay_to_mom(job **pjob_ptr, batch_request   *request, void (*func)(struct work_task *))
  {
  return(0);
  }

batch_request *duplicate_request(batch_request *preq, int type) 
  {
  return(NULL);
  }
