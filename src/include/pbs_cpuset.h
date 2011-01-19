#ifndef PBS_CPUSET_H
#define PBS_CPUSET_H 1

#include <sys/types.h>
#include <unistd.h>

#include "pbs_job.h"

extern void         initialize_root_cpuset();
extern int          create_jobset(job *);
extern int          delete_cpuset(char *);
extern int          move_to_jobset(pid_t, job *);
extern int          move_to_taskset(pid_t, job *, char *);
extern struct pidl *get_cpuset_pidlist(const char *, struct pidl *);
extern void         free_cpuset_pidlist(struct pidl *);
extern int          get_cpuset_mempressure(const char *);

#endif /* END PBS_CPUSET_H */
