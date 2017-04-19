
#include <string>

#include "numa_constants.h"

extern const char *NAME;
extern const char *NUMA_INDEX;
extern const char *ARCHITECTURE;
extern const char *OPERATING_SYSTEM;
extern const char *UNAME;
extern const char *SESSIONS;
extern const char *NSESSIONS;
extern const char *NUSERS;
extern const char *IDLETIME;
extern const char *TOTAL_MEMORY;
extern const char *AVAILABLE_MEMORY;
extern const char *PHYSICAL_MEMORY;
extern const char *NCPUS;
extern const char *LOADAVE;
extern const char *MESSAGE;
extern const char *NETLOAD;
extern const char *SIZE; // optional
extern const char *STATE;
extern const char *JOBS;
extern const char *CPU_TIME_USED;
extern const char *MEMORY_USED;
extern const char *VMEMORY_USED;
extern const char *JOBDATA; // optional
extern const char *VARATTR;
extern const char *LAYOUT;
extern const char *CPU_CLOCK;
extern const char *MACADDR;
extern const char *PLUGIN_RESOURCES;
extern const char *GENERIC_RESOURCES;
extern const char *GENERIC_METRICS;
extern const char *MIC_STR;
extern const char *GPU_STR;
extern const char *GPU_DISPLAY;
extern const char *GPU_ID;
extern const char *GPU_PCI_DEVICE_ID;
extern const char *GPU_PCI_LOCATION_ID;
extern const char *GPU_PRODUCT_NAME;
extern const char *GPU_FAN_SPEED;
extern const char *GPU_MEMORY_TOTAL;
extern const char *GPU_MEMORY_USED;
extern const char *GPU_MODE;
extern const char *GPU_UTILIZATION;
extern const char *GPU_ECC_MODE;
extern const char *GPU_SINGLE_BIT_ERRORS;
extern const char *GPU_DOUBLE_BIT_ERRORS;
extern const char *GPU_TEMPERATURE;
extern const char *TIMESTAMP;
extern const char *DRIVER_VERSION;
extern const char *MIC_ID;
extern const char *NUM_CORES;
extern const char *NUM_THREADS;
extern const char *PHYSMEM;
extern const char *FREE_PHYSMEM;
extern const char *FREE_SWAP;
extern const char *MAX_FREQUENCY;
extern const char *ISA;
extern const char *LOAD;
extern const char *NORMALIZED_LOAD;
extern const char *NODE_INT;
extern const char *ARCH;
extern const char *CCU;
extern const char *CPROC;
extern const char *APROC;
extern const char *CMEMORY;
extern const char *RESERVATION_ID;
extern const char *FEATURE_LIST;
extern const char *NODE_INDEX;
extern const char *ACCELERATORS;
extern const char *FAMILY;
extern const char *CLOCK_MHZ;


/*
 * The mom update comes in the following form:
 *
node : {
  "name"    : "name"
  "numa"    : numa_index # only for large-scale numa systems
  "arch"    : "architecture",
  "opsys"   : "os",
  "uname"   : "uname",
  "sessions" : "session1,session2,...",
  "nsessions" : "number of sessions",
  "nusers"    : "number of users",
  "idletime"  : "idletime",
  "totmem"    : "totmemkb",
  "availmem"  : "availmemkb",
  "physmem"   : "physmemkb",
  "ncpus"     : "ncpus",
  "loadave"   : "loadave",
  "message"   : "message",
  "gres"      : "gres",
  "netload"   : "netload",
  "size"      : "size", # optional
  "state"     : "state",
  "jobs"      : {
    "jobid" : {
      "cput"             : "cpu time",
      "mem"              : "mem_usedkb",
      "vmem"             : "vmem_usedkb",
      "plugin_resources" : { # optional
        "pr_name1" : "pr_val1",
        "pr_name2" : "pr_val2",
        ...
      },
    "jobid2" : {
      ...
      },
    }
  }
  "jobdata"   : "jobdata", # optional
  "varattr"   : "varattr",
  "cpuclock"  : "clock mode",
  "macaddr"   : "macaddr",
  "layout" : {
    Machine layout json
  }
  "version" : "version",
  "plugin_resources" : {
    "generic_resources" : { # optional
      "gres1" : "gresval1",...
      },
    "varattrs" : { # optional 
      "varattr1" : "varattrval1",..
      },
    "generic_metrics" : { # optional
      "gmetric1" : gmetricdouble1,...
      },
    "features" : "feature1,feature2,..." # optional
    }
  "gpus" : [
      {
      "gpu_display" : "[En|Dis]abled",
      "gpuid" : "bus id",
      "gpu_pci_device_id" : "pci device id",
      "gpu_pci_location_id" : "bus id",
      "gpu_product_name" : "name",
      "gpu_fan_speed" : "fan speed",
      "gpu_memory_total" : "mem MB",
      "gpu_memory_used" : "mem MB",
      "gpu_mode" : "mode",
      "gpu_utilization" : "utilization",
      "gpu_ecc_mode" : "[En|Dis]abled",
      "gpu_single_bit_errors" : "count",
      "gpu_double_bit_errors" : "count",
      "gpu_temperature" : degrees
    } , ...
  ],
  "mics" : [
      {
      "mic_id" : micid,
      "num_cores" : count,
      "num_threads" : count,
      "physmem" : "value",
      "free_physmem" : "value",
      "swap" : "value",
      "free_swap" : "value",
      "max_frequency" : "value",
      "isa" : "value",
      "load" : "value",
      "normalized_load" : "value"
    } , ...
  ]
}

ALPS
: {
  "node int" : {
    "node"           : "node id string",
    "ARCH"           : "architecture",
    "name"           : "node_name",
    "CCU"            : num_compute_units,
    "CPROC"          : num_procs,
    "APROC"          : available_procs,
    "CMEMORY"        : "memorykb",
    "socket"         : socket_count,
    "totmem"         : "memorykb",
    "physmem"        : "memorykb",
    "availmem"       : "available_memorykb",
    "reservation_id" : "rsv_id",   # may not exist
    "state"          : "node_state",
    "feature_list"   : "feature1,featurelist", # may not exist
    "node_index"     : index,
    "accelerators"   : [ # may not exist
        {
        "gpu_id" : "id",
        "state"  : "state",
        "family" : "family",
        "memory" : "memory",
        "clock_mhz" : "clock_mhz"
        },
        ...
      ]
    }
  } ,
  "node int" : {
    ...
  }, ...
}*/
