#include "license_pbs.h"  /* See this file for software license */
#ifndef _TRQ_CGROUPS_H_
#define  _TRQ_CGROUPS_H_

#include <string>
#include <vector>
#include "pbs_job.h"
#include "log.h"
#include "allocation.hpp"

extern std::string cg_cpu_path;
extern std::string cg_cpuset_path;
extern std::string cg_cpuacct_path;
extern std::string cg_memory_path;
extern std::string cg_devices_path;


int trq_cg_cleanup_torque_cgroups();
int trq_cg_get_cgroup_path(std::string path); 
void trq_cg_init_subsys_online(bool val);
int trq_cg_initialize_hierarchy();
int trq_cg_create_cgroup(std::string &, job *);
void *trq_cg_remove_process_from_accts(void *vp);
int trq_cg_set_resident_memory_limit(const char *job_id, unsigned long long memory_limit);
int trq_cg_set_task_resident_memory_limit(const char *job_id, unsigned int req_index, unsigned int task_index, unsigned long long memory_limit);
int trq_cg_set_swap_memory_limit(const char *job_id, unsigned long long memory_limit);
int trq_cg_set_task_swap_memory_limit(const char *job_id, unsigned int req_index, unsigned int task_index, unsigned long long memory_limit);
int trq_cg_get_cgroup_paths_from_file();
int trq_cg_initialize_cpuset_string(std::string file_name);
int trq_cg_remove_process_from_cgroup(std::string& cgroup_path, const char *job_id);
int trq_cg_add_process_to_cgroup(std::string &cgroup_path, const char *job_id, pid_t new_pid);
int trq_cg_add_pid_to_cgroup_tasks(std::string& cgroup_path, pid_t job_pid);
int trq_cg_create_all_cgroups(job *pjob);
int trq_cg_add_process_to_all_cgroups(const char *job_id, pid_t job_pid);
int trq_cg_add_process_to_task_cgroup(std::string &cgroup_path, const char *job_id, 
                 const unsigned int req_index, const unsigned int task_index, pid_t new_pid);
int trq_cg_get_task_memory_stats(const char *job_id, const unsigned int req_index, const unsigned int task_index, unsigned long long &mem_used);
int trq_cg_get_task_cput_stats(const char *job_id, const unsigned int req_index, const unsigned int task_index, unsigned long &cput_used);
void trq_cg_delete_job_cgroups(const char *job_id, bool successfully_created);
bool have_incompatible_dash_l_resource(pbs_attribute *pattr);
int  trq_cg_add_devices_to_cgroup(job *pjob);
int init_torque_cgroups();
void trq_cg_signal_tasks(const std::string& cgroup_path, int signal);
int find_range_in_cpuset_string(std::string &source, std::string &output);
#endif /* _TRQ_CGROUPS_H_ */
