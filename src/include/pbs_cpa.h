#ifndef PBS_CPA_H
#define PBS_CPA_H 1

#include "pbs_job.h"

int CPACreatePartition(job *pjob, struct var_table *vtab);

int CPADestroyPartition(job *pjob);

int CPASetJobEnv(

  unsigned long  ParID,        /* I */
  unsigned long  AllocCookie,  /* I */
  char          *JobID)        /* I */
;


#endif /* PBS_CPA_H */
