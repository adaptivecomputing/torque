#ifndef _TMSOCK_RECOV_H
#define _TMSOCK_RECOV_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */

int save_tmsock(job *pjob, int, char *, int *, size_t);

int recov_tmsock(int fds, job *pjob);

#endif /* _TMSOCK_RECOV_H */
