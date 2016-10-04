#include "pbs_nodes.h"
#include "id_map.hpp"
#include "u_tree.h"
#include "mom_hierarchy_handler.h"

#include <pbs_config.h>

std::map<std::string, std::string> encoded;

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


svrattrl *attrlist_create(

  const char  *aname, /* I - pbs_attribute name */
  const char  *rname, /* I - resource name if needed or null */
  int           vsize) /* I - size of resource value         */

  {
  svrattrl *pal;
  size_t    asz;
  size_t    rsz;

  asz = strlen(aname) + 1;     /* pbs_attribute name,allow for null term */

  if (rname == NULL)      /* resource name only if type resource */
    rsz = 0;
  else
    rsz = strlen(rname) + 1;

  pal = attrlist_alloc(asz, rsz, vsize);

  strcpy(pal->al_name, aname); /* copy name right after struct */

  if (rsz > 0)
    strcpy(pal->al_resc, rname);

  return(pal);
  }

void append_link(

  tlist_head *head, /* ptr to head of list */
  list_link  *new_link,  /* ptr to new entry */
  void       *pobj) /* ptr to object to link in */

  {
  svrattrl *pal = (svrattrl *)pobj;
  encoded[pal->al_name] = pal->al_value;
  }

int decode_arst(

  pbs_attribute *patr,    /* O (modified) */
  const char *  UNUSED(name),    /* I pbs_attribute name (notused) */
  const char *  UNUSED(rescn),   /* I resource name (notused) */
  const char    *val,     /* I pbs_attribute value */
  int           UNUSED(perm)) /* only used for resources */

  {
  return(0);
  }

int set_arst(

  pbs_attribute *attr,  /* I/O */
  pbs_attribute *new_attr,   /* I */
  enum batch_op     op)    /* I */

  {
  return(0);
  }

void free_arst(pbs_attribute *attr) {}

int node_status_list(

  pbs_attribute *new_attr,           /*derive status into this pbs_attribute*/
  void          *pnode,         /*pointer to a pbsnode struct     */
  int            actmode)       /*action mode; "NEW" or "ALTER"   */

  {
  return(0);
  }
