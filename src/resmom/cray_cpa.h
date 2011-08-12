#ifndef _CRAY_CPA_H
#define _CRAY_CPA_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "mom_func.h" /* var_table */

int CPACreatePartition(job *pjob, struct var_table *vtab);

int CPADestroyPartition(job *pjob);

int CPASetJobEnv(unsigned long ParID, unsigned long AllocCookie, char *JobID);

#endif /* _CRAY_CPA_H */
