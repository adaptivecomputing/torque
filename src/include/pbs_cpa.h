#ifndef PBS_CPA_H
#define PBS_CPA_H 1

#include "job.h"

int CPACreatePartition(job *pjob,struct var_table *vtab);

int CPADestroyPartition(

  unsigned long      ParID,         /* I */
  unsigned long long AdminCookie,   /* I */
  int *ErrorP)        /* O - non-zero if users of partition encountered error */
;

int CPASetJobEnv(

  unsigned long  ParID,        /* I */
  unsigned long  AllocCookie,  /* I */
  char          *JobID)        /* I */
;


#endif /* PBS_CPA_H */
