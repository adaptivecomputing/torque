#ifndef __U_TREE_H__
#define __U_TREE_H__

#include <stdlib.h>
#include <pbs_nodes.h>


struct AvlNode
  {
  u_long	 	  key;
  unsigned short  port;
  struct pbsnode *pbsnode;
  struct AvlNode  *left;
  struct AvlNode  *right;
  int			  height;
  };

typedef struct AvlNode *AvlTree;
typedef struct AvlNode *NodeEntry;

/* u_tree prototypes */
AvlTree AVL_delete_node( u_long key, uint16_t port, AvlTree tree);
struct pbsnode *AVL_find(u_long key, uint16_t port, AvlTree tree);
AvlTree AVL_insert( u_long key, uint16_t port, struct pbsnode *node, AvlTree tree );
int AVL_list_add_item(AvlTree tree, char **Buf, long *current_len, long *max_len);
int AVL_list(AvlTree tree, char **Buf, long *current_len, long *max_len);
int AVL_is_in_tree(u_long key, uint16_t port, AvlTree tree);
int AVL_is_in_tree_no_port_compare(u_long key, uint16_t port, AvlTree tree);
uint16_t AVL_get_port_by_ipaddr(u_long key, AvlTree tree);


#endif /* __U_TREE_H__*/
