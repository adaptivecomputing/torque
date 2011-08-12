#ifndef _JOB_RECOV_H
#define _JOB_RECOV_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */

int job_save(job *pjob, int updatetype, int mom_port);

job *job_recov(char *filename);

#endif /* _JOB_RECOV_H */
