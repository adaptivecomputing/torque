#include <iostream>
#include <string>
#include <vector>
#include "machine.hpp"
#include <errno.h>
#include <hwloc.h>

using namespace std;

  Chip::Chip()
    {
    totalProcessingUnits = 0;
    }

  Chip::~Chip()
    {
    totalProcessingUnits = 0;
    }

  int Chip::getTotalCores()
    {
    return(this->cores.size());
    }

  int Chip::getTotalProcessingUnits()
    {
    return(this->totalProcessingUnits);
    }

  int Chip::initializeChip(hwloc_obj_t obj)
    {
    hwloc_obj_t child_obj;

    child_obj = obj->first_child;

    /* traverse past all of the cache levels until you find the core level */
    if (child_obj->type != HWLOC_OBJ_CORE)
      {
      initializeChip(child_obj);
      while (child_obj->next_sibling != NULL)
        {
        hwloc_obj_t temp_obj = child_obj->next_sibling;
        initializeChip(temp_obj);
        child_obj = temp_obj;
        }

      return(0);
      }

    /* We found the core level */
    Core newCore;

    newCore.initializeCore(child_obj);
    this->cores.push_back(newCore);
    this->totalProcessingUnits += newCore.getNumberOfProcessingUnits();

    return(0);
    }

  int Chip::getMemoryInBytes()
    {
    return(this->memory);
    }



