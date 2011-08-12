#ifndef _JOB_QS_UPGRADE_H
#define _JOB_QS_UPGRADE_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */

int job_qs_upgrade(job *pj, int fds, char *path, int version);

int upgrade_v1(job *pj, int fds);

int upgrade_2_3_X(job *pj, int fds);

int upgrade_2_2_X(job *pj, int fds);

int upgrade_2_1_X(job *pj, int fds);


#endif /* _JOB_QS_UPGRADE_H */
