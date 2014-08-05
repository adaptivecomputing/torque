#include <iostream>
#include <string>
#include <vector>
#include "machine.hpp"
#include <errno.h>
#include <hwloc.h>

using namespace std;


  Socket::Socket()
    {
    id = 0;
    memory = 0;
    numberOfProcessingUnits = 0;
    numberOfCores = 0;
    }

  Socket::~Socket()
    {
    id = -1;
    }


  /* this signature is called when the socket depth is above the
     numa_node depth or there is no numa_node. This is the case
     for AMD hardware or non-numa systems */
  int Socket::initializeSocket(hwloc_obj_t obj)
    {
    hwloc_obj_type_t type;
    hwloc_obj_t      child_obj;

    /* See what kind of device is our immediate child */
    child_obj = obj->first_child;

    /* depending on the object type we have NUMA or we have non-numa */
    if (child_obj->type == HWLOC_OBJ_CACHE) 
      {
      /* this is non-numa */
      Chip newChip;

      newChip.initializeChip(child_obj);
      this->numberOfProcessingUnits = newChip.getTotalProcessingUnits();
      this->numberOfCores = newChip.getTotalCores();
      this->isNUMA = false;
      }

    if (child_obj->type == HWLOC_OBJ_NODE)
      {
      hwloc_obj_t  chip_obj;
      
      this->isNUMA = true;
      chip_obj = child_obj;
      while (chip_obj != NULL)
        {
        hwloc_obj_t temp_obj;

        Chip *new_NUMA_chip = new Chip();
        new_NUMA_chip->initializeChip(chip_obj);
        this->numberOfProcessingUnits += new_NUMA_chip->getTotalProcessingUnits();
        this->numberOfCores += new_NUMA_chip->getTotalCores();
        this->chips.push_back(*new_NUMA_chip);
        temp_obj = chip_obj->next_sibling;
        chip_obj = temp_obj;
        delete new_NUMA_chip;
        }
      }

      this->id = obj->os_index;

      return(0);
    }


  int Socket::initializeIntelSocket(hwloc_obj_t socket_obj, Chip &newChip)
    {
    /* When this is an Intel NUMA configuration only one Socket per
     * NUMA chip. No need to walk a linked list */
    this->id = socket_obj->os_index;
    this->memory = newChip.getMemoryInBytes();
    this->numberOfCores = newChip.getTotalCores();
    this->numberOfProcessingUnits = newChip.getTotalProcessingUnits();
    this->chips.push_back(newChip);
    isNUMA = true;
    return(0);
    }

  int Socket::getTotalCores()
    {
    return(this->numberOfCores);
    }

  int Socket::getTotalProcessors()
    {
    return(this->numberOfProcessingUnits);
    }

  int Socket::getTotalChips()
    {
    return(this->chips.size());
    }

  hwloc_uint64_t Socket::getMemoryInBytes()
    {
    return(this->memory);
    }

