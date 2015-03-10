#ifndef __MACHINE_HPP__
#define __MACHINE_HPP__
#include <string>
#include <vector>
#include <sstream>
#include <hwloc.h>

#include "pbs_job.h"
#include "req.hpp"
#include "allocation.hpp"

extern const int ALL_TASKS;


/* These two macros are for a string length
 * to display cpuset ranges and memory ranges */
#define MAX_CPUSET_SIZE 128
#define MAX_NODESET_SIZE 128

#define INTEL    1
#define AMD      2
#define NON_NUMA 3

using namespace std;

int get_hardware_style(hwloc_topology_t topology);
int get_machine_total_memory(hwloc_topology_t topology, hwloc_uint64_t *memory);



class PCI_Device 
  {
  string         name; /* this is the software name of the device */
  int            id;
  string         info_name;  /* PCI device hardware name */
  string         info_value; /* value for PCI device given by the hardware */
  hwloc_cpuset_t nearest_cpuset;
  char           cpuset_string[MAX_CPUSET_SIZE];

  public:
    PCI_Device();
    ~PCI_Device();
    int initializePCIDevice(hwloc_obj_t, int, hwloc_topology_t);
    void displayAsString(stringstream &out) const;
    void setName(const string &name);
    void setId(int id);
  };



// forward declare Chip
class Chip;



class Core
  {
  // allow Chip to view a core's data
  friend class Chip;
  int                   id;
  hwloc_const_cpuset_t  core_cpuset;
  hwloc_const_nodeset_t core_nodeset;
  char                  core_cpuset_string[MAX_CPUSET_SIZE];
  char                  core_nodeset_string[MAX_NODESET_SIZE];
  int                   totalThreads;
  bool                  free; // Core is not being used at all
  std::vector<int>      indices; // OS indexes of my processing units
  std::vector<bool>     is_index_busy; // Tells whether or not each processing unit is busy
  int                   processing_units_open; // Count of currently unused processing units

  public:
    Core();
    ~Core();
    int getid();
    int getNumberOfProcessingUnits();
    int initializeCore(hwloc_obj_t obj, hwloc_topology_t topology);
    std::vector<int> getPU();
    void displayAsString(stringstream &out) const;
    void mark_as_busy(int index);
    int  get_open_processing_unit();
    bool free_pu_index(int index, bool &core_is_now_free);
    void unit_test_init(); // Only for unit tests
  };



class Chip
  {
  int                     id;
  int                     totalCores;
  int                     totalThreads;
  int                     availableCores;
  int                     availableThreads;
  bool                    chip_is_available;
  bool                    isThreaded;
  hwloc_const_cpuset_t    chip_cpuset;
  hwloc_const_nodeset_t   chip_nodeset;
  char                    chip_cpuset_string[MAX_CPUSET_SIZE];
  char                    chip_nodeset_string[MAX_NODESET_SIZE];
  hwloc_uint64_t          memory;
  hwloc_uint64_t          available_memory;
  std::vector<Core>       cores;
  std::vector<PCI_Device> devices;
  vector<allocation>      allocations;

  public:
    Chip();
    ~Chip();
    int getid();
    int getTotalCores();
    int getTotalThreads();
    int getAvailableCores();
    int getAvailableThreads();
    int getMemory();
    int initializeChip(hwloc_obj_t obj, hwloc_topology_t);
    int initializeNonNUMAChip(hwloc_obj_t, hwloc_topology_t);
    int initializePCIDevices(hwloc_obj_t, hwloc_topology_t);
    bool chipIsAvailable();
#ifdef MIC
    int initializeMICDevices(hwloc_obj_t, hwloc_topology_t);
#endif

#ifdef NVIDIA_GPUS
  #ifdef NVML_API
    int initializeNVIDIADevices(hwloc_obj_t, hwloc_topology_t);
  #endif
#endif

    std::vector<Core> getCores();
    void displayAsString(stringstream &out) const;
    void setMemory(hwloc_uint64_t mem);
    void setId(int id);
    void setCores(int cores); // used for unit tests
    void setThreads(int threads); // used for unit tests
    void setChipAvailable(bool available);
    int  how_many_tasks_fit(const req &r);
    int  place_task(const char *jobid, const req &r, allocation &a, int to_place);
    bool free_task(const char *jobid);
    void free_cpu_index(int index);
    void make_core(int id = 0); // used for unit tests
  };



class Socket
  {
  int                   id;   /* logical index of socket returned by hwloc */
  hwloc_uint64_t        memory;
  hwloc_uint64_t        available_memory;
  int                   totalCores;
  int                   totalThreads;
  int                   availableCores;
  int                   availableThreads;
  std::vector<Chip>     chips;
  char                  socket_cpuset_string[MAX_CPUSET_SIZE];
  char                  socket_nodeset_string[MAX_CPUSET_SIZE];
  bool                  socket_is_available;
  hwloc_const_cpuset_t  socket_cpuset;
  hwloc_const_nodeset_t socket_nodeset;
  vector<allocation>    allocations;

  public:
    Socket();
    ~Socket();
    int initializeSocket(hwloc_obj_t obj);
    int initializeIntelSocket(hwloc_obj_t obj, Chip &newChip);
    int initializeNonNUMASocket(hwloc_obj_t obj, hwloc_topology_t);
    int getTotalChips();
    int getTotalCores();
    int getTotalThreads();
    int getAvailableChips();
    int getAvailableCores();
    int getAvailableThreads();
    int getid();
    hwloc_uint64_t getMemory();
    int initializeAMDSocket(hwloc_obj_t, hwloc_topology_t);
    int initializeIntelSocket(hwloc_obj_t, hwloc_topology_t);
    void setMemory(hwloc_uint64_t mem);
    void displayAsString(stringstream &out) const;
    void setId(int id);
    void addChip(); // used for unit tests
    int  how_many_tasks_fit(const req &r);
    int  place_task(const char *jobid, const req &r, allocation &a, int to_place);
    bool free_task(const char *jobid);
  };



class Machine
  {
  hwloc_uint64_t      totalMemory;
  int                 totalSockets;
  int                 totalChips;
  int                 totalCores;
  int                 totalThreads;
  int                 availableSockets;
  int                 availableChips;
  int                 availableCores;
  int                 availableThreads;
  char                allowed_cpuset_string[MAX_CPUSET_SIZE];
  char                allowed_nodeset_string[MAX_NODESET_SIZE];
  std::vector<Socket> sockets;
  std::vector<PCI_Device> NVIDIA_device;
  vector<allocation>  allocations;

  public:
    Machine& operator=(const Machine& newMachine);
    Machine();
    ~Machine();
    int getNumberOfSockets();
    Socket getSocket();
    int initializeMachine(hwloc_topology_t topology);
    int initializeNonNUMAMachine(hwloc_obj_t obj, hwloc_topology_t topology);
    int initializeNVIDIADevices(hwloc_obj_t obj, hwloc_topology_t topology);
    int getTotalChips();
    hwloc_uint64_t getTotalMemory();
    int getTotalCores();
    int getTotalThreads();
    int getAvailableSockets();
    int getAvailableChips();
    int getAvailableMemory();
    int getAvailableCores();
    int getAvailableThreads();
    bool isNUMA;
    void displayAsString(stringstream &out) const;
    void insertNvidiaDevice(PCI_Device& device);
    int  place_job(job *pjob, string &cpu_string, string &mem_string);
    void setMemory(long long mem); // used for unit tests
    void free_job_allocation(const char *jobid);
    void addSocket(int count); // used for unit tests
    int  get_jobs_cpusets(const char *jobid, string &cpus, string &mems);
  };

extern Machine this_node;


#endif  /* __MACHINE_HPP__ */
