#ifndef __MACHINE_HPP__
#define __MACHINE_HPP__
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <hwloc.h>
#ifdef NVIDIA_GPUS
  #ifdef NVML_API
#include <hwloc/nvml.h>
  #endif
#endif

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

extern const int MIC_TYPE;
extern const int GPU;
extern const int CORE;
extern const int THREAD;

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
  int            type;
  bool           busy;

  public:
    PCI_Device();
    PCI_Device(const PCI_Device &other);
    PCI_Device &operator=(const PCI_Device &other);
    ~PCI_Device();
    int initializePCIDevice(hwloc_obj_t, int, hwloc_topology_t);
#ifdef MIC
    void initializeMic(int idx, hwloc_topology_t topology);
#endif

#ifdef NVIDIA_GPUS
    void initializeGpu(int idx, hwloc_topology_t topology);
#endif
    void displayAsString(stringstream &out) const;
    void setName(const string &name);
    void setId(int id);
    const char *get_cpuset() const;
    int  get_type() const;
    int  get_id() const;
    bool is_busy() const;
    void set_type(int type);
    void set_state(bool in_use);
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
    Core(const std::string &layout);
    ~Core();
    int get_id() const;
    int getNumberOfProcessingUnits();
    int initializeCore(hwloc_obj_t obj, hwloc_topology_t topology);
    std::vector<int> getPU();
    void displayAsString(stringstream &out) const;
    int  get_open_processing_unit();
    int  add_processing_unit(int which, int os_index);
    bool is_free() const;
    bool free_pu_index(int index, bool &core_is_now_free);
    void unit_test_init(); // Only for unit tests
    void append_indices(std::vector<int> core_indices, int which) const;
    bool reserve_processing_unit(int index);
    int  get_thread_index(int thread_indice) const;
  };



class Chip
  {
  int                     id;
  int                     totalCores;
  int                     totalThreads;
  int                     availableCores;
  int                     availableThreads;
  int                     total_gpus;
  int                     available_gpus;
  int                     total_mics;
  int                     available_mics;
  bool                    chip_exclusive;
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
    Chip(int execution_slots, int &es_remainder, int &per_numa_remainder);
    Chip(const std::string &layout, std::vector<std::string> &valid_ids);
    Chip(const Chip &other);
    Chip &operator=(const Chip &other);
    ~Chip();
    int get_id() const;
    int getTotalCores() const;
    int getTotalThreads() const;
    int get_total_gpus() const;
    int getAvailableCores() const;
    int getAvailableThreads() const;
    hwloc_uint64_t getAvailableMemory() const;
    hwloc_uint64_t getMemory() const;
    int get_available_gpus() const;
    int get_available_mics() const;
    int initializeChip(hwloc_obj_t obj, hwloc_topology_t);
    int initializeNonNUMAChip(hwloc_obj_t, hwloc_topology_t);
    int initializePCIDevices(hwloc_obj_t, hwloc_topology_t);
    void initialize_cores_from_strings(std::string &cores, std::string &threads);
    void initialize_accelerators_from_strings(std::string &gpus, std::string &mics);
    bool chipIsAvailable() const;
    bool is_completely_free() const;
#ifdef MIC
    int initializeMICDevices(hwloc_obj_t, hwloc_topology_t);
#endif
    void parse_values_from_json_string(const std::string &json_layout, std::string &cores,
                                       std::string &threads, std::string &gpus, std::string &mics,
                                       std::vector<std::string> &valid_ids);

#ifdef NVIDIA_GPUS
  #ifdef NVML_API
    int initializeNVIDIADevices(hwloc_obj_t, hwloc_topology_t);
  #endif
#endif

    std::vector<Core> getCores();
    void displayAsString(stringstream &out) const;
    void displayAsJson(stringstream &out, bool include_jobs) const;
    void displayAllocationsAsJson(stringstream &out) const;
    void setMemory(hwloc_uint64_t mem);
    void setId(int id);
    void setCores(int cores); // used for unit tests
    void setThreads(int threads); // used for unit tests
    void setChipAvailable(bool available);
    void set_gpus(int gpus); // used for unit tests
    double how_many_tasks_fit(const req &r, int place_type) const;
    bool has_socket_exclusive_allocation() const;
    bool task_will_fit(const req &r, int place_type) const;
    int  free_core_count() const;
    void calculateStepCounts(const int lprocs_per_task, const int pu_per_task, int &step, int &step_rem, int &place_count, int &place_count_rem);
    bool spread_place(req &r, allocation &master, allocation &to_place, allocation &remainder);
    bool spread_place_cores(req &r, allocation &master, int &remaining_cores, int &remaining_lprocs, int &gpus, int &mics);
    bool spread_place_threads(req &r, allocation &master, int &remaining_cores, int &remaining_lprocs, int &gpus, int &mics);
    void place_all_execution_slots(req &r, allocation &task_alloc);
    int  place_task(req &r, allocation &a, int to_place, const char *hostname);
    void place_tasks_execution_slots(int to_bind, int to_place, allocation &a, int type);
    void place_task_for_legacy_threads(int threads_to_bind, int threads_to_place, allocation &master, allocation &a);
    void uncount_allocation(int index);
    bool free_task(const char *jobid);
    void remove_last_allocation(const char *jobid);
    void free_cpu_index(int index, bool increment_available_cores);
    void make_core(int id = 0); // used for unit tests
    void set_cpuset(const char *cpuset); // used for unit tests
    bool partially_place_task(allocation &remaining, allocation &master);
    bool store_pci_device_appropriately(PCI_Device &device, bool force);
    bool cpusets_overlap(const std::string &other) const;
    void place_accelerators(allocation &remaining, allocation &a);
    int  reserve_accelerator(int type);
    void free_accelerators(allocation &a);
    void free_accelerator(int index, int type);
    void initialize_allocations(char *allocations, std::vector<std::string> &valid_ids);
    void initialize_allocation(char *allocation, std::vector<std::string> &valid_ids);
    void aggregate_allocation(allocation &a);
    void adjust_open_resources();
    void reserve_allocation_resources(allocation &a);
    void aggregate_allocations(vector<allocation> &master_list);
    bool reserve_core(int core_index, allocation &a);
    bool reserve_chip_core(int core_index, allocation &a);
    bool getOpenThreadVector(std::vector<int> &slots, int execution_slots_per_task);
    bool getContiguousThreadVector(std::vector<int> &slots, int execution_slots_per_task);
    bool getContiguousCoreVector(std::vector<int> &slots, int execution_slots_per_task);
    bool reserve_thread(int core_index, allocation &a);
    bool reserve_chip_thread(int core_index, allocation &a);
    bool reserve_place_thread(int core_index, allocation &a);
    bool reserve_chip_place_thread(int core_index, allocation &a);
    void save_allocations(const Chip &other);
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
  bool                  socket_exclusive;
  hwloc_const_cpuset_t  socket_cpuset;
  hwloc_const_nodeset_t socket_nodeset;

  public:
    Socket();
    Socket(int execution_slots, int numa_nodes, int &es_remainder);
    Socket(const std::string &layout, std::vector<std::string> &valid_ids);
    ~Socket();
    int initializeSocket(hwloc_obj_t obj);
    int initializeIntelSocket(hwloc_obj_t obj, Chip &newChip);
    int initializeNonNUMASocket(hwloc_obj_t obj, hwloc_topology_t);
    int getTotalChips() const;
    int getTotalCores() const;
    int getTotalThreads() const;
    int get_total_gpus() const;
    int get_available_gpus() const;
    int getAvailableChips() const;
    int getAvailableCores() const;
    int get_free_cores() const;
    int getAvailableThreads() const;
    hwloc_uint64_t getAvailableMemory() const;
    int getid();
    hwloc_uint64_t getMemory() const;
    hwloc_uint64_t get_memory_for_completely_free_chips(unsigned long diff, int &count) const;
    int initializeAMDSocket(hwloc_obj_t, hwloc_topology_t);
    int initializeIntelSocket(hwloc_obj_t, hwloc_topology_t);
    void setMemory(hwloc_uint64_t mem);
    void displayAsString(stringstream &out) const;
    void displayAsJson(stringstream &out, bool include_jobs) const;
    void setId(int id);
    void addChip(); // used for unit tests
    double how_many_tasks_fit(const req &r, int place_type) const;
    void place_all_execution_slots(req &r, allocation &task_alloc);
    bool spread_place(req &r, allocation &master, allocation &to_place, allocation &remainder, bool chips);
    bool spread_place_pu(req &r, allocation &task_alloc, int &cores, int &lprocs, int &gpus, int &mics);
    int  place_task(req &r, allocation &a, int to_place, const char *hostname);
    bool free_task(const char *jobid);
    bool is_available() const;
    bool is_completely_free() const;
    bool fits_on_socket(const allocation &remaining) const;
    bool partially_place(allocation &remaining, allocation &a);
    bool store_pci_device_appropriately(PCI_Device &device, bool force);
    void update_internal_counts(vector<allocation> &allocs);
    int  get_gpus_remaining();
    int  get_mics_remaining();
    void save_allocations(const Socket &other);
  };



class Machine
  {
  int                 hardwareStyle; /* Intel or AMD */
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
#ifdef NVIDIA_GPUS
  #ifdef NVML_API
  hwloc_obj_t get_non_nvml_device(hwloc_topology_t topology, nvmlDevice_t device, std::set<hwloc_obj_t> &);
  #endif
#endif

  public:
    Machine& operator=(const Machine& newMachine);
    Machine(const std::string &layout, std::vector<std::string> &valid_ids);
    Machine(int execution_slots, int numa_nodes, int sockets);
    Machine();
    ~Machine();
    Socket getSocket();
    int initializeMachine(hwloc_topology_t topology);
    int initializeNonNUMAMachine(hwloc_obj_t obj, hwloc_topology_t topology);
    int initializeNVIDIADevices(hwloc_obj_t obj, hwloc_topology_t topology);
    hwloc_uint64_t getTotalMemory() const;
    int getTotalSockets() const;
    int getTotalChips() const;
    int getTotalCores() const;
    int getTotalThreads() const;
    int get_total_gpus() const;
    int getAvailableSockets() const;
    int getAvailableChips() const;
    hwloc_uint64_t getAvailableMemory() const;
    int getAvailableCores() const;
    int getAvailableThreads() const;
    int getDedicatedSockets() const;
    int getDedicatedChips() const;
    int getDedicatedCores() const;
    int getDedicatedThreads() const;
    int getHardwareStyle() const;
    bool isNUMA;
    void displayAsString(stringstream &out) const;
    void displayAsJson(stringstream &out, bool include_jobs) const;
    void insertNvidiaDevice(PCI_Device& device);
    void store_device_on_appropriate_chip(PCI_Device &device, bool no_info);
    void place_all_execution_slots(req &r, allocation &master, const char *hostname);
    int  spread_place(req &r, allocation &master, int tasks_for_node, const char *hostname);
    int  spread_place_pu(req &r, allocation &master, int tasks_for_node, const char *hostname);
    int  place_job(job *pjob, cgroup_info &cgi, const char *hostname, bool legacy_vmem);
    void setMemory(long long mem); 
    void addSocket(int count); // used for unit tests
    void setIsNuma(bool is_numa); // used for unit tests
    void save_allocations(const Machine &other);
    void free_job_allocation(const char *jobid);
    int  fit_tasks_within_sockets(req &r, allocation &job_alloc, const char *hostname, int &remaining_tasks);
    void place_remaining(req &to_split, allocation &master, int &remaining_tasks, const char *hostname);
    int  how_many_tasks_can_be_placed(req &r) const;
    void update_internal_counts();
    void populate_job_ids(std::vector<std::string> &job_ids) const;
    bool check_if_possible(int &sockets, int &numa_nodes, int &cores, int &threads) const;
    void compare_remaining_values(allocation &remaining, const char *caller) const;
  };

extern Machine this_node;


#endif  /* __MACHINE_HPP__ */
