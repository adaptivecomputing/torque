#ifndef _NODE_FUNC_H
#define _NODE_FUNC_H
#include "license_pbs.h" /* See here for the software license */
#include "list_link.h" /* tlist_head */
#include "net_connect.h"

/* Forward declarations. */
struct pbsnode;
struct node_check_info;
struct node_iterator;
struct all_nodes;
struct svrattrl;
struct pbs_attribute;
struct attribute_def;
struct hello_info;
struct hello_container;

typedef struct _node_info_
  {
  char     *nodename;
  struct svrattrl *plist;
  int      perms;
  tlist_head atrlist;
  } node_info;

struct pbsnode *PGetNodeFromAddr(pbs_net_t addr);

void bad_node_warning(pbs_net_t addr, struct pbsnode *);

int addr_ok(pbs_net_t addr, struct pbsnode *pnode);

struct pbsnode *find_nodebyname(const char *nodename);

struct pbssubn *find_subnodebyname(char *nodename);

void save_characteristic(struct pbsnode *pnode, struct node_check_info *nci);

int chk_characteristic(struct pbsnode *pnode, struct node_check_info *nci, int *pneed_todo);

int status_nodeattrib(struct svrattrl *pal, struct attribute_def *padef, struct pbsnode *pnode, int limit, int priv, tlist_head *phead, int *bad);

/* static void subnode_delete(struct pbssubn *psubn); */

void effective_node_delete(struct pbsnode **pnode);

/* static int process_host_name_part(char *objname, u_long **pul, char **pname, int *ntype); */

int update_nodes_file(struct pbsnode *held);

void recompute_ntype_cnts(void);

struct prop *init_prop(char *pname);

int create_a_gpusubnode(struct pbsnode *pnode);

int copy_properties(struct pbsnode *dest, struct pbsnode *src);

/*static int read_val_and_advance(int *val, char **str);*/

/*static int setup_node_boards(struct pbsnode *pnode, u_long *pul);*/

/*static void recheck_for_node(struct work_task *ptask);*/

int create_pbs_node(char *objname, struct svrattrl *plist, int perms, int *bad);

/* static char *parse_node_token(char *start, int cok, int comma, int *err, char *term); */

int setup_nodes(void);

/* static void delete_a_subnode(struct pbsnode *pnode); */

/* static void delete_a_gpusubnode(struct pbsnode *pnode); */

int node_np_action(struct pbs_attribute *new_attr, void *pobj, int actmode);

int node_mom_port_action(struct pbs_attribute *new_attr, void *pobj, int actmode);

int node_mom_rm_port_action(struct pbs_attribute *new_attr, void *pobj, int actmode);

int node_gpus_action(struct pbs_attribute *new_attr, void *pnode, int actmode);

int node_numa_action(struct pbs_attribute *new_attr, void *pnode, int actmode);

int numa_str_action(struct pbs_attribute *new_attr, void *pnode, int actmode);

int gpu_str_action(struct pbs_attribute *new_attr, void *pnode, int actmode);

int create_partial_pbs_node(char *nodename, unsigned long addr, int perms);

/*static node_iterator *get_node_iterator();*/

void reinitialize_node_iterator(struct node_iterator *iter);

/*static struct pbsnode *get_my_next_node_board(node_iterator *iter, struct pbsnode *np);*/

struct pbsnode *next_node(struct all_nodes *an, struct pbsnode *current, struct node_iterator *iter);

void initialize_all_nodes_array(struct all_nodes *an);

int insert_node(struct all_nodes *an, struct pbsnode *pnode);

int remove_node(struct all_nodes *an, struct pbsnode *pnode);

struct pbsnode *next_host(struct all_nodes *an, int *iter, struct pbsnode *held);

void *send_hierarchy_threadtask(void *vp);

int send_hierarchy(char *name, unsigned short  port);

struct hello_container* initialize_hello_container(struct hello_container *);

int needs_hello(struct hello_container *hc, char *node_name);

int add_hello(struct hello_container *hc, char *node_name);

int add_hello_after(struct hello_container *hc, char *node_name, int index);

int add_hello_info(struct hello_container *hc, struct hello_info *hi);

struct hello_info *pop_hello(struct hello_container *hc);

int remove_hello(struct hello_container *hc, char *node_name);

#endif /* _NODE_FUNC_H */
