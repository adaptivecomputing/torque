 #include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "u_tree.h"
#include "tcp.h"
#include "pbs_job.h"
#include "pbs_nodes.h"
#include "mutex_mgr.hpp"
#include "threadpool.h"
#include "execution_slot_tracker.hpp"
#include "execution_slot_tracker.hpp"
#include "mom_hierarchy_handler.h"
#include "id_map.hpp"
#include "node_manager.h"
#include "pbs_ifl.h"


id_map      job_mapper;
int         allow_any_mom;
AvlTree     ipaddrs = NULL;
int         LOGLEVEL;
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

threadpool_t *task_pool;

#define ATTR_NODE_total_sockets        "total_sockets"
#define ATTR_NODE_total_numa_nodes     "total_numa_nodes"
#define ATTR_NODE_total_cores          "total_cores"
#define ATTR_NODE_total_threads        "total_threads"
#define ATTR_NODE_dedicated_sockets    "dedicated_sockets"
#define ATTR_NODE_dedicated_numa_nodes "dedicated_numa_nodes"
#define ATTR_NODE_dedicated_cores      "dedicated_cores"
#define ATTR_NODE_dedicated_threads    "dedicated_threads"


char * netaddr(struct sockaddr_in *ap)
  {
  return(NULL);
  }

void *send_hierarchy_threadtask(void *vp)
  {
  return(NULL);
  }

int create_partial_pbs_node(

  char          *nodename,
  unsigned long  addr,
  int            perms)

  {
  return(0);
  }

void log_record(

  int         eventtype,  /* I */
  int         objclass,   /* I */
  const char *objname,    /* I */
  const char *text)       /* I */ {}

int insert_addr_name_info(
    
  char               *hostname,
  char               *full_hostname,
  struct sockaddr_in *sai)

  {
  return(0);
  }

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

unsigned long disrul(

  struct tcp_chan *chan,  /* I */
  int *retval)  /* O */

  {
  return(0);
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

int process_alps_status(

  char                     *nd_name,
  std::vector<std::string> &status_info)

  {
  return(0);
  }

char *get_cached_nameinfo(
    
  struct sockaddr_in  *sai)

  {
  return(NULL);
  }

int write_tcp_reply(

  struct tcp_chan *chan,
  int protocol,
  int version,
  int command,
  int exit_code)

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

char *disrst(

  struct tcp_chan *chan,
  int *retval)

  {
  return(NULL);
  }

long disrsl(

  struct tcp_chan *chan,
  int *retval)

  {
  return(0);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_err(int errnum, const char *routine, const char *text) {}
void close_conn(int sd, int has_mutex) {}

job *get_job_from_jobinfo(
    
  struct jobinfo *jp,
  struct pbsnode *pnode)

  {
  return(NULL);
  }

char * netaddr_long(long ap, char *out)

  {
  return(NULL);
  }

mutex_mgr::mutex_mgr(pthread_mutex_t *, bool a)
  {
  }

int mutex_mgr::unlock() 
  {
  return(0);
  }

void mutex_mgr::mark_as_locked() {}

int unlock_ji_mutex(

  job        *pjob,
  const char *id,
  const char *msg,
  int        logging)

  {
  return(0);
  }

job *get_job_from_job_usage_info(job_usage_info *jui, struct pbsnode *pnode)
  {
  return(NULL);
  }

char *get_cached_nameinfo(const struct sockaddr_in  *sai)
  {
  return(NULL);
  }

struct addrinfo * insert_addr_name_info(struct addrinfo *pAddrInfo,const char *host)
  {
  return(NULL);
  }

mutex_mgr::~mutex_mgr() {}

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

execution_slot_tracker::execution_slot_tracker() {}

execution_slot_tracker &execution_slot_tracker::operator =(const execution_slot_tracker &other)
  {
  return *this;
  }

execution_slot_tracker::execution_slot_tracker(execution_slot_tracker const&){}

mom_hierarchy_handler hierarchy_handler; //The global declaration.

void mom_hierarchy_handler::sendHierarchyToANode(struct pbsnode *node)
  {
  }

int disrsi(tcp_chan *channel, int *ret)
  {
  return(PBSE_NONE);
  }

int disrui(tcp_chan *channel, int *ret)
  {
  return(PBSE_NONE);
  }

                                                                                                                                           
void write_node_power_state(void)
  {
  return;
  }

int gpu_entry_by_id(struct pbsnode *pnode, const char *gpuid, int get_empty)
  {
  return(0);
  }

int update_nodes_file(struct pbsnode *held)
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

void log_ext(

  int         errnum,   /* I (errno or PBSErrno) */
  const char *routine,  /* I */
  const char *text,     /* I */
  int         severity) /* I */

  {
  return;
  }

struct prop *init_prop(

  char *pname) /* I */

  {
  return(new prop(pname));
  }  /* END init_prop() */



void *sync_node_jobs(void *vp)
  {
  return(NULL);
  }

int is_job_on_node(

  struct pbsnode *pnode,           /* I */
  int             internal_job_id) /* I */

  {
  return(1);
  }



int node_gpustatus_list(

  pbs_attribute *new_attr,      /* derive status into this pbs_attribute*/
  void          *pnode,    /* pointer to a pbsnode struct     */
  int            actmode)  /* action mode; "NEW" or "ALTER"   */

  {
  return(0);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  return(0);
  }

void clear_nvidia_gpus(

  struct pbsnode *np)  /* I */
  
  {
  return;
  }

int node_status_list(

  pbs_attribute *new_attr,           /*derive status into this pbs_attribute*/
  void          *pnode,         /*pointer to a pbsnode struct     */
  int            actmode)       /*action mode; "NEW" or "ALTER"   */

  {
  return(0);
  }

int encode_ntype(pbs_attribute*, tlist_head*, const char*, const char*, int, int)
  {
  return(0);
  }


int node_numa_action(pbs_attribute*, void*, int)
  {
  return(0);
  }

int node_mom_rm_port_action(pbs_attribute*, void*, int)
  {
  return(0);
  }


int node_state(pbs_attribute*, void*, int)
  {
  return(0);
  }

int node_power_state(pbs_attribute*, void*, int)
  {
  return(0);
  }

int node_ttl(pbs_attribute*, void*, int)
  {
  return(0);
  }

int node_acl(pbs_attribute*, void*, int)
  {
  return(0);
  }

int node_requestid(pbs_attribute*, void*, int)
  {
  return(0);
  }

int node_np_action(pbs_attribute*, void*, int)
  {
  return(0);
  }

int node_mom_port_action(pbs_attribute*, void*, int)
  {
  return(0);
  }


int node_gpus_action(pbs_attribute*, void*, int)
  {
  return(0);
  }

int node_mics_action(pbs_attribute*, void*, int)
  {
  return(0);
  }

int set_power_state(pbs_attribute*, pbs_attribute*, enum batch_op)
  {
  return(0);
  }

int set_node_props(pbs_attribute*, pbs_attribute*, enum batch_op)
  {
  return(0);
  }

int set_null(pbs_attribute*, pbs_attribute*, enum batch_op)
  {
  return(0);
  }

int set_node_state(pbs_attribute*, pbs_attribute*, enum batch_op)
  {
  return(0);
  }

int set_node_ntype(pbs_attribute*, pbs_attribute*, enum batch_op)
  {
  return(0);
  }

int encode_state(pbs_attribute *, tlist_head *, const char *, const char *, int, int)
  {
  return(0);
  }

int encode_jobs(pbs_attribute*, tlist_head*, const char*, const char*, int, int)
  {
  return(0);
  }

int encode_power_state(pbs_attribute *, tlist_head *, const char *, const char *, int, int)
  {
  return(0);
  }

int numa_str_action(pbs_attribute *, void *,int)
  {
  return(0);
  }


int node_note(pbs_attribute*, void*, int)
  {
  return(0);
  }

int node_ntype(

  pbs_attribute *new_attr,      /*derive ntype into this pbs_attribute*/
  void          *pnode,   /*pointer to a pbsnode struct     */
  int            actmode) /*action mode; "NEW" or "ALTER"   */

  {
  return(0);
  }

int set_note_str(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op)
  {
  return(0);
  }

int decode_state(pbs_attribute*, const char*, const char*, const char*, int)
  {
  return(0);
  }

int decode_power_state(pbs_attribute*, const char*, const char*, const char*, int)
  {
  return(0);
  }

int decode_utc(pbs_attribute*, const char*, const char*, const char*, int)
  {
  return(0);
  }

int decode_props(pbs_attribute*, const char*, const char*, const char*, int)
  {
  return(0);
  }

int decode_ntype(pbs_attribute*, const char*, const char*, const char*, int)
  {
  return(0);
  }

int decode_null(pbs_attribute*, const char*, const char*, const char*, int)
  {
  return(0);
  }

int gpu_str_action(pbs_attribute*, void*, int)
  {
  return(0);
  }

int node_prop_list(pbs_attribute*, void*, int)
  {
  return(0);
  }

int ctnodes(

  char *spec)

  {
  int   ct = 0;
  char *pc;

  while (1)
    {
    while (isspace((int)*spec))
      ++spec;

    if (isdigit((int)*spec))
      ct += atoi(spec);
    else
      ++ct;

    if ((pc = strchr(spec, '+')) == NULL)
      break;

    spec = pc + 1;
    }  /* END while (1) */

  return(ct);
  }  /* END ctnodes() */

int csv_length(const char *csv_str)
  {
  int  length = 0;
  const char *cp;

  if (!csv_str || *csv_str == 0)
    return(0);

  length++;

  cp = csv_str;

  while ((cp = strchr(cp, ',')))
    {
    cp++;
    length++;
    }

  return(length);
  }

#define NBUFFERS        32
char *csv_nth(const char *csv_str, int n)
  {
  int  i;
  const char *cp;
  char *tp;
  static char buffer[NBUFFERS][128];
  static  int     buffer_index;

  if (!csv_str || *csv_str == 0)
    return(0);

  cp = csv_str;

  for (i = 0; i < n; i++)
    {
    if (!(cp = strchr(cp, ',')))
      {
      return(0);
      }

    cp++;
    }

  buffer_index++;

  if (buffer_index >= NBUFFERS)
    buffer_index = 0;

  memset(buffer[buffer_index], 0, sizeof(buffer[buffer_index]));

  if ((tp = strchr((char *)cp, ',')))
    {
    if ((tp - cp) > 128)
      return 0;
    /* Does this need to be null terminated? */
    strncpy(buffer[buffer_index], cp, tp - cp);
    }
  else
    {
    snprintf(buffer[buffer_index], 128, "%s", cp);
    }

  return(buffer[buffer_index]);
  }

char *csv_find_string(

  const char *csv_str,
  const char *search_str)

  {
  int  i;
  int  nitems;
  int  search_length = 0;
  char *cp;

  if (!search_str)
    return(NULL);

  search_length = strlen(search_str);

  nitems = csv_length(csv_str);

  for (i = 0; i < nitems; i++)
    {
    cp = csv_nth(csv_str, i);

    if (cp)
      {
      while (isspace(*cp))
        cp++;

      if ((int)strlen(cp) >= search_length &&
          !isalpha(cp[search_length]) &&
          !strncmp(cp, search_str, search_length))
        {
        return(cp);
        }
      }
    }

  return(NULL);
  }

int process_alps_status(

  const char               *nd_name,
  std::vector<std::string> &status_info)

  {
  return(PBSE_NONE);
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


/*
 *  * capture_until_close_character()
 *   */

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
  } // capture_until_close_character()


#define MAXLINE 1024

void add_range_to_string(

  std::string &range_string,
  int          begin,
  int          end)

  {
  char buf[MAXLINE];

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


void translate_vector_to_range_string(

  std::string            &range_string,
  const std::vector<int> &indices)

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
  } // END translate_vector_to_range_string()
  

int translate_range_string_to_vector(

  const char       *range_string,
  std::vector<int> &indices)

  {
  char *str = strdup(range_string);
  char *ptr = str;
  int   prev;
  int   curr;

  while (*ptr != '\0')
    {
    prev = strtol(ptr, &ptr, 10);

    if (*ptr == '-')
      {
      ptr++;
      curr = strtol(ptr, &ptr, 10);

      while (prev <= curr)
        {
        indices.push_back(prev);

        prev++;
        }

      if ((*ptr == ',') ||
          (is_whitespace(*ptr)))
        ptr++;
      }
    else
      {
      indices.push_back(prev);

      if ((*ptr == ',') ||
          (is_whitespace(*ptr)))
        ptr++;
      }
    }

  free(str);

  return(PBSE_NONE);
  } /* END translate_range_string_to_vector() */

bool task_hosts_match(const char *one, const char *two)
  {
  return(true);
  }

const char *pbsnode::get_name() const
  {
  return(this->nd_name.c_str());
  }

int pbsnode::lock_node(const char *caller, const char *msg, int level)
  {
  return(0);
  }

int pbsnode::unlock_node(const char *caller, const char *msg, int level)
  {
  return(0);
  }

void pbsnode::change_name(const char *hostname)
  {
  this->nd_name = hostname;
  }

void pbsnode::set_version(const char *) {}

#include "../../src/server/id_map.cpp"
#include "../../src/server/node_attr_def.c"
#include "../../src/lib/Libutils/machine.cpp"
#include "../../src/lib/Libutils/numa_chip.cpp"
#include "../../src/lib/Libutils/numa_socket.cpp"
#include "../../src/lib/Libutils/numa_core.cpp"
#include "../../src/lib/Libutils/numa_pci_device.cpp"
#include "../../src/lib/Libutils/allocation.cpp"
#include "../../src/lib/Libattr/req.cpp"
#include "../../src/lib/Libattr/complete_req.cpp"
//#include "../../src/lib/Libattr/attr_fn_str.c"
#ifdef MIC
int Chip::initializeMICDevices(hwloc_obj_t chip_obj, hwloc_topology_t topology)
  {
  return(0);
  }
#endif

#ifdef NVIDIA_GPUS
int Machine::initializeNVIDIADevices(hwloc_obj_t, hwloc_topology_t) {return(0);}
#endif

job::job() {}
