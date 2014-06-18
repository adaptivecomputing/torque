#include "license_pbs.h" /* See here for the software license */
#ifndef _JOB_RECYCLER_H
#define _JOB_RECYCLER_H

#include "pbs_job.h" /* job */

void initialize_recycler();

void *remove_some_recycle_jobs(void *vp);

void update_recycler_next_id();

#endif /* _JOB_RECYCLER_H */
