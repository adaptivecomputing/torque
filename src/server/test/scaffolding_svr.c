#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */
#include "work_task.h" /* work_task */

int relay_to_mom(job **pjob, struct batch_request *request, void (*func)(struct work_task *))
  {
  fprintf(stderr, "The call to relay_to_mom needs to be mocked!!\n");
  exit(1);
  }

