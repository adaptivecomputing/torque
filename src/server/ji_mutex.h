#ifndef _JI_MUTEX_H
#define _JI_MUTEX_H
#include "license_pbs.h"
#include "pbs_job.h"
#include "array.h"

int lock_ji_mutex(job *, const char *, char *, int);
int unlock_ji_mutex(job *, const char *, char *, int);

int lock_ai_mutex(job_array *, const char *, char *, int);
int unlock_ai_mutex(job_array *, const char *, char *, int);

int lock_alljobs_mutex(struct all_jobs*, const char *, char *, int);
int unlock_alljobs_mutex(struct all_jobs*, const char *, char *, int);
#endif

