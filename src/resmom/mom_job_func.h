#ifndef _JOB_FUNC_H
#define _JOB_FUNC_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */

void tasks_free(job *pj);

int remtree(char *dirname);

int conn_qsub(char *hostname, long port, char *EMsg);

job *job_alloc(void);

void mom_job_free(job *pj);

int job_unlink_file(job *pjob, const char *name);

/* static void job_init_wattr(job *pj); */

void *delete_job_files(void *vp);

void mom_job_purge(job *pjob);

#endif /* _JOB_FUNC_H */
