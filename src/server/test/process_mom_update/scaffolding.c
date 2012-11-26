#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "attribute.h"
#include "pbs_nodes.h"
#include "pbs_job.h"
#include "u_tree.h"

char        server_name[PBS_MAXSERVERNAME + 1]; /* host_name[:service|port] */
int         allow_any_mom;
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

attribute_def node_attr_def[1];



void log_record(int eventtype, int objclass, const char *objname, char *text) {}
void log_event(int eventtype, int objclass, const char *objname, char *text) {}
void log_err(int errnum, const char *routine, char *text) {}
void close_conn(int sd, int has_mutex) {}
hello_container         hellos;

char *threadsafe_tokenizer(

  char **str,    /* M */
  char  *delims) /* I */

  {
  return(NULL);
  }

int remove_hello(

  hello_container *hc,
  char            *node_name)

  {
  return(0);
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
  char       *msg,
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

int enqueue_threadpool_request(

  void *(*func)(void *),
  void *arg)

  {
  return(0);
  }

int lock_node(
    
  struct pbsnode *the_node,
  const char     *id,
  char           *msg,
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

  char *nodename) /* I */

  {
  return(NULL);
  }

int unlock_node(
    
  struct pbsnode *the_node,
  const char     *id,
  char           *msg,
  int             logging)

  {
  return(0);
  }

job *svr_find_job(char *jobid, int subjob)
  
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
  char           *jobid) /* I */

  {
  return(0);
  }

int node_status_list(

  pbs_attribute *new,           /*derive status into this pbs_attribute*/
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
  char          *name,    /* I pbs_attribute name (notused) */
  char          *rescn,   /* I resource name (notused) */
  char          *val,     /* I pbs_attribute value */
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

int append_dynamic_string(

  dynamic_string *ds,
  const char     *str)

  {
  return(0);
  }

dynamic_string *get_dynamic_string(
    
  int         initial_size, /* I (-1 means default) */
  const char *str)          /* I (optional) */

  {
  return(NULL);
  }

int node_micstatus_list(

  pbs_attribute *new,      /* derive status into this pbs_attribute*/
  void          *pnode,    /* pointer to a pbsnode struct     */
  int            actmode)  /* action mode; "NEW" or "ALTER"   */

  {
  return(0);
  }

int append_char_to_dynamic_string(
    
  dynamic_string *ds,
  char            c)

  {
  return(0);
  }

void clear_dynamic_string(dynamic_string *ds) {}
