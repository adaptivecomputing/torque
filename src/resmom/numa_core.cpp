#include <iostream>
#include <string>
#include <vector>
#include "machine.hpp"
#include <errno.h>
#include <hwloc.h>
#include "pbs_config.h"

#ifdef PENABLE_LINUX26_CPUSETS

#include "pbs_error.h"
#include "log.h"

using namespace std;

#define INTEL 1
#define AMD   2


  Core::Core()
    {
    id = -1;
    memset(core_cpuset_string, 0, MAX_CPUSET_SIZE);
    memset(core_nodeset_string, 0, MAX_NODESET_SIZE);
    }

  Core::~Core()
    {
    id = -1;
    }


  int Core::initializeCore(hwloc_obj_t core_obj, hwloc_topology_t topology)
    {
    hwloc_obj_t pu_obj;

    /* We now need to find all of the processing units associated with this core */
    this->id = core_obj->logical_index;
    this->core_cpuset = core_obj->allowed_cpuset;
    this->core_nodeset = core_obj->allowed_nodeset;
    hwloc_bitmap_list_snprintf(this->core_cpuset_string, MAX_CPUSET_SIZE, this->core_cpuset);
    hwloc_bitmap_list_snprintf(this->core_nodeset_string, MAX_NODESET_SIZE, this->core_nodeset);return(0);
    this->totalThreads = hwloc_get_nbobjs_inside_cpuset_by_type(topology, this->core_cpuset, HWLOC_OBJ_PU);
    return(PBSE_NONE);
    }


  int Core::getNumberOfProcessingUnits()
    {
    return(this->totalThreads);
    }


#endif /* PENABLE_LINUX26_CPUSETS */  
