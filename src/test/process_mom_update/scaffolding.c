#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>
#include <string>

#include "threadpool.h"
#include "attribute.h"
#include "pbs_nodes.h"
#include "pbs_job.h"
#include "u_tree.h"
#include "id_map.hpp"

#include "machine.hpp"

char        server_name[PBS_MAXSERVERNAME + 1]; /* host_name[:service|port] */
int         allow_any_mom;
int         LOGLEVEL = 10;
int         event_logged;
const char *dis_emsg[] =
  {
  "No error",
  "Input value too large to convert to this type",
  "Tried to write floating point infinity",
  "Negative sign on an unsigned datum",
  "Input count or value has leading zero",
  "Non-digit found where a digit was expected",
  "Input string has an embedded ASCII NUL",
  "Premature end of message",
  "Unable to calloc enough space for string",
  "Supporting protocol failure",
  "Protocol failure in commit",
  "End of File",
  "Invalid condition in code"
  };

attribute_def node_attr_def[1];



void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) 
  {
  event_logged++;
  }
void log_err(int errnum, const char *routine, const char *text) {}
void close_conn(int sd, int has_mutex) {}
id_map job_mapper;
bool exit_called = false;

char *threadsafe_tokenizer(

  char **str,    /* M */
  const char  *delims) /* I */

  {
  return(NULL);
  }

int get_svr_attr_l(

  int   attr_index,
  long *l)

  {
  return(0);
  }

int unlock_ji_mutex(

  job        *pjob,
  const char *id,
  const char *msg,
  int        logging)
  
  {
  return(0);
  }

int modify_job_attr(

  job      *pjob,  /* I (modified) */
  svrattrl *plist, /* I */
  int       perm,
  int      *bad)   /* O */
  
  {
  return(0);
  }

int diswsl(

  struct tcp_chan *chan,
  long value)
  
  {
  return(0);
  }

void *sync_node_jobs(

  void *vp)

  {
  return(NULL);
  }

threadpool_t *task_pool;

int enqueue_threadpool_request(

  void *(*func)(void *),
  void *arg,
  threadpool_t *tp)

  {
  return(0);
  }

int lock_node(
    
  struct pbsnode *the_node,
  const char     *id,
  const char     *msg,
  int             logging)

  {
  return(0);
  }

void update_node_state(

  struct pbsnode *np,         /* I (modified) */
  int             newstate)   /* I (one of INUSE_*) */

  {
  }

struct pbsnode *find_nodebyname(

  const char *nodename) /* I */

  {
  return(NULL);
  }

int unlock_node(
    
  struct pbsnode *the_node,
  const char     *id,
  const char     *msg,
  int             logging)

  {
  return(0);
  }

job *svr_find_job(const char *jobid, int subjob)
  
  {
  return(NULL);
  }

struct prop *init_prop(

  char *pname) /* I */
  
  {
  return(NULL);
  }

int is_job_on_node(

  struct pbsnode *pnode, /* I */
  int             jobid) /* I */

  {
  return(0);
  }

int node_status_list(

  pbs_attribute *new_attr,           /*derive status into this pbs_attribute*/
  void          *pnode,         /*pointer to a pbsnode struct     */
  int            actmode)       /*action mode; "NEW" or "ALTER"   */

  {
  return(0);
  }

struct pbsnode *AVL_find(
    
  u_long   key,
  uint16_t port,
  AvlTree  tree)

  {
  return(NULL);
  }

int decode_arst(

  pbs_attribute *patr,    /* O (modified) */
  const char   *name,    /* I pbs_attribute name (notused) */
  const char *rescn,   /* I resource name (notused) */
  const char    *val,     /* I pbs_attribute value */
  int            perm) /* only used for resources */

  {
  return(0);
  }

void free_arst(pbs_attribute *attr) {}

int update_nodes_file(
    
  struct pbsnode *held)

  {
  return(0);
  }

int node_micstatus_list(

  pbs_attribute *new_attr,      /* derive status into this pbs_attribute*/
  void          *pnode,    /* pointer to a pbsnode struct     */
  int            actmode)  /* action mode; "NEW" or "ALTER"   */

  {
  return(0);
  }

int gpu_has_job(

  struct pbsnode *pnode,
  int  gpuid)

  {
  return(0);
  }

int gpu_entry_by_id(

  struct pbsnode *pnode,  /* I */
  const char     *gpuid,
  int             get_empty)

  {
  return(0);
  }

void log_ext(

  int         errnum,   /* I (errno or PBSErrno) */
  const char *routine,  /* I */
  const char *text,     /* I */
  int         severity) /* I */

  {}

int diswsi(

  struct tcp_chan *chan,
  int value)

  {
  return(0);
  }

int node_gpustatus_list(

  pbs_attribute *new_attr,      /* derive status into this pbs_attribute*/
  void          *pnode,    /* pointer to a pbsnode struct     */
  int            actmode)  /* action mode; "NEW" or "ALTER"   */

  {
  return(0);
  }

void clear_nvidia_gpus(struct pbsnode *np) {}

const char *id_map::get_name(int id)
  {
  return("");
  }

int id_map::get_id(

  const char *name)

  {
  return(0);
  }

void write_node_power_state(void)
  {
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

void move_past_whitespace(

  char **str)

  {
  if ((str == NULL) ||
      (*str == NULL))
    return;

  char *current = *str;

  while (is_whitespace(*current) == TRUE)
    current++;

  *str = current;
  } // END move_past_whitespace()

void add_range_to_string(

  std::string &range_string,
  int          begin,
  int          end)

  {
  char buf[1024];

  if (begin == end)
    {
    if (range_string.size() == 0)
      sprintf(buf, "%d", begin);
    else
      sprintf(buf, ",%d", begin);
    }
  else
    {
    if (range_string.size() == 0)
      sprintf(buf, "%d-%d", begin, end);
    else
      sprintf(buf, ",%d-%d", begin, end);
    }

  range_string += buf;
  } // END add_range_to_string()

void translate_vector_to_range_string(std::string &range_string, const std::vector<int> &indices)
  {
  // range_string starts empty
  range_string.clear();

  if (indices.size() == 0)
    return;

  int first = indices[0];
  int prev = first;

  for (unsigned int i = 1; i < indices.size(); i++)
    {
    if (indices[i] == prev + 1)
      {
      // Still in a consecutive range
      prev = indices[i];
      }
    else
      {
      add_range_to_string(range_string, first, prev);

      first = prev = indices[i];
      }
    }

  // output final piece
  add_range_to_string(range_string, first, prev);
  }

void translate_range_string_to_vector(const char *range_string, std::vector<int> &indices)
  {
  char *str = strdup(range_string);
  char *ptr = str;
  int   prev = 0;
  int   curr;

  while (is_whitespace(*ptr))
    ptr++;

  while (*ptr != '\0')
    {
    char *old_ptr = ptr;
    prev = strtol(ptr, &ptr, 10);

    if (ptr == old_ptr)
      {
      // This means *ptr wasn't numeric, error. break out to prevent an infinite loop
      break;
      }
    
    if (*ptr == '-')
      {
      ptr++;
      curr = strtol(ptr, &ptr, 10);

      while (prev <= curr)
        {
        indices.push_back(prev);

        prev++;
        }

      while ((*ptr == ',') ||
          (is_whitespace(*ptr)))
        ptr++;
      }
    else
      {
      indices.push_back(prev);

      while ((*ptr == ',') ||
             (is_whitespace(*ptr)))
        ptr++;
      }
    }

  free(str);
  }

void capture_until_close_character(

  char        **start,
  std::string  &storage,
  char          end) 
  
  {
  if ((start == NULL) ||
      (*start == NULL))
    return;

  char *val = *start;
  char *ptr = strchr(val, end);

  // Make sure we found a close quote and this wasn't an empty string
  if ((ptr != NULL) &&
      (ptr != val))
    {
    storage = val;
    storage.erase(ptr - val);
    *start = ptr + 1; // add 1 to move past the character
    }
  else
    {
    // Make sure we aren't returning stale values
    storage.clear();
    }
  }

id_map::id_map(){}

id_map::~id_map(){}

bool task_hosts_match(const char *one, const char *two)
  {
  return(true);
  }

#include "../../lib/Libutils/machine.cpp"
#include "../../lib/Libutils/numa_chip.cpp"
#include "../../lib/Libutils/numa_core.cpp"
#include "../../lib/Libutils/numa_socket.cpp"
#include "../../lib/Libutils/numa_pci_device.cpp"
#include "../../lib/Libutils/allocation.cpp"
#include "../../lib/Libattr/req.cpp"
#include "../../lib/Libattr/complete_req.cpp"

#ifdef NVML_API
int Machine::initializeNVIDIADevices(hwloc_obj_t machine_obj, hwloc_topology_t topology)
  {
  return(0);
  }
#endif

#ifdef MIC
void PCI_Device::initializeMic(int x, hwloc_topology *fred)
  {
  return;
  }

int Chip::initializeMICDevices(hwloc_obj_t chip_obj, hwloc_topology_t topology)
  {
  return(0);
  }
#endif

#ifdef NVIDIA_GPUS
void PCI_Device::initializeGpu(int idx, hwloc_topology_t topology)
    {
      return;
        }

int Machine::get_total_gpus() const
  {
  return(0);
  }

#endif

