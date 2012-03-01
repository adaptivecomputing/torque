#ifndef _NODE_FUNC_H
#define _NODE_FUNC_H
#include "license_pbs.h" /* See here for the software license */

#include "server_limits.h" /* pbs_net_t. Also defined in net_connect.h */
#include "pbs_nodes.h" /* pbs_nodes, node_check_info, node_iterator, all_nodes */
#include "attribute.h" /* svrattrl, pbs_attribute */
#include "list_link.h" /* tlist_head */
#include "work_task.h" /* work_task */

typedef struct _node_info_
  {
  char     *nodename;
  svrattrl *plist;
  int      perms;
  tlist_head atrlist;
  } node_info;

struct pbsnode *PGetNodeFromAddr(pbs_net_t addr);

void bad_node_warning(pbs_net_t addr, struct pbsnode *);

int addr_ok(pbs_net_t addr, struct pbsnode *pnode);

struct pbsnode *find_nodebyname(char *nodename);

struct pbssubn *find_subnodebyname(char *nodename);

void save_characteristic(struct pbsnode *pnode, node_check_info *nci);

int chk_characteristic(struct pbsnode *pnode, node_check_info *nci, int *pneed_todo);

int status_nodeattrib(svrattrl *pal, attribute_def *padef, struct pbsnode *pnode, int limit, int priv, tlist_head *phead, int *bad);

/* static void subnode_delete(struct pbssubn *psubn); */

void effective_node_delete(struct pbsnode *pnode);

/* static int process_host_name_part(char *objname, u_long **pul, char **pname, int *ntype); */

int update_nodes_file(struct pbsnode *held);

void recompute_ntype_cnts(void);

struct prop *init_prop(char *pname);

int create_a_gpusubnode(struct pbsnode *pnode);

int copy_properties(struct pbsnode *dest, struct pbsnode *src);

int read_val_and_advance(int *val, char **str);

int setup_node_boards(struct pbsnode *pnode, u_long *pul);

void recheck_for_node(struct work_task *ptask);

int create_pbs_node(char *objname, svrattrl *plist, int perms, int *bad);

/* static char *parse_node_token(char *start, int cok, int comma, int *err, char *term); */

int setup_nodes(void);

/* static void delete_a_subnode(struct pbsnode *pnode); */

/* static void delete_a_gpusubnode(struct pbsnode *pnode); */

int node_np_action(pbs_attribute *new, void *pobj, int actmode);

int node_mom_port_action(pbs_attribute *new, void *pobj, int actmode);

int node_mom_rm_port_action(pbs_attribute *new, void *pobj, int actmode);

int node_gpus_action(pbs_attribute *new, void *pnode, int actmode);

int node_numa_action(pbs_attribute *new, void *pnode, int actmode);

int numa_str_action(pbs_attribute *new, void *pnode, int actmode);

int gpu_str_action(pbs_attribute *new, void *pnode, int actmode);

int create_partial_pbs_node(char *nodename, unsigned long addr, int perms);

node_iterator *get_node_iterator();

void reinitialize_node_iterator(node_iterator *iter);

struct pbsnode *get_my_next_node_board(node_iterator *iter, struct pbsnode *np);

struct pbsnode *next_node(all_nodes *an, struct pbsnode *current, node_iterator *iter);

void initialize_all_nodes_array(all_nodes *an);

int insert_node(all_nodes *an, struct pbsnode *pnode);

int remove_node(all_nodes *an, struct pbsnode *pnode);

struct pbsnode *next_host(all_nodes *an, int *iter, struct pbsnode *held);

#endif /* _NODE_FUNC_H */
