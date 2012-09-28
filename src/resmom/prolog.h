#ifndef _PROLOG_H
#define _PROLOG_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */

#define PBS_PROLOG_TIME 300

/* static char *resc_to_string(job *pjob, int aindex, char *buf, int buflen); */

/* static int pelog_err(job *pjob, char *file, int n, char *text); */

/* static void pelogalm(int sig); */

int run_pelog(int which, char *specpelog, job *pjob, int pe_io_type);

#endif /* _PROLOG_H */
