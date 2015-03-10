#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "u_tree.h"
#include "dynamic_string.h"
#include "tcp.h"
#include "pbs_job.h"
#include "mutex_mgr.hpp"
#include "threadpool.h"
#include "execution_slot_tracker.hpp"
#include "execution_slot_tracker.hpp"
#include "mom_hierarchy_handler.h"


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

dynamic_string *get_dynamic_string(
    
  int         initial_size, /* I (-1 means default) */
  const char *str)          /* I (optional) */

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

void free_dynamic_string(dynamic_string *ds) {}

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

int copy_to_end_of_dynamic_string(dynamic_string *ds, const char *to_copy) 
  {
  return(0);
  }

int process_status_info(

  char                     *nd_name,
  std::vector<std::string> &status_info)

  {
  return(0);
  }

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
