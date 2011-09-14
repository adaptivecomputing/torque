#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <netdb.h> /* addrinfo */


#include "resizable_array.h" /* resizable_array */
#include "pbs_nodes.h" /* all_nodes, pbsnode */
#include "attribute.h" /* attribute_def, attribute, svrattrl */
#include "server.h" /* server */
#include "u_tree.h" /* AvlTree */
#include "hash_table.h" /* hash_table_t */
#include "list_link.h" /* list_link */
#include "work_task.h" /* work_task, work_type */

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
int LOGLEVEL = 0;
int svr_totnodes = 0; 


int insert_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to insert_thing needs to be mocked!!\n");
  exit(1);
  }

svrattrl *attrlist_create(char *aname, char *rname, int vsize)
  {
  fprintf(stderr, "The call to attrlist_create needs to be mocked!!\n");
  exit(1);
  }

int remove_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to remove_thing needs to be mocked!!\n");
  exit(1);
  }

AvlTree AVL_delete_node(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_delete_node needs to be mocked!!\n");
  exit(1);
  }

int add_hash(hash_table_t *ht, int value, char *key)
  {
  fprintf(stderr, "The call to add_hash needs to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_record needs to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task needs to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr needs to be mocked!!\n");
  exit(1);
  }

int mgr_set_node_attr(struct pbsnode *pnode, attribute_def *pdef, int limit, svrattrl *plist, int privil, int *bad, void *parent, int mode)
  {
  fprintf(stderr, "The call to mgr_set_node_attr needs to be mocked!!\n");
  exit(1);
  }

void free_prop_list(struct prop *prop)
  {
  fprintf(stderr, "The call to free_prop_list needs to be mocked!!\n");
  exit(1);
  }

int get_value_hash(hash_table_t *ht, char *key)
  {
  fprintf(stderr, "The call to get_value_hash needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

resizable_array *initialize_resizable_array(int size)
  {
  fprintf(stderr, "The call to initialize_resizable_array needs to be mocked!!\n");
  exit(1);
  }

struct pbsnode *AVL_find(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_find needs to be mocked!!\n");
  exit(1);
  }

void free_attrlist(tlist_head *pattrlisthead)
  {
  fprintf(stderr, "The call to free_attrlist needs to be mocked!!\n");
  exit(1);
  }

void *next_thing(resizable_array *ra, int *iter)
  {
  fprintf(stderr, "The call to next_thing needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

hash_table_t *create_hash(int size)
  {
  fprintf(stderr, "The call to create_hash needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }

AvlTree AVL_insert(u_long key, uint16_t port, struct pbsnode *node, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_insert needs to be mocked!!\n");
  exit(1);
  }
