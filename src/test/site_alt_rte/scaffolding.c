#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "pbs_job.h" /* job */
#include "queue.h" /* pbs_queue */

job *scaf_pjob = NULL;
pbs_queue *scaf_qp = NULL;
int scaf_retry_time = -1;
int LOGLEVEL;

int default_router(job *pjob, struct pbs_queue *qp, long retry_time)
  {
  scaf_pjob = pjob;
  scaf_qp = qp;
  scaf_retry_time = retry_time;
  return 1;
  }

void log_event(

  int         eventtype,
  int         objclass,
  const char *objname,
  const char       *text) {}

