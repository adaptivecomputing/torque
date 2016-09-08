#include "pbs_nodes.h"
#include "id_map.hpp"
#include "u_tree.h"
#include "mom_hierarchy_handler.h"

#include <pbs_config.h>

extern std::string global_string;

AvlTree                 ipaddrs = NULL;
int                     LOGLEVEL = 10;
id_map                  node_mapper;
mom_hierarchy_handler   hierarchy_handler; //The global declaration.
bool                    exit_called;
bool                    cray_enabled;

int pbs_getaddrinfo(
    
  const char       *pNode,
  struct addrinfo  *pHints,
  struct addrinfo **ppAddrInfoOut)

  {
  return(0);
  }

void log_record(

  int         eventtype,  /* I */
  int         objclass,   /* I */
  const char *objname,    /* I */
  const char *text)       /* I */

  {
  }

int id_map::get_new_id(const char *nodename)
  {
  return(0);
  }

struct prop *init_prop(

  const char *pname) /* I */

  {
  return(NULL);
  }

void free_prop_list(struct prop *prop)
  {
  }

struct pbsnode *next_host(

  all_nodes           *an,    /* I */
  all_nodes_iterator **iter,  /* M */
  struct pbsnode      *held)  /* I */

  {
  return(NULL);
  }

AvlTree AVL_delete_node(
  u_long   key,
  uint16_t port,
  AvlTree  tree)

  {
  return(NULL);
  }

void log_err(

  int         errnum,  /* I (errno or PBSErrno) */
  const char *routine, /* I */
  const char *text)    /* I */

  {
  }

id_map::id_map() {}
id_map::~id_map() {}

job::job() 
  {
  memset(this->ji_wattr, 0, sizeof(this->ji_wattr));
  }

job::~job() {}

void update_node_state(pbsnode *pnode, int newstate)
  {
  pnode->nd_state = newstate;
  }

void add_to_property_list(std::string &propstr, const char *token)
  {
  if (propstr.length() != 0)
    propstr += ",";

  propstr += token;
  }

int record_node_property_list(std::string const &propstr, tlist_head *atrlist_ptr)
  {
  if ((global_string.length() > 0) && (propstr != global_string))
    {
    // no match, return not ok
    return(1);
    }

  // return ok
  return(0);
  }

#ifdef PENABLE_LINUX_CGROUPS
#include "machine.hpp"
#include "allocation.hpp"

PCI_Device::PCI_Device(PCI_Device const &other) {}
PCI_Device::~PCI_Device() {}

Machine &Machine::operator =(const Machine &other)
  {
  return(*this);
  }

Machine::~Machine() {}
Machine::Machine() {}

Chip::Chip(const Chip &other) {}
Chip::~Chip() {}

allocation::allocation(const allocation &other) {}

Socket::~Socket() {}
Socket::Socket() {}

Core::Core() {}
Core::~Core() {}

#endif

pbsnode *alps_reporter;
const char *alps_reporter_feature  = "alps_reporter";
const char *alps_starter_feature   = "alps_login";
