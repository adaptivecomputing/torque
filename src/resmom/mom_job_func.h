#ifndef _JOB_FUNC_H
#define _JOB_FUNC_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */

void tasks_free(mom_job *pj);

int remtree(char *dirname);

int conn_qsub(char *hostname, long port, char *EMsg);

mom_job *job_alloc(void);

void mom_job_free(mom_job *pj);

int job_unlink_file(mom_job *pjob, const char *name);

void delete_job_files(void *vp);
void remove_tmpdir_files(void);

void mom_job_purge(mom_job *pjob);

#endif /* _JOB_FUNC_H */
