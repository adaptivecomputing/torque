#include "license_pbs.h"  /* See this file for software license */
#ifndef _TRQ_CGROUPS_H_
#define  _TRQ_CGROUPS_H_

#include <string>

int cleanup_torque_cgroups();
int trq_cg_get_cgroup_path(std::string path); 
void trq_cg_init_subsys_online();
int trq_cg_initialize_hierarchy();
#endif /* _TRQ_CGROUPS_H_ */
