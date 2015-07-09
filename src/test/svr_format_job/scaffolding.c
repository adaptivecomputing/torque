#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

const char *msg_job_stageinfail = "File stage in failed, see below.\nJob will be retried later, please investigate and correct problem.";
const char *msg_job_start = "Begun execution";
const char *msg_job_end = "Execution terminated";
const char *msg_job_copychkptfail = "Checkpoint file copy failed, see below.\nJob will be retried later, please investigate and correct problem.";
const char *msg_job_otherfail = "An error has occurred processing your job, see below.";
const char *msg_job_abort = "Aborted by PBS Server ";
const char *msg_job_del       = "job deleted";

extern "C"
{
char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt to be mocked!!\n");
  exit(1);
  }
}

