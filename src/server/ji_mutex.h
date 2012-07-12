#ifndef _JI_MUTEX_H
#define _JI_MUTEX_H
#include "license_pbs.h"
#include "pbs_job.h"

int lock_ji_mutex(job *, const char *, char *, int);
int unlock_ji_mutex(job *, const char *, char *, int);

#endif

