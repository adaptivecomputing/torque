#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <netdb.h> /* addrinfo */
#include <netinet/in.h>


#include "resizable_array.h" /* resizable_array */
#include "pbs_nodes.h" /* all_nodes, pbsnode */
#include "attribute.h" /* attribute_def, pbs_attribute, svrattrl */
#include "server.h" /* server */
#include "u_tree.h" /* AvlTree */
#include "hash_table.h" /* hash_table_t */
#include "list_link.h" /* list_link */
#include "work_task.h" /* work_task, work_type */
#include "tcp.h"
#include "pbs_job.h"


hello_container failures;
int svr_tsnodes = 0; 
resource_t next_resource_tag;
char server_name[PBS_MAXSERVERNAME + 1];
all_nodes allnodes;
char *path_nodes;
char *path_nodestate;
char *path_nodenote;
struct addrinfo hints;
char *path_nodes_new;
attribute_def node_attr_def[2];
int svr_clnodes = 0; 
struct server server;
AvlTree ipaddrs = NULL;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
int svr_totnodes = 0; 
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
dynamic_string *hierarchy_holder;
extern int cray_enabled;


int insert_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "This mock insert_thing always returns 0!!\n");
  return(0);
  }

svrattrl *attrlist_create(const char *aname, const char *rname, int vsize)
  {
  fprintf(stderr, "This mock  attrlist_create always returns NULL!!\n");
  return(NULL);
  }

int remove_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "This mock remove_thing always returns 0!!\n");
  return(0);
  }

AvlTree AVL_delete_node(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_delete_node needs to be mocked!!\n");
  exit(1);
  }

int add_hash(hash_table_t *ht, int value, void *key)
  {
  fprintf(stderr, "This mock add_hash always returns 0!!\n");
  return(0);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(work_task *), void *parm, int get_lock)
  {
  fprintf(stderr, "This mock set_task always returns NULL!!\n");
  return(NULL);
  }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
  {
  fprintf(stderr, "This mock find_attr always returns 0!!\n");
  return(0);
  }

int mgr_set_node_attr(struct pbsnode *pnode, attribute_def *pdef, int limit, svrattrl *plist, int privil, int *bad, void *parent, int mode)
  {
  fprintf(stderr, "This mock mgr_set_node_attr always returns 0!!\n");
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

int get_value_hash(hash_table_t *ht, const void *key)
  {
  char *str = (char *)key;

  if (str == NULL)
    return(-1);
  else if (!strcmp(str, "bob"))
    return(1);
  else if (!strcmp(str, "tom"))
    return(2);
  else
    return(-1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "This mock get_next always returns NULL!!\n");
  return(NULL);
  }

resizable_array *initialize_resizable_array(int size)
  {
  resizable_array *ra = (resizable_array *)calloc(1, sizeof(resizable_array));
  size_t           amount = sizeof(slot) * size;
  size = 10;

  ra->max       = size;
  ra->num       = 0;
  ra->next_slot = 1;
  ra->last      = 0;

  ra->slots = (slot *)calloc(1, amount);

  return(ra);
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

void *next_thing(resizable_array *ra, int *iter)
  {
  return(NULL);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

hash_table_t *create_hash(int size)
  {
  hash_table_t *ht = (hash_table_t *)calloc(1, sizeof(hash_table_t));
  size_t        amount = sizeof(bucket *) * size;

  ht->size = size;
  ht->num  = 0;
  ht->buckets = (bucket **)calloc(1, amount);

  return(ht);
  }

char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }

AvlTree AVL_insert(u_long key, uint16_t port, struct pbsnode *node, AvlTree tree)
  {
  fprintf(stderr, "This mock  AVL_insert is always returns 0!!\n");
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

void *pop_thing(resizable_array *ra)

  {
  return(NULL);
  }

int is_present(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to is_present needs to be mocked!!\n");
  exit(1);                    
  }


struct tcp_chan* DIS_tcp_setup(int sock)
  {
  fprintf(stderr, "This mock DIS_tcp_setup always returns NULL!!\n");
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


int tcp_connect_sockaddr(struct sockaddr *sa, size_t s)
  {
  fprintf(stderr, "This mock tcp_connect_sockaddr always returns 0!!\n");
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

int remove_thing_from_index(resizable_array *ra, int index)
  {
  fprintf(stderr, "This mock remove_thing_from_index always returns 0!!\n");
  return(0);
  }

int insert_thing_after(resizable_array *ra, void *thing, int index)
  {
  fprintf(stderr, "This mock insert_thing_after always returns 0!!\n");
  return(0);
  }

int get_addr_info(char *name, struct sockaddr_in *sa_info, int retry)
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

int append_dynamic_string(

  dynamic_string *ds,
  const char     *str)

  {
  return(0);
  }

dynamic_string *get_dynamic_string(int initial_size, const char *str)
  {
  dynamic_string *ds = (dynamic_string *)calloc(1, sizeof(dynamic_string));

  if (ds == NULL)
    return(ds);

  if (initial_size > 0)
    ds->size = initial_size;
  else
    ds->size = DS_INITIAL_SIZE;

  ds->str = (char *)calloc(1, ds->size);

  if (ds->str == NULL)
    {
    free(ds);
    return(NULL);
    }

  /* initialize empty str */
  ds->used = 0;

  /* add the string if it exists */
  if (str != NULL)
    {
    if (append_dynamic_string(ds,str) != PBSE_NONE)
      {
      free_dynamic_string(ds);
      return(NULL);
      }
    }

  return(ds);
  }


int login_node_count() 

  {
  return(0);
  }

struct sockaddr_in *get_cached_addrinfo(
    
  char               *hostname)

  {
  return(NULL);
  }

void free_dynamic_string(dynamic_string *ds) {}

job *get_job_from_jobinfo(
    
  struct jobinfo *jp,
  struct pbsnode *pnode)

  {
  return(NULL);
  }

int insert_addr_name_info(
    
  char               *hostname,
  char               *full_hostname,
  struct sockaddr_in *sai)

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
