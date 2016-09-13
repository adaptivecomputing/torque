#include <stdlib.h>
#include <stdio.h>

#include "license_pbs.h" /* See here for the software license */
#include "mom_hierarchy.h"
#include "pbs_nodes.h"
#include "threadpool.h"
#include "execution_slot_tracker.hpp"
#include "id_map.hpp"

all_nodes                allnodes;
char                    *path_mom_hierarchy = NULL;
threadpool_t             *async_pool = NULL;
extern mom_hierarchy_t  *mh;
int                     LOGLEVEL;
bool                    exit_called = false;
threadpool_t           *task_pool;
id_map                  node_mapper;
int			lock_node_count = 0;

execution_slot_tracker::execution_slot_tracker(){}

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


void free_mom_hierarchy(

  mom_hierarchy_t *mh)

  {
  mh->paths.clear();
  free(mh);
  } /* END free_mom_hierarchy() */

mom_hierarchy_t *initialize_mom_hierarchy()

  {
  mom_hierarchy_t *nt = (mom_hierarchy_t *)calloc(1, sizeof(mom_hierarchy_t));

  nt->current_path  = -1;
  nt->current_level = -1;
  nt->current_node  = -1;

  return(nt);
  } /* END initialize_mom_hierarchy() */

struct sockaddr_in dummy_ai_addr = {0,0,{0},{0,0,0,0,0,0,0,0}};
struct addrinfo dummy_ai;

int pbs_getaddrinfo(const char *pNode, struct addrinfo *pHints, struct addrinfo **ppAddrInfoOut)
  {
  dummy_ai.ai_addr = (sockaddr *)&dummy_ai_addr;
  *ppAddrInfoOut = &dummy_ai;
  return(0);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

void log_err(int errnum, const char *routine, const char *text) {}

int tcp_connect_sockaddr(struct sockaddr *sa, size_t s)
  {
  return(0);
  }

int add_conn(int sock, enum conn_type type, pbs_net_t addr, unsigned int port, unsigned int socktype, void *(*func)(void *))
  {
  return(0);
  }

int lock_node(
  struct pbsnode *the_node,
  const char     *id,
  const char     *msg,
  int             logging)
  {
  lock_node_count++;
  return(0);
  }

int unlock_node(
  struct pbsnode *the_node,
  const char     *id,
  const char     *msg,
  int             logging)
  {
  return(0);
  }

int nodeId = 0;
int create_partial_pbs_node(char *nodename, unsigned long addr, int perms)
  {
  pbsnode *pnode = (pbsnode *)calloc(1,sizeof(pbsnode));
  pnode->nd_name = strdup(nodename);
  pnode->nd_id = nodeId++;
  pnode->nd_hierarchy_level = 500;
  allnodes.insert(pnode,nodename,true);
  return(0);
  }

void parse_mom_hierarchy(int fds)
  {
  for(int path = 0;path < 3;path++)
    {
    mom_levels ml;
    for(int level = 0;level < 5;level++)
      {
      mom_nodes   mn;
      for(int node = 0;node < 4;node++)
        {
        node_comm_t nc;
        char       name[30];
        memset(&nc,0,sizeof(nc));

        sprintf(name,"node_%d_%d_%d",path,level,node);
        nc.name = strdup(name);
        mn.push_back(nc);
        }
      ml.push_back(mn);
      }
    mh->paths.push_back(ml);
    }
  }

struct pbsnode *next_host(

  all_nodes           *an,    /* I */
  all_nodes_iterator **iter,  /* M */
  struct pbsnode      *held)  /* I */

  {
  struct pbsnode *pnode;

  an->lock();
  if (*iter == NULL)
    {
    *iter = an->get_iterator();
    }

  pnode = (*iter)->get_next_item();

  an->unlock();

  return(pnode);
  } /* END next_host() */

struct pbsnode *find_nodebyid(
  int node_id)
  {
  allnodes.lock();
  all_nodes_iterator *it = allnodes.get_iterator();
  pbsnode *pNode;
  while((pNode = it->get_next_item()) != NULL)
    {
    if(pNode->nd_id == node_id)
      {
      break;
      }
    }
  delete it;
  allnodes.unlock();
  return(pNode);
  } /* END find_nodebyid() */

struct pbsnode *find_nodebyname(
  const char *nodename) /* I */
  {
  allnodes.lock();

  pbsnode *pnode = allnodes.find(nodename);

  allnodes.unlock();

  return pnode;
  }

int enqueue_threadpool_request(

  void *(*func)(void *),
  void *arg,
  threadpool_t *tp)

  {
  return(0);
  }

int is_compose(

  struct tcp_chan *chan,
  int   command)

  {
  return(0);
  }

int xml_index = 0;
const char *xml_out[] =
{
"<sp>",
"<sl>",
"node_0_0_0:0,node_0_0_1:0,node_0_0_2:0,node_0_0_3:0",
"</sl>",
"<sl>",
"node_0_1_0:0,node_0_1_1:0,node_0_1_2:0,node_0_1_3:0",
"</sl>",
"<sl>",
"node_0_2_0:0,node_0_2_1:0,node_0_2_2:0,node_0_2_3:0",
"</sl>",
"<sl>",
"node_0_3_0:0,node_0_3_1:0,node_0_3_2:0,node_0_3_3:0",
"</sl>",
"<sl>",
"node_0_4_0:0,node_0_4_1:0,node_0_4_2:0,node_0_4_3:0",
"</sl>",
"</sp>",
"<sp>",
"<sl>",
"node_1_0_0:0,node_1_0_1:0,node_1_0_2:0,node_1_0_3:0",
"</sl>",
"<sl>",
"node_1_1_0:0,node_1_1_1:0,node_1_1_2:0,node_1_1_3:0",
"</sl>",
"<sl>",
"node_1_2_0:0,node_1_2_1:0,node_1_2_2:0,node_1_2_3:0",
"</sl>",
"<sl>",
"node_1_3_0:0,node_1_3_1:0,node_1_3_2:0,node_1_3_3:0",
"</sl>",
"<sl>",
"node_1_4_0:0,node_1_4_1:0,node_1_4_2:0,node_1_4_3:0",
"</sl>",
"</sp>",
"<sp>",
"<sl>",
"node_2_0_0:0,node_2_0_1:0,node_2_0_2:0,node_2_0_3:0",
"</sl>",
"<sl>",
"node_2_1_0:0,node_2_1_1:0,node_2_1_2:0,node_2_1_3:0",
"</sl>",
"<sl>",
"node_2_2_0:0,node_2_2_1:0,node_2_2_2:0,node_2_2_3:0",
"</sl>",
"<sl>",
"node_2_3_0:0,node_2_3_1:0,node_2_3_2:0,node_2_3_3:0",
"</sl>",
"<sl>",
"node_2_4_0:0,node_2_4_1:0,node_2_4_2:0,node_2_4_3:0",
"</sl>",
"</sp>",
"END_OF_LINE"
};

int diswcs(
  struct tcp_chan *chan,
  const char      *value,
  size_t           nchars)
  {
  //fprintf(stderr,"%s\n",value);
  if(value == NULL) return 0;
  if(strcmp(value,xml_out[xml_index]))
    {
    fprintf(stderr,"Unexpected xml in hierarchy: %s expected, %s received.",xml_out[xml_index],value);
    exit(1);
    }
  xml_index++;
  return 0;
  }

int DIS_tcp_cleanup(struct tcp_chan *chan)
  {
  return 0;
  }

int DIS_tcp_wflush(struct tcp_chan *chan)
  {
  return 0;
  }

int DIS_tcp_setup(int)
  {
  return 0;
  }

void close_conn(int,int){}

int tcp_connect_sockaddr(

  struct sockaddr *sa,      /* I */
  size_t           sa_size, /* I */
  bool             use_log) /* I */

  {
  return(0);
  }

id_map::id_map() {}
id_map::~id_map() {}

const char *id_map::get_name(int id) 

  {
  return(NULL);
  }

int insert_node(all_nodes *an, struct pbsnode *pnode)
  {
  if ((an == NULL) || (pnode == NULL) || (pnode->nd_name == NULL)) return 0;

  an->lock();
  an->insert(pnode, pnode->nd_name);
  an->unlock();
  return(PBSE_NONE);
  }

int remove_node(all_nodes *an, struct pbsnode *pnode)
  {
  if ((an == NULL) || (pnode == NULL) || (pnode->nd_name == NULL)) return 0;

  an->lock();
  an->remove(pnode->nd_name);
  an->unlock();
  return(PBSE_NONE);
  }
