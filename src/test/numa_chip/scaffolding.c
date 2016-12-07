#include <stdlib.h>
#include <stdio.h>
#include "machine.hpp"
#include "log.h"
#include "pbs_error.h"
#include "machine.hpp"
#include "json/json.h"
#include "numa_constants.h"

const char  *use_cores = "usecores";
std::string  my_placement_type;
std::string  thread_type;
int          hardware_style;
int          recorded = 0;

const int    MIC_TYPE = 0;
const int    GPU = 1;

const char *place_node = "node";
const char *place_socket = "socket";
const char *place_numa_node = "numanode";
const char *place_core = "core";
const char *place_thread = "thread";
const char *place_legacy = "legacy";
const char *place_legacy2 = "legacy2";
Json::Value alloc_json;
std::stringstream Json_as_stream;

void log_err(int errnum, const char *routine, const char *text)
  {
  }

int get_hardware_style(hwloc_topology_t topology)
  {
  hwloc_obj_t node_obj;
  hwloc_obj_t socket_obj;
  int node_depth;
  int socket_depth;

  node_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_NODE, NULL); 
  if (node_obj == NULL)
    {
    return(NON_NUMA);
    }
  node_depth = node_obj->depth;

  socket_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_SOCKET, NULL); 
  if (socket_obj == NULL)
    {
    log_err(-1, __func__, "failed to get node object");
    return(PBSE_SYSTEM);
    }
  socket_depth = socket_obj->depth;

  if (socket_depth < node_depth)
    return(AMD);
  else
    return(INTEL);
  }

PCI_Device::PCI_Device() 
  {
  static int pci_count = 0;

  pci_count++;
  if (pci_count % 2)
    this->type = MIC_TYPE;
  else
    this->type = GPU;

  this->id = pci_count;
  this->busy = false;
  strcpy(this->cpuset_string, "0");
  }

void PCI_Device::set_type(int type)
  {
  this->type = type;
  }

void PCI_Device::setId(int id)
  {
  this->id = id;
  }

PCI_Device::~PCI_Device() {}

PCI_Device::PCI_Device(const PCI_Device &other) 
  
  {
  this->type = other.type;
  this->id = other.id;
  this->busy = other.busy;
  strcpy(this->cpuset_string, other.cpuset_string);
  }

void PCI_Device::displayAsString(std::stringstream &out) const {}

int PCI_Device::get_type() const

  {
  return(this->type);
  }

PCI_Device &PCI_Device::operator=(const PCI_Device &other)
  {
  this->type = other.type;
  this->id = other.id;
  this->busy = other.busy;
  strcpy(this->cpuset_string, other.cpuset_string);
  return(*this);
  }

const char *PCI_Device::get_cpuset() const
  {
  return(this->cpuset_string);
  }

void PCI_Device::set_state(bool in_use)
  {
  this->busy = in_use;
  }

int PCI_Device::get_id() const
  {
  return(this->id);
  }

bool PCI_Device::is_busy() const
  {
  return(this->busy);
  }

int get_machine_total_memory(hwloc_topology_t topology, unsigned long *memory)
  {
  *memory = 123456789;
  return(PBSE_NONE);
  }

req::req() : mem(0), cores(0), threads(0), gpus(0), mics(0) {}

unsigned long req::getMemory() const
  {
  return(this->mem);
  }

int req::getPlaceCores() const
  {
  return(this->cores);
  }

int req::getPlaceThreads() const
  {
  return(this->threads);
  }

std::string req::getPlacementType() const
  {
  return(my_placement_type);
  }

std::string req::getThreadUsageString() const
  {
  return(thread_type);
  }


int req::getExecutionSlots() const
  {
  return(this->execution_slots);
  }

int req::set_value(const char *name, const char *value, bool is_default)
  {
  if (!strcmp(name, "lprocs"))
    this->execution_slots = atoi(value);
  else if (!strcmp(name, "memory"))
    this->mem = strtol(value, NULL, 10);
  else if (!strcmp(name, GPUS))
    this->gpus = atoi(value);
  else if (!strcmp(name, MICS))
    this->mics = atoi(value);

  return(0);
  }

int req::getMics() const
  {
  return(this->mics);
  }
 
int req::get_gpus() const
  {
  return(this->gpus);
  }

void req::record_allocation(const allocation &a)
  {
  recorded++;
  }

int is_whitespace(

  char c)

  {
  if ((c == ' ')  ||
      (c == '\n') ||
      (c == '\t') ||
      (c == '\r') ||
      (c == '\f'))
    return(TRUE);
  else
    return(FALSE);
  } /* END is_whitespace */

#ifdef MIC
int Chip::initializeMICDevices(hwloc_obj_t chip_obj, hwloc_topology_t topology)
  {
  return(0);
  }
#endif

void setAllocJson(int test_num)
  {
  alloc_json = Json::nullValue;
  if(test_num == 1)//test_initialize_allocation's configuration
    {
    alloc_json[JOBID] = "666979[0].mgr.bwfor.privat";
    alloc_json[CPUS] = "0";
    alloc_json[MEM] = "4203424";
    alloc_json[EXCLUSIVE] = 0;
    alloc_json[CORES_ONLY] = 0;  
    return;
    }
  
  //the next few tests have similar setups
  //so create test_spread_place_threads config
  //and use if statements to modify the Json for the specific tests
  if(test_num < 5)//after test 4 the string changes significantly
    {
    alloc_json[OS_INDEX] = 0;
    alloc_json[CORES] = "0-15";
    alloc_json[THREADS] = "16-31";
    alloc_json[MEM] = "6";
    alloc_json[ALLOCATIONS][0][ALLOCATION][JOBID] = "1.napali";
    alloc_json[ALLOCATIONS][0][ALLOCATION][CPUS] = "16";
    alloc_json[ALLOCATIONS][0][ALLOCATION][MEM] = "0";
    alloc_json[ALLOCATIONS][0][ALLOCATION][EXCLUSIVE] = 0;
    alloc_json[ALLOCATIONS][0][ALLOCATION][CORES_ONLY] = 0;
   
    
    if(test_num == 2)//test_spread_place_threads's configuration is the previous Json statments
      {
      return;
      }
  
    if(test_num == 3)//test_spread_place_cores' configuration
      {
      alloc_json[ALLOCATIONS][0][ALLOCATION][CPUS] = "1";
      alloc_json[ALLOCATIONS][0][ALLOCATION][CORES_ONLY] = 1;
      return;
      }
    if(test_num == 4)//test_spread_place's configuration
      {
      alloc_json[ALLOCATIONS][0][ALLOCATION][CPUS] = "0,4,8,12";
      alloc_json[ALLOCATIONS][0][ALLOCATION][EXCLUSIVE] = 3;
      alloc_json[ALLOCATIONS][0][ALLOCATION][CORES_ONLY] = 1;
      return;
      }
    }//end test_num <5

  //The next 3 configurations(5-7) are from test_json_constructor
  if(test_num < 8)
    {
    alloc_json[NUMA_NODES][0][NUMA_NODE][OS_INDEX] = 0;
    alloc_json[NUMA_NODES][0][NUMA_NODE][CORES] = "0-15";
    alloc_json[NUMA_NODES][0][NUMA_NODE][THREADS] = "16-31";
    alloc_json[NUMA_NODES][0][NUMA_NODE][MEM] = "1024";
    alloc_json[NUMA_NODES][0][NUMA_NODE][GPUS] = "0-1";
    alloc_json[NUMA_NODES][0][NUMA_NODE][MICS] = "2-3";
    
    if(test_num == 5)//j4's config
      {
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][JOBID] = "0.napali";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][CPUS] = "0-3,16-19";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][MEM] ="0";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][EXCLUSIVE] = 0;
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][CORES_ONLY] = 0;
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][GPUS] = "0-1";

      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][JOBID] = "1.napali";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][CPUS] = "4-15";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][MEM] = "0";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][EXCLUSIVE] = 0;
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][CORES_ONLY] = 1;
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][MICS] = "2-3";
      return;
      }
    if(test_num == 6)//j6's config
      {
      for(int i = 0; i < 4; i++)
        {
        alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][i][ALLOCATION][EXCLUSIVE] = 0;
        alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][i][ALLOCATION][CORES_ONLY] = 0;
        }      
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][JOBID] = "0.napali";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][CPUS] = "0";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][MEM] = "10";
      
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][JOBID] = "1.napali";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][CPUS] = "0";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][MEM] = "10";

      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][2][ALLOCATION][JOBID] = "0.napali";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][2][ALLOCATION][CPUS] = "5";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][2][ALLOCATION][MEM] = "10";
      
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][3][ALLOCATION][JOBID] = "2.napali";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][3][ALLOCATION][CPUS] = "6-7";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][3][ALLOCATION][MEM] = "1000";
      return;
      }
    if(test_num == 7)//j6 after removal
      {
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][JOBID] = "1.napali";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][CPUS] = "0";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][MEM] = "10";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][EXCLUSIVE] = 0;
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][0][ALLOCATION][CORES_ONLY] = 0;
      
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][JOBID] = "2.napali";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][CPUS] = "6-7";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][MEM] = "1000";
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][EXCLUSIVE] = 0;
      alloc_json[NUMA_NODES][0][NUMA_NODE][ALLOCATIONS][1][ALLOCATION][CORES_ONLY] = 0;
      }
    }//end if test_num < 8
  }
