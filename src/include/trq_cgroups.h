#include "license_pbs.h"  /* See this file for software license */
#ifndef _TRQ_CGROUPS_H_
#define  _TRQ_CGROUPS_H_

#include <ostream>
#include <string>
#include <vector>

extern std::string cg_cpu_path;
extern std::string cg_cpuset_path;
extern std::string cg_cpuacct_path;
extern std::string cg_memory_path;
extern std::string cg_devices_path;


int cleanup_torque_cgroups();
int trq_cg_get_cgroup_path(std::string path); 
void trq_cg_init_subsys_online();
int trq_cg_initialize_hierarchy();
int trq_cg_create_cgroup(std::string&, pid_t);
int trq_cg_add_process_to_cgroup_accts(pid_t);
int trq_cg_add_process_to_cgroup(std::string&, pid_t, pid_t);
int trq_cg_remove_process_from_accts(job *pjob);
#endif /* _TRQ_CGROUPS_H_ */
