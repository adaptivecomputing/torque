#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include <string>
#include <vector>

#include "resource.h"
#include "mail_throttler.hpp"

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
  return(strdup("Error"));
  }
}

int resource_index_to_string(

  std::string           &output,
  std::vector<resource> &resources,
  size_t                 index)

  {
  static int rcount = 0;

  switch (rcount)
    {
    case 0:

      output = "walltime=100";
      break;

    case 1:

      output = "nodes=2:ppn=8";
      break;

    case 2:

      output="cput=800";
      break;

    case 3:

      output="vmem=8024mb";
      break;

    case 4:

      output="mem=7000mb";
      break;

    default:
      break;
    }

  rcount++;

  return(0);
  }

mail_info::mail_info()
  {
  }

