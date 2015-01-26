#include "utils.h"
#include "u_tree.h"
#include <pthread.h>


static pthread_mutex_t AVLMutex = PTHREAD_MUTEX_INITIALIZER;


/* height - returns the height of the given node */
static int height(NodeEntry n)
  {
  if (n == NULL)
    return(-1);
  else
    return(n->height);
  }

static int Max( int right_side, int left_side )
  {
  return( left_side > right_side ? left_side : right_side );
  }


/* This function can be called only if K2 has a left child */
/* Perform a rotate between a node (K2) and its left child */
/* Update heights, then return new root */

static NodeEntry single_rotate_with_left( NodeEntry K2 )
  {
  NodeEntry K1;

  if ((K2 == NULL) ||
      (K2->left == NULL))
    return(K2);
  
  K1 = K2->left;
  K2->left = K1->right;
  K1->right = K2;
  
  K2->height = Max(height(K2->left), height(K2->right)) + 1;
  K1->height = Max(height(K1->left), height(K1->right)) + 1;
  
  return K1;  /* New root */
  }

/* This function can be called only if K1 has a right child */
/* Perform a rotate between a node (K1) and its right child */
/* Update heights, then return new root */

static NodeEntry single_rotate_with_right( NodeEntry K1 )
  {
  NodeEntry K2;
  
  K2 = K1->right;
  K1->right = K2->left;
  K2->left = K1;
  
  K1->height = Max(height(K1->left), height(K1->right)) + 1;
  K2->height = Max(height(K2->left), height(K2->right)) + 1;
  
  return K2;  /* New root */
  }

/* This function can be called only if K3 has a left */
/* child and K3's left child has a right child */
/* Do the left-right double rotation */
/* Update heights, then return new root */

static NodeEntry double_rotate_with_left( NodeEntry K3 )
  {
  /* Rotate between K1 and K2 */
  K3->left = single_rotate_with_right( K3->left );
  
  /* Rotate between K3 and K2 */
  return single_rotate_with_left( K3 );
  }

/* This function can be called only if K1 has a right */
/* child and K1's right child has a left child */
/* Do the right-left double rotation */
/* Update heights, then return new root */

static NodeEntry double_rotate_with_right( NodeEntry K1 )
  {
  /* Rotate between K3 and K2 */
  K1->right = single_rotate_with_left( K1->right );
  
  /* Rotate between K1 and K2 */
  return single_rotate_with_right( K1 );
  }

static AvlTree AVL_insert_internal( u_long key, uint16_t port, struct pbsnode *node, AvlTree tree )
  {
  if (tree == NULL)
    {
    /* Create and return a node */
    if ((tree = ( AvlTree )calloc(1, sizeof( struct AvlNode ) )) == NULL)
      {
      return( tree );
      }

    tree->key = key;
    tree->port = port;
    tree->pbsnode = node;
    tree->left = NULL;
    tree->right = NULL;
    tree->height = 0;
    }


  /* If key is less than current node value go left else go right.
		 If equal compare port and go left or right accordingly */
  if (key < tree->key)
    {
    tree->left = AVL_insert_internal( key, port, node, tree->left );
    if (height(tree->left) - height(tree->right) >= 2 )
      {
      if (key <= tree->left->key )
        tree = single_rotate_with_left( tree );
      else
        tree = double_rotate_with_left( tree );
      }
    }
  else if (key > tree->key )
    {
    tree->right = AVL_insert_internal( key, port, node, tree->right );
    if (height(tree->right) - height(tree->left) >= 2 )
      {
      if (key >= tree->right->key)
        tree = single_rotate_with_right( tree );

      else
        tree = double_rotate_with_right( tree );
      }
    }
  else
    {
    /* if it is in the tree, do not add it again */
    if (port == tree->port)
      return(tree);

    /* the keys are equal. sort by port */
    if (port != 0)
      {
      if (port < tree->port)
        {
        tree->left = AVL_insert_internal( key, port, node, tree->left );
        if (height(tree->left) - height(tree->right) >= 2)
          {
          if (port <= tree->left->port)
            tree = single_rotate_with_left( tree );
          else
            tree = double_rotate_with_left( tree );
          }
        }
      else if (port > tree->port )
        {
        tree->right = AVL_insert_internal( key, port, node, tree->right );
        if (height(tree->right) - height(tree->left) >= 2)
          {
          if (port >= tree->right->port)
            tree = single_rotate_with_right( tree );
          else
            tree = double_rotate_with_right( tree );
          }
        }
      }
    }


  tree->height = Max(height(tree->left), height(tree->right)) + 1;

  return(tree);
  } /* End AVL_insert_internal */

AvlTree AVL_insert( u_long key, uint16_t port, struct pbsnode *node, AvlTree tree )
  {
  pthread_mutex_lock(&AVLMutex);
  AvlTree t = AVL_insert_internal(key,port,node,tree);
  pthread_mutex_unlock(&AVLMutex);
  return t;
  }


/* return a pbsnode with the corresponding key and port */
static struct pbsnode *AVL_find_internal(
    
  u_long   key,
  uint16_t port,
  AvlTree  tree)

  {
	if (tree == NULL)
		{
		return( NULL );
		}

	if (key < tree->key)
		return( AVL_find_internal( key, port, tree->left ));
	else if ( key > tree->key )
		return( AVL_find_internal( key, port, tree->right ));
	else
		{
		if (port < tree->port)
			return( AVL_find_internal( key, port, tree->left ) );
		else if ( port > tree->port )
			return( AVL_find_internal( key, port, tree->right ));
		}

		return( tree->pbsnode );
  } /* end AVL_find_internal */

struct pbsnode *AVL_find(
    u_long   key,
    uint16_t port,
    AvlTree  tree)
  {
  pthread_mutex_lock(&AVLMutex);
  pbsnode *pn = AVL_find_internal(key,port,tree);
  pthread_mutex_unlock(&AVLMutex);
  return pn;
  }

/* If the key and port are found return 1 otherwise return 0 */
static int AVL_is_in_tree_internal(u_long key, uint16_t port, AvlTree tree)
  {
  AvlTree current = tree;

  while (current != NULL)
    {
    if (key < current->key)
      current = current->left;
    else if (key > current->key)
      current = current->right;
    else
      {
      if (port < current->port)
        current = current->left;
      else if(port > current->port)
        current = current->right;
      else
        return(1);
      }
    }
  return(0);
  } /* end AVL_is_in_tree_internal */

int AVL_is_in_tree(u_long key, uint16_t port, AvlTree tree)
  {
  pthread_mutex_lock(&AVLMutex);
  int i = AVL_is_in_tree_internal(key,port,tree);
  pthread_mutex_unlock(&AVLMutex);
  return i;
  }


static int AVL_is_in_tree_no_port_compare_internal(

  u_long   key,
  uint16_t port,
  AvlTree  tree)

  {
  if (tree == NULL)
    return(0);

  if (key < tree->key)
    return(AVL_is_in_tree_no_port_compare_internal(key,port,tree->left));
  else if (key > tree->key)
    return(AVL_is_in_tree_no_port_compare_internal(key,port,tree->right));
  else
    return(1);
  } /* END AVL_is_in_tree_no_port_compare_internal() */

int AVL_is_in_tree_no_port_compare(
    u_long   key,
    uint16_t port,
    AvlTree  tree)
  {
  pthread_mutex_lock(&AVLMutex);
  int i = AVL_is_in_tree_no_port_compare_internal(key,port,tree);
  pthread_mutex_unlock(&AVLMutex);
  return i;
  }

static uint16_t AVL_get_port_by_ipaddr_internal(
    
  u_long key,
  AvlTree tree)

  {
  if (tree == NULL)
    return(0);

  if (key < tree->key)
    return(AVL_get_port_by_ipaddr_internal(key,tree->left));
  else if (key > tree->key)
    return(AVL_get_port_by_ipaddr_internal(key,tree->right));
  else
    return(tree->port);
  } /* END AVL_get_port_by_ipaddr_internal() */


uint16_t AVL_get_port_by_ipaddr(

  u_long key,
  AvlTree tree)
  {
  pthread_mutex_lock(&AVLMutex);
  uint16_t i = AVL_get_port_by_ipaddr_internal(key,tree);
  pthread_mutex_unlock(&AVLMutex);
  return i;
  }


static AvlTree AVL_delete_node_internal(
  u_long   key, 
  uint16_t port, 
  AvlTree  tree)

  {
  AvlTree	h1 = NULL;
  AvlTree 	h2 = NULL;

  if (tree == NULL)
    return(NULL);

  if (key < tree->key)
    {
    if (tree->left)
      {
      h1 = AVL_delete_node_internal(key, port, tree->left);
      tree->left = h1;
      }
    return(tree);
    }
  else if (key > tree->key)
    {
    if (tree->right)
      {
      h1 = AVL_delete_node_internal(key, port, tree->right);
      tree->right = h1;
      }
    return(tree);
    }
  else if ( key == tree->key )
    {
    /* we found the key. Now find the port */
    if (port < tree->port)
      {
      if (tree->left)
        {
        h1 = AVL_delete_node_internal(key, port, tree->left);
        tree->left = h1;
        }
      return(tree);
      }
    else if (port > tree->port)
      {
      if (tree->right)
        {
        h1 = AVL_delete_node_internal(key, port, tree->right);
        tree->right = h1;
        }
      return(tree);
      }
    else if (port == tree->port)
      {
      
      /* We found our node. Delete it */
      if (tree->right == NULL && tree->left == NULL)
        {
        free(tree);
        return(NULL);
        }
      else if (tree->right == NULL)
        {
        h1 = tree->left;
        free(tree);
        return(h1);
        }
      else if (tree->left == NULL)
        {
        h1 = tree->right;
        free(tree);
        return(h1);
        }
      else
        {
        /* If right node has no children it replaces the deleted node
         * Otherwise get the right nodes left most decendent and 
         * promote it to the deleted node */
        h1 = tree->right;
        if (h1->left == NULL)
          {
          h1->left = tree->left;
          return(h1);
          }
        
        /* Find the left most node of this branch */
        while (h1->left != NULL)
          {
          h2 = h1;
          h1 = h1->left;
          }
        
        /* Promote leftmost node to the deleted node */
        h1->right = tree->right;
        h1->left = tree->left;
        h2->left = NULL;
        free(tree);
        return(h1);
        }
      }
    }
  return(tree);
  } /*  End AVL_delete_node_internal */

AvlTree AVL_delete_node(
  u_long   key,
  uint16_t port,
  AvlTree  tree)
  {
  pthread_mutex_lock(&AVLMutex);
  AvlTree t = AVL_delete_node_internal(key,port,tree);
  pthread_mutex_unlock(&AVLMutex);
  return t;
  }


static int AVL_list_add_item(

  AvlTree   tree,
  char    **r_buf,
  long     *current_len,
  long     *max_len)

  {
  char    *tmp_buf = NULL;
  char    *current_buffer = NULL;
  int     add_len = 0;
	char    tmp_line[32];
 
	/* each entry can be a maximum of 21 bytes plus one for
	   NULL termination and one for a ','. We need at least 23 bytes to make
	   this work. (entry format XXX.XXX.XXX.XXX:XXXXX --
	   This does not work for IPV6 )*/
	if ((*max_len - *current_len) < 23)
    {
    *max_len = *max_len + 1024;
    tmp_buf = (char *)calloc(1, *max_len + 1);
    if (tmp_buf == NULL)
      return PBSE_MEM_MALLOC;
    if(*r_buf != NULL)
      {
      memcpy(tmp_buf, *r_buf, *current_len);
      free(*r_buf);
      }
    *r_buf = tmp_buf;
    }
  current_buffer = *r_buf;

	sprintf( tmp_line, "%ld.%ld.%ld.%ld:%d",
	  (tree->key & 0xFF000000) >> 24,
    (tree->key & 0x00FF0000) >> 16,
    (tree->key & 0x0000FF00) >> 8,
    (tree->key & 0x000000FF),
    tree->port);

  add_len = strlen(tmp_line);
	/* r_buf must come in with at least the first byte set to NULL
	   initially. Every time after that append
	   a comma */
	if (current_buffer[0] == 0)
	  {
		strcpy(current_buffer, tmp_line);
    *current_len = *current_len + add_len;
	  }
	else
	  {
	  strcat(current_buffer, ",");
	  strcat(current_buffer, tmp_line);
    *current_len = *current_len + add_len + 1;
	  }
  return PBSE_NONE;
  }

/* AVL_list_internal -- return the key and port values for
 * each entry in the tree in a ',' delimited list
 * returned in Buf 
 * @param T - root of the tree to list 
 * @param Buf - output buffer 
 * @param BufSize - size of the output buffer 
 * return PBSE_NONE on success 
 * return PBSE_MEM_MALLOC if malloc failed 
 * return PBSE_RMNOPARAM if parameters are invalid.
 */ 
static int AVL_list_internal( AvlTree tree, char **Buf, long *current_len, long *max_len )
  {
	int rc = PBSE_NONE;

	if (Buf == NULL || current_len == NULL || max_len == NULL)
    {
    return PBSE_RMNOPARAM;
    }

	/* start down the left side */
	if ( tree->left != NULL )
    {
		if ((rc = AVL_list_internal(tree->left, Buf, current_len, max_len)) != PBSE_NONE)
      return rc;
    }

  if ((rc = AVL_list_add_item(tree, Buf, current_len, max_len)) != PBSE_NONE)
    return rc;

	/* now go right */
	if ( tree->right != NULL )
    {
    if ((rc = AVL_list_internal(tree->right, Buf, current_len, max_len)) != PBSE_NONE)
      return rc;
    }

	return PBSE_NONE;
  } /* end AVL_list_internal */

int AVL_list( AvlTree tree, char **Buf, long *current_len, long *max_len )
  {
  pthread_mutex_lock(&AVLMutex);
  int i = AVL_list_internal(tree,Buf,current_len,max_len);
  pthread_mutex_unlock(&AVLMutex);
  return i;
  }

AvlTree AVL_clear_tree(
    
  AvlTree tree)

  {
  pthread_mutex_lock(&AVLMutex);
  while (tree != NULL)
    {
    tree = AVL_delete_node_internal(tree->key, tree->port, tree);
    }
  pthread_mutex_unlock(&AVLMutex);
  return(NULL);
  } /* AVL_clear_tree() */

