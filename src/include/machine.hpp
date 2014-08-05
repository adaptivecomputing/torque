#include <iostream>
#include <string>
#include <hwloc.h>

class Core
  {
  int id;
  int numberOfProcessingUnits;
  std::vector<int> pu;

  public:
    Core();
    ~Core();
    int getid();
    int getNumberOfProcessingUnits();
    int initializeCore(hwloc_obj_t obj);
    std::vector<int> getPU();
  };

class Chip
  {
  int id;
  hwloc_uint64_t memory;
  std::vector<Core> cores;
  int totalProcessingUnits;
  /* need a bit map for available cores */
  /* need bitmap for used cores */

  public:
    Chip();
    ~Chip();
    int getid();
    int getTotalCores();
    int getTotalProcessingUnits();
    bool isThreaded;
    int getThreads();
    int getMemoryInBytes();
    int initializeChip(hwloc_obj_t obj);
    std::vector<Core> getCores();
    /* bitmap for getAvailableCores() */
    /* bitmap for getUsedCores() */
  };

class Socket
  {
  int               id;   /* logical index of socket returned by hwloc */
  hwloc_uint64_t  memory;
  int numberOfProcessingUnits;
  int numberOfCores;
  std::vector<Chip> chips;
  /*std::vectory<PCIDevice> PCIDevices;*/

  public:
    Socket();
    ~Socket();
    int initializeSocket(hwloc_obj_t obj);
    int initializeIntelSocket(hwloc_obj_t obj, Chip &newChip);
    int getTotalCores();
    int getTotalProcessors();
    int getid();
    int getTotalChips();
    hwloc_uint64_t getMemoryInBytes();
    bool isNUMA;
    /*std::vector<PCIDevice> getPCIDevices();*/
  };


class Machine
  {
  int numberOfSockets;
  hwloc_uint64_t  totalMemoryInBytes;
  int totalChips;
  int totalCores;
  int totalThreads;
  int availableSockets;
  int availableChips;
  int availableCores;
  int availableThreads;
  std::vector<Socket> sockets;

  public:
    Machine& operator=(const Machine& newMachine);
    Machine();
    ~Machine();
    int initializeMachine(hwloc_topology_t topology);
    hwloc_uint64_t getTotalMemory();
    Socket getSocket();
    int getNumberOfSockets();
    int getTotalChips();
    int getTotalCores();
    int getTotalThreads();
    int getAvailableSockets();
    int getAvailableChips();
    int getAvailableMemory();
    int getAvailableCores();
    int getAvailableThreads();
    bool isNUMA;
  };



