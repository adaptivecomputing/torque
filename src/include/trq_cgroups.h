#include "license_pbs.h"  /* See this file for software license */
#ifndef _TRQ_CGROUPS_H_
#define  _TRQ_CGROUPS_H_

#include <string>
#include <vector>
#include "pbs_job.h"
#include "log.h"

extern std::string cg_cpu_path;
extern std::string cg_cpuset_path;
extern std::string cg_cpuacct_path;
extern std::string cg_memory_path;
extern std::string cg_devices_path;


int trq_cg_cleanup_torque_cgroups();
int trq_cg_get_cgroup_path(std::string path); 
void trq_cg_init_subsys_online(bool val);
int trq_cg_initialize_hierarchy();
int trq_cg_create_cgroup(std::string &, const char *job_id);
void *trq_cg_remove_process_from_accts(void *vp);
int trq_cg_set_resident_memory_limit(const char *job_id, unsigned long memory_limit);
int trq_cg_set_swap_memory_limit(const char *job_id, unsigned long memory_limit);
int trq_cg_get_cgroup_paths_from_file();
int trq_cg_initialize_cpuset_string(std::string file_name);
int trq_cg_remove_process_from_cgroup(std::string& cgroup_path, const char *job_id);
int trq_cg_add_process_to_cgroup(std::string &cgroup_path, const char *job_id, pid_t new_pid);
int trq_cg_reserve_cgroup(job *pjob);
int trq_cg_add_pid_to_cgroup_tasks(std::string& cgroup_path, pid_t job_pid);
int trq_cg_add_process_to_cgroup_accts(const char *job_id, pid_t job_pid);
int trq_cg_create_all_cgroups(job *pjob);
int trq_cg_add_process_to_cgroup(const char *job_id, pid_t job_pid);
int trq_cg_add_process_to_all_cgroups(const char *job_id, pid_t job_pid);
#endif /* _TRQ_CGROUPS_H_ */
