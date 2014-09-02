#include <iostream>
#include <string>
#include <hwloc.h>


/* These two macros are for a string length
 * to display cpuset ranges and memory ranges */
#define MAX_CPUSET_SIZE 128
#define MAX_NODESET_SIZE 128

using namespace std;

int get_hardware_style(hwloc_topology_t topology);
int get_machine_total_memory(hwloc_topology_t topology, hwloc_uint64_t *memory);


class PCI_Device 
  {
  string name; /* this is the software name of the device */
  int    id;
  string info_name;  /* PCI device hardware name */
  string info_value; /* value for PCI device given by the hardware */
  hwloc_cpuset_t nearest_cpuset;
  char cpuset_string[MAX_CPUSET_SIZE];

  public:
    PCI_Device();
    ~PCI_Device();
    int initializePCIDevice(hwloc_obj_t, int, hwloc_topology_t);
  };

class Core
  {
  int id;
  hwloc_const_cpuset_t core_cpuset;
  hwloc_const_nodeset_t core_nodeset;
  char core_cpuset_string[MAX_CPUSET_SIZE];
  char core_nodeset_string[MAX_NODESET_SIZE];
  int totalThreads;

  public:
    Core();
    ~Core();
    int getid();
    int getNumberOfProcessingUnits();
    int initializeCore(hwloc_obj_t obj, hwloc_topology_t topology);
    std::vector<int> getPU();
  };

class Chip
  {
  int id;
  int totalCores;
  int totalThreads;
  int availableCores;
  int availableThreads;
  bool chip_is_available;
  bool isThreaded;
  hwloc_const_cpuset_t chip_cpuset;
  hwloc_const_nodeset_t chip_nodeset;
  char chip_cpuset_string[MAX_CPUSET_SIZE];
  char chip_nodeset_string[MAX_NODESET_SIZE];
  hwloc_uint64_t memory;
  std::vector<Core> cores;
  std::vector<PCI_Device> devices;

  public:
    Chip();
    ~Chip();
    int getid();
    int getTotalCores();
    int getTotalThreads();
    int getAvailableCores();
    int getAvailableThreads();
    int getMemoryInBytes();
    int initializeChip(hwloc_obj_t obj, hwloc_topology_t);
    int initializeNonNUMAChip(hwloc_obj_t, hwloc_topology_t);
    int initializePCIDevices(hwloc_obj_t, hwloc_topology_t);
    bool chipIsAvailable();
#ifdef MIC
    int initializeMICDevices(hwloc_obj_t, hwloc_topology_t);
#endif

#ifdef NVIDIA_GPU
  #ifdef NVML_API
    int initializeNVIDIADevices(hwloc_obj_t, hwloc_topology_t);
  #endif
#endif

    std::vector<Core> getCores();
  };

class Socket
  {
  int               id;   /* logical index of socket returned by hwloc */
  hwloc_uint64_t  memory;
  int totalCores;
  int totalThreads;
  int availableCores;
  int availableThreads;
  std::vector<Chip> chips;
  char socket_cpuset_string[MAX_CPUSET_SIZE];
  char socket_nodeset_string[MAX_CPUSET_SIZE];
  bool socket_is_available;
  hwloc_const_cpuset_t socket_cpuset;
  hwloc_const_nodeset_t socket_nodeset;

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
    hwloc_uint64_t getMemoryInBytes();
    int initializeAMDSocket(hwloc_obj_t, hwloc_topology_t);
    int initializeIntelSocket(hwloc_obj_t, hwloc_topology_t);
  };


class Machine
  {
  hwloc_uint64_t  totalMemoryInBytes;
  int totalSockets;
  int totalChips;
  int totalCores;
  int totalThreads;
  int availableSockets;
  int availableChips;
  int availableCores;
  int availableThreads;
  char allowed_cpuset_string[MAX_CPUSET_SIZE];
  char allowed_nodeset_string[MAX_NODESET_SIZE];
  std::vector<Socket> sockets;

  public:
    Machine& operator=(const Machine& newMachine);
    Machine();
    ~Machine();
    int getNumberOfSockets();
    Socket getSocket();
    int initializeMachine(hwloc_topology_t topology);
    int initializeNonNUMAMachine(hwloc_obj_t obj, hwloc_topology_t topology);
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
  };



