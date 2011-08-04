#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "pbs_job.h" /* job */
#include "queue.h" /* pbs_queue */

int default_router(job *jobp, struct pbs_queue *qp, long retry_time)
  { 
  fprintf(stderr, "The call to gettime needs to be mocked!!\n");
  exit(1);
  }


