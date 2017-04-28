#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <netdb.h> /* addrinfo */
#include <netinet/in.h>


#include "pbs_nodes.h" /* all_nodes, pbsnode */
#include "attribute.h" /* attribute_def, pbs_attribute, svrattrl */
#include "server.h" /* server */
#include "u_tree.h" /* AvlTree */
#include "list_link.h" /* list_link */
#include "work_task.h" /* work_task, work_type */
#include "batch_request.h" /* batch_request */
#include "tcp.h"
#include "pbs_job.h"
#include <string>
#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>
#include "id_map.hpp"
#include "threadpool.h"
#include "mom_hierarchy_handler.h"
#include "machine.hpp"


std::string attrname;
std::string attrval;
int         created_subnode;

int svr_tsnodes = 0; 
int svr_unresolvednodes = 0;
resource_t next_resource_tag;
char server_name[PBS_MAXSERVERNAME + 1];
bool exit_called = false;
all_nodes allnodes;
char *path_nodes;
char *path_nodestate;
char *path_nodepowerstate;
char *path_nodenote;
struct addrinfo hints;
char *path_nodes_new;
attribute_def node_attr_def[2];
int svr_clnodes = 0; 
struct server server;
AvlTree ipaddrs = NULL;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
int svr_totnodes = 0; 
threadpool_t *task_pool = NULL;
const char *dis_emsg[] = {"No error",
  "Input value too large to convert to this type",
  "Tried to write floating point infinity",
  "Negative sign on an unsigned datum",
  "Input count or value has leading zero",
  "Non-digit found where a digit was expected",
  "Input string has an embedded ASCII NUL",
  "Premature end of message",
  "Unable to malloc enough space for string",
  "Supporting protocol failure",
  "Protocol failure in commit",
  "End of File"
};
boost::ptr_vector<std::string> hierarchy_holder;
pthread_mutex_t                 hierarchy_holder_Mutex = PTHREAD_MUTEX_INITIALIZER;
extern int cray_enabled;

mom_hierarchy_handler hierarchy_handler; //The global declaration.


svrattrl *s = NULL;

void attrlist_free()
  {
  if (s != NULL)
    {
    free(s);
    s = NULL;
    }
  }

svrattrl *attrlist_create(
  const char *aname, 
  const char *rname, 
  int vsize)

  {
  size_t    asz;
  size_t    rsz;

  asz = strlen(aname) + 1;     /* pbs_attribute name,allow for null term */

  if (rname == NULL)      /* resource name only if type resource */
    rsz = 0;
  else
    rsz = strlen(rname) + 1;

  s = attrlist_alloc(asz, rsz, vsize);

  strcpy(s->al_name, aname); /* copy name right after struct */

  if (rsz > 0)
    strcpy(s->al_resc, rname);

  return(s);
}

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  svrattrl *pal = (svrattrl *)pobj;
  attrname = pal->al_name;
  attrval = pal->al_value;
  }


AvlTree AVL_delete_node(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_delete_node needs to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(work_task *), void *parm, int get_lock)
  {
  return(NULL);
  }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
  {
  return(0);
  }

int mgr_set_node_attr(struct pbsnode *pnode, attribute_def *pdef, int limit, svrattrl *plist, int privil, int *bad, void *parent, int mode, bool dont_update)
  {
  return(0);
  }

void free_prop_list(struct prop *prop)
  {
  struct prop *pp;
  while (prop)
    {
    pp = prop->next;
    free(prop);
    prop = pp;
    }
  }

void *get_next(list_link pl, char *file, int line)
  {
  return(NULL);
  }

struct pbsnode *AVL_find(u_long key, uint16_t port, AvlTree tree)
  {
  static struct pbsnode numa;
  static char           buf[20];

  if (key > 2)
    return(NULL);
  else
    {
    sprintf(buf, "%lu", key);
    numa.nd_name = buf;
    return(&numa);
    }
  }

void free_attrlist(tlist_head *pattrlisthead)
  {
  fprintf(stderr, "The call to free_attrlist needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  {
  return(NULL);
  }

AvlTree AVL_insert(u_long key, uint16_t port, struct pbsnode *node, AvlTree tree)
  {
  return(NULL);
  }

int unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }                           

int lock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  { 
  return(0); 
  }                           

int tmp_unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }                           

int tmp_lock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  { 
  return(0); 
  }

struct tcp_chan* DIS_tcp_setup(int sock)
  {
  return(NULL);
  }



int DIS_tcp_wflush(tcp_chan *c)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);                    
  }


int diswcs (tcp_chan* c, char const* a, unsigned long l)
  {
  fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
  exit(1);                    
  }


int tcp_connect_sockaddr(struct sockaddr *sa, size_t s, bool use_log)
  {
  return(0);
  }

int diswsi(int sock, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);                    
  }

int get_svr_attr_l(int index, long *l)
  {
  if (index == SRV_ATR_CrayEnabled)
    *l = cray_enabled;

  return(0);
  }

int add_conn(int sock, enum conn_type type, pbs_net_t addr, unsigned int port, unsigned int socktype, void *(*func)(void *))
  {
  return(0);
  }

void close_conn(int sd, int has_mutex)
  {
  }

int get_svr_attr_str(int index, char **str)
  {
  return(0);
  }

int is_compose(

  struct tcp_chan *chan,
  int   command)

  {
  return(0);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan)
  {
  }

int add_to_login_holder(

  struct pbsnode *pnode)

  {
  return(0);
  }


int login_node_count() 

  {
  return(0);
  }

struct sockaddr_in *get_cached_addrinfo(
    
  const char *hostname)

  {
  return(NULL);
  }

job *get_job_from_jobinfo(
    
  struct jobinfo *jp,
  struct pbsnode *pnode)

  {
  return(NULL);
  }

int insert_addr_name_info(
    struct addrinfo *,
    const char *)

  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

const char *alps_reporter_feature  = "alps_reporter";
const char *alps_starter_feature   = "alps_starter";

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

struct addrinfo dummyAddrInfo;
struct sockaddr dummySockAddr;

int pbs_getaddrinfo(const char *,struct addrinfo *,struct addrinfo **ai)
  {
  memset(&dummyAddrInfo,0,sizeof(struct addrinfo));
  memset(&dummySockAddr,0,sizeof(struct sockaddr));
  dummyAddrInfo.ai_addr = &dummySockAddr;
  *ai = &dummyAddrInfo;
  return 0;
  }

bool overwrite_cache(const char *,struct addrinfo **ai)
  {
  memset(&dummyAddrInfo,0,sizeof(struct addrinfo));
  memset(&dummySockAddr,0,sizeof(struct sockaddr));
  dummyAddrInfo.ai_addr = &dummySockAddr;
  *ai = &dummyAddrInfo;
  return true;
  }

job *get_job_from_job_usage_info(job_usage_info *jui, struct pbsnode *pnode)
  {
  fprintf(stderr, "The call to get_job_from_job_usage_info needs to be mocked!!\n");
  exit(1);
  }

struct pbsnode *create_alps_subnode(struct pbsnode *parent, const char *node_id)
  {
  created_subnode++;
  return(NULL);
  }

id_map::id_map() : counter(0) {}

int id_map::get_id(char const *name)
  {
  if (name == NULL)
    return(-1);
  else if (!strcmp(name, "bob"))
    return(1);
  else if (!strcmp(name, "tom"))
    return(2);
  else
    return(-1);
  }

const char *id_map::get_name(int id)
  {
  switch (id)
    {
    case 0:
    return("one");

    case 1:
    return("two");

    case 2:
    return("three");

    case 3:
    return("four");

    case 4:
    return("five");

    default:
    return(NULL);
    } 
  }

int id_map::get_new_id(char const *name)
  {
  static int id = 0;
  return(id++);
  }

id_map::~id_map() 
  {
  }


id_map node_mapper;
id_map job_mapper;

struct pbsnode *tfind_addr(

  const u_long  key,
  uint16_t      port,
  char         *job_momname)

  {
  return(NULL);
  }
int issue_Drequest(
  int                    conn,
  struct batch_request  *request)
  {
  fprintf(stderr,"%s needs to be mocked.\n",__func__);
  exit(-1);
  return 0;
  }

int svr_connect(
  pbs_net_t        hostaddr,  /* host order */
  unsigned int     port,   /* I */
  int             *my_err,
  struct pbsnode  *pnode,
  void           *(*func)(void *))
  {
  fprintf(stderr,"%s needs to be mocked.\n",__func__);
  exit(-1);
  return 0;
  }

int enqueue_threadpool_request(

  void *(*func)(void *),
  void *arg,
  threadpool_t *tp)

  {
  return(PBSE_NONE);
  }

struct batch_request *alloc_br(

  int type)

  {

  struct batch_request *req = NULL;

  if ((req = (struct batch_request *)calloc(1, sizeof(struct batch_request))) == NULL)
    {
    fprintf(stderr, "failed to allocate batch request. alloc_br()\n");
    }
  else
    {

    req->rq_type = type;

    req->rq_conn = -1;  /* indicate not connected */
    req->rq_orgconn = -1;  /* indicate not connected */
    req->rq_time = time(NULL);
    req->rq_reply.brp_choice = BATCH_REPLY_CHOICE_NULL;
    req->rq_noreply = FALSE;  /* indicate reply is needed */
    }

  return(req);
  } /* END alloc_br() */

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


void free_br(struct batch_request *preq)
  {
  return;
  }

void mom_hierarchy_handler::reloadHierarchy()
  {
  }

int Machine::getDedicatedSockets() const
  {
  return(0);
  }

int Machine::getTotalThreads() const
  {
  return(0);
  }

int Machine::getDedicatedChips() const
  {
  return(0);
  }

int Machine::getTotalCores() const
  {
  return(0);
  }

int Machine::getDedicatedCores() const
  {
  return(0);
  }

int Machine::getDedicatedThreads() const
  {
  return(0);
  }

int Machine::getTotalSockets() const
  {
  return(0);
  }

int Machine::getTotalChips() const
  {
  return(0);
  }

struct pbsnode *get_compute_node(

  const char *node_name)

  {
  return(NULL);
  }
