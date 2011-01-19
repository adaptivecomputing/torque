#ifndef PBS_CPUSET_H
#define PBS_CPUSET_H 1

#include <sys/types.h>
#include <unistd.h>
#include <hwloc.h>

#include "pbs_job.h"

extern int          init_cpusets(void);
extern int          create_cpuset(const char *, const hwloc_bitmap_t, const hwloc_bitmap_t, mode_t);
extern int          read_cpuset(const char *, hwloc_bitmap_t, hwloc_bitmap_t);
extern int          delete_cpuset(const char *);
extern int          init_torque_cpuset(void);
extern void         cleanup_torque_cpuset(void);
extern int          create_job_cpuset(job *);
extern int          move_to_job_cpuset(pid_t, job *);
extern struct pidl *get_cpuset_pidlist(const char *, struct pidl *);
extern void         free_cpuset_pidlist(struct pidl *);
extern int          get_cpuset_mempressure(const char *);
extern int          hwloc_bitmap_parselist(const char *, hwloc_bitmap_t);
extern int          hwloc_bitmap_displaylist(char *, size_t len, hwloc_bitmap_t);

#endif /* END PBS_CPUSET_H */
