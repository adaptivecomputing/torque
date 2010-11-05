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
int AVL_list( AvlTree tree, char *Buf, unsigned long BufSize );
int AVL_is_in_tree(u_long key, uint16_t port, AvlTree tree);


#endif /* __U_TREE_H__*/
