#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */
#include "node_func.h" /* node_info */
#include "attribute.h"

bool find_job = true;
bool nullify_pointer = false;
int  unlocked_job = 0;
int  found_job = 0;
int  relay_rc = 0;

attribute_def job_attr_def[10];

int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
char scaff_buffer[1024];

batch_request *alloc_br(int type)
  {
  static batch_request preq;
  return(&preq);
  }

void set_old_nodes(job *pjob)
  {
  fprintf(stderr, "The call to set_old_nodes to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save to be mocked!!\n");
  exit(1);
  }

void set_statechar(job *pjob)
  {
  fprintf(stderr, "The call to set_statechar to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

void free_nodes(job *pjob, const char *spec) { }

void svr_disconnect(int handle)
  {
  fprintf(stderr, "The call to svr_disconnect to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  static job pjob;

  if (find_job)
    {
    found_job++;
    strcpy(pjob.ji_qs.ji_jobid, jobid);
    return(&pjob);
    }

  return(NULL);
  }



job *chk_job_request(char *jobid, struct batch_request *preq)
  {
  //We're mocking so that the following field has the address of a job struct.
  return (job *) atol(preq->rq_ind.rq_signal.rq_jid);
  }

int copy_batchrequest(struct batch_request **newreq, struct batch_request *preq, int type, int jobid)
  {
  return(0);
  }

void free_br(struct batch_request *b) {}

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
  unlocked_job++;

  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

void log_record(int eventtype, int objclass, const char *objname, const char *text)
  {
  snprintf(scaff_buffer, sizeof(scaff_buffer), "%s", text);
  }

int relay_to_mom(job **pjob_ptr, batch_request   *request, void (*func)(struct work_task *))
  {
  if (nullify_pointer)
    *pjob_ptr = NULL;

  return(relay_rc);
  }

batch_request *duplicate_request(batch_request *preq, int type) 
  {
  return(NULL);
  }

int pbs_getaddrinfo(const char *hostname, struct addrinfo *in, struct addrinfo **out)
  {
  return(0);
  }

bool log_available(int eventtype)
  {
  return true;
  }

char * netaddr_long(long ap, char *out)
  {
  return(NULL);
  }

int svr_setjobstate(

  job *pjob,
  int newstate,
  int newsubstate,
  int has_queue_mutex)

  {
  if (pjob == NULL)
    return(PBSE_BAD_PARAMETER);

  pjob->ji_qs.ji_state = newstate;
  pjob->ji_qs.ji_substate = newsubstate;

  return(0);
  }


void rel_resc(

  job *pjob)

  {
  return;
  }


