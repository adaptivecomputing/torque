#include "license_pbs.h" /* See here for the software license */
#ifndef _LIB_MOM_H
#define _LIB_MOM_H
#ifdef BUILD_L26_CPUSETS
#include <hwloc.h> /* hwloc_bitmap_t */
#endif

#include "pbs_job.h" /* job */
#include "list_link.h" /* pidl */
#include "resource.h" /* resource */
#include "mom_mach.h" /* proc_stat_t, startjob_rtn */
#include "mom_func.h" /* var_table */

/* was in mom_mach.c */
typedef struct proc_mem
  {
  unsigned long long mem_total;
  unsigned long long mem_used;
  unsigned long long mem_free;
  unsigned long long swap_total;
  unsigned long long swap_used;
  unsigned long long swap_free;
  } proc_mem_t;

#ifdef BUILD_L26_CPUSETS
/* cpuset.c */
int hwloc_bitmap_parselist(const char *buf, hwloc_bitmap_t map);

int hwloc_bitmap_displaylist(char *buf, size_t buflen, hwloc_bitmap_t map);

int init_cpusets(void);

int create_cpuset(const char *name, const hwloc_bitmap_t cpus, const hwloc_bitmap_t mems, mode_t flags);

int read_cpuset(const char *name, hwloc_bitmap_t cpus, hwloc_bitmap_t mems);

int delete_cpuset(const char *name);

void cleanup_torque_cpuset(void);

int init_torque_cpuset(void);

int add_obj_from_cpuset(hwloc_bitmap_t cpuset, hwloc_bitmap_t cpus, unsigned idx);

int create_job_cpuset(job *pjob);

int move_to_job_cpuset(pid_t pid, job *pjob);

void adjust_root_map(char *cpusetStr, int cpusetMap[], int add);

void remove_boot_set(char *rootStr, char *bootStr);

static int PidIsPid(pid_t pid);

struct pidl *get_cpuset_pidlist(const char *name, struct pidl *pids);

int get_cpuset_mempressure(const char *name);
#endif /* BUILD_L26_CPUSETS */

/* mom_mach.c */

void proc_get_btime(void);

proc_stat_t *get_proc_stat(int pid);

#ifdef USELIBMEMACCT
long long get_memacct_resi(pid_t pid);
#endif

#ifdef PNOT
proc_mem_t *get_proc_mem(void);
#endif /* PNOT */

void dep_initialize(void);

void dep_cleanup(void);

void dep_main_loop_cycle(void);

static int mm_getsize(resource *pres, unsigned long *ret);

static int mm_gettime(resource *pres, unsigned long *ret);

static int injob(job *pjob, pid_t sid);

static unsigned long cput_sum(job *pjob);

static int overcpu_proc(job *pjob, unsigned long limit);

/* mom_start.c */
int set_job(job *pjob, struct startjob_rtn *sjr);

void set_globid(job *pjob, struct startjob_rtn *sjr);

int set_mach_vars(job *pjob, struct var_table *vtab);

char *set_shell(job *pjob, struct passwd *pwdp);

void scan_for_terminated(void);

#ifndef USEOLDTTY
 int open_master(char **rtn_name);
#else /* USEOLDTTY */
 int open_master(char **rtn_name);
#endif /* USEOLDTTY */

/* pe_input.c */

int pe_input(char *jobid);

#endif /* _LIB_MOM_H */
