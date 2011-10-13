#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

char *msg_job_stageinfail = "File stage in failed, see below.\nJob will be retried later, please investigate and correct problem.";
char *msg_job_start = "Begun execution";
char *msg_job_end = "Execution terminated";
char *msg_job_copychkptfail = "Checkpoint file copy failed, see below.\nJob will be retried later, please investigate and correct problem.";
char *msg_job_otherfail = "An error has occurred processing your job, see below.";
char *msg_job_abort = "Aborted by PBS Server ";
char *msg_job_del       = "job deleted";

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt to be mocked!!\n");
  exit(1);
  }

