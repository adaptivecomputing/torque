#ifndef _RUN_SCHED_H
#define _RUN_SCHED_H
#include "license_pbs.h" /* See here for the software license */

/* static int contact_sched(int cmd); */

int schedule_jobs(void);

/* static int contact_listener(int l_idx); */

void notify_listeners(void);

/* static void scheduler_close(); */

/* static void listener_close(int sock); */

/* static int put_4byte(int sock, unsigned int val); */

#endif /* _RUN_SCHED_H */
