#include <iostream>
#include <string>
#include <vector>
#include "machine.hpp"
#include <errno.h>
#include <hwloc.h>

using namespace std;

  Core::Core()
    {
    id = 0;
    }

  Core::~Core()
    {
    id = 0;
    }

  int Core::initializeCore(hwloc_obj_t obj)
    {
    hwloc_obj_t child_obj;

    /* We now need to find all of the processing units associated with this core */
    this->id = obj->os_index;
    this->numberOfProcessingUnits = 0;
    child_obj = obj->first_child;

    while(child_obj != NULL)
      {
      hwloc_obj_t temp_obj;

      this->pu.push_back(child_obj->os_index);
      this->numberOfProcessingUnits++;
      temp_obj = child_obj->next_sibling;
      child_obj = temp_obj;
      }

    return(0);
    }


  int Core::getNumberOfProcessingUnits()
    {
    return(this->numberOfProcessingUnits);
    }


