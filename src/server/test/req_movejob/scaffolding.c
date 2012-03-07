#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "pbs_job.h" /* job, all_jobs */
#include "batch_request.h" /* batch_request */
#include "queue.h" /* pbs_queue */

char *pbs_o_host = "PBS_O_HOST";
char *msg_movejob = "Job moved to ";
char *msg_manager = "%s at request of %s@%s";
int LOGLEVEL = 0;

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save to be mocked!!\n");
  exit(1);
  }

int svr_movejob(job *jobp, char *destination, struct batch_request *req)
  {
  fprintf(stderr, "The call to svr_movejob to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_jobs_queue(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_queue to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

char *get_variable(job *pjob, char *variable)
  {
  fprintf(stderr, "The call to get_variable to be mocked!!\n");
  exit(1);
  }

void svr_dequejob(job *pjob, int val)
  {
  fprintf(stderr, "The call to svr_dequejob to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

int svr_enquejob(job *pjob, int has_sv_qs_mutex, int prev_index)
  {
  fprintf(stderr, "The call to svr_enquejob to be mocked!!\n");
  exit(1);
  }

int swap_jobs(struct all_jobs *aj, job *job1, job *job2)
  {
  fprintf(stderr, "The call to swap_jobs to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt to be mocked!!\n");
  exit(1);
  }

int svr_chkque(job *pjob, pbs_queue *pque, char *hostname, int mtype, char *EMsg)
  {
  fprintf(stderr, "The call to svr_chkque to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *id, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }

job *chk_job_request(char *jobid, struct batch_request *preq)
  {
  fprintf(stderr, "The call to chk_job_request to be mocked!!\n");
  exit(1);
  }

