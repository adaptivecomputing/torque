#include "utils.h"
#include "u_tree.h"


/* height - returns the height of the given node */
static int height(NodeEntry node)
{
	if( node == NULL )
		return( -1 );
	else
		return( node->height );
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

    K1 = K2->left;
    K2->left = K1->right;
    K1->right = K2;

    K2->height = Max( height( K2->left ), height( K2->right ) ) + 1;
    K1->height = Max( height( K1->left ), K2->height ) + 1;

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

    K1->height = Max( height( K1->left ), height( K1->right ) ) + 1;
    K2->height = Max( height( K2->right ), K1->height ) + 1;

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

AvlTree AVL_insert( u_long key, uint16_t port, struct pbsnode *node, AvlTree tree )
  {

	/* If this key is already in the tree do nothing */
	if(AVL_is_in_tree( key, port, tree ))
	  {
	  return( tree );
	  }

	if(tree == NULL)
	  {
	  /* Create and return a node */
	  tree = ( AvlTree )malloc( sizeof( struct AvlNode ) );
	  if(tree == NULL)
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
	if ( key < tree->key )
		{
		tree->left = AVL_insert( key, port, node, tree->left );
		if( height( tree->left ) - height( tree->right ) == 2 )
			{
			if( key <= tree->left->key )
				tree = single_rotate_with_left( tree );
			else
				tree = double_rotate_with_left( tree );
			}
		}
	else if (key > tree->key )
		{
		tree->right = AVL_insert( key, port, node, tree->right );
		if( (height( tree->right ) - height( tree->left )) == 2 )
		  {
		  if(key >= tree->right->key)
        tree = single_rotate_with_right( tree );
		  
		  else
        tree = double_rotate_with_right( tree );
      }
    }
	else
		{
		/* the keys are equal. sort by port */
		if( port != 0 )
			{
			if( port < tree->port )
			  {
			  tree->left = AVL_insert( key, port, node, tree->left );
			  if( height( tree->left ) - height( tree->right ) == 2 )
			  	{
			  	if( port <= tree->left->port)
			  		tree = single_rotate_with_left( tree );
			  	else
			  		tree = double_rotate_with_left( tree );
			  	}
			  }
			else if (port > tree->port )
			  {
			  tree->right = AVL_insert( key, port, node, tree->right );
			  if( height( tree->right ) - height( tree->left ) == 2 )
			  	{
			  	if( port >= tree->right->port)
			  		tree = single_rotate_with_right( tree );
			  	else
			  		tree = double_rotate_with_right( tree );
			  	}
			  }
			}									  
		}


	tree->height = Max( height( tree->left ), height( tree->right )) + 1;
	return( tree );
  } /* End AVL_insert */

/* return a pbsnode with the corresponding key and port */
struct pbsnode *AVL_find(u_long key, uint16_t port, AvlTree tree)
  {
	if(tree == NULL)
		{
		return( NULL );
		}

	if( key < tree->key )
		return( AVL_find( key, port, tree->left ));
	else if ( key > tree->key )
		return( AVL_find( key, port, tree->right ));
	else
		{
		if( port < tree->port )
			return( AVL_find( key, port, tree->left ) );
		else if ( port > tree->port )
			return( AVL_find( key, port, tree->right ));
		}

		return( tree->pbsnode );
  } /* end AVL_find */

/* If the key and port are found return 1 otherwise return 0 */
int AVL_is_in_tree(u_long key, uint16_t port, AvlTree tree)
  {
	if(tree == NULL)
		{
		return( 0 );
		}

	if( key < tree->key )
		return( AVL_is_in_tree( key, port, tree->left ));
	else if ( key > tree->key )
		return( AVL_is_in_tree( key, port, tree->right ));
	else
		{
		if( port < tree->port )
			return( AVL_is_in_tree( key, port, tree->left ) );
		else if ( port > tree->port )
			return( AVL_is_in_tree( key, port, tree->right ));
		}

		return( 1 );
  } /* end AVL_is_in_tree */


AvlTree AVL_delete_node( 

  u_long   key, 
  uint16_t port, 
  AvlTree  tree)

  {
  AvlTree	h1;
  AvlTree 	h2=NULL;

  if (tree == NULL)
    return(NULL);

  if( key < tree->key )
    {
    if( tree->left )
      {
      h1 = AVL_delete_node(key, port, tree->left);
      tree->left = h1;
      }
    return(tree);
    }
  else if( key > tree->key )
    {
    if( tree->right )
      {
      h1 = AVL_delete_node(key, port, tree->right);
      tree->right = h1;
      }
    return(tree);
    }
  else if ( key == tree->key )
    {
    /* we found the key. Now find the port */
    if(port < tree->port)
      {
      if( tree->left )
        {
        h1 = AVL_delete_node(key, port, tree->left);
        tree->left = h1;
        }
      return(tree);
      }
    else if(port > tree->port)
      {
      if( tree->right )
        {
        h1 = AVL_delete_node(key, port, tree->right);
        tree->left = h1;
        }
      return(tree);
      }
    else if (port == tree->port)
      {
      
      /* We found our node. Delete it */
      if(tree->right == NULL && tree->left == NULL)
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
        if(h1->left == NULL)
          {
          h1->left = tree->left;
          return(h1);
          }
        
        /* Find the left most node of this branch */
        while(h1->left != NULL)
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
  } /*  End delete_node */

/* AVL_list -- return the key and port values for
 * each entry in the tree in a ',' delimited list
 * returned in Buf 
 * @param T - root of the tree to list 
 * @param Buf - output buffer 
 * @param BufSize - size of the output buffer 
 * return 0 on success 
 * return -1 if Buffer is too small 
 * return 1 if Buf or T are null 
 */ 
int AVL_list( AvlTree tree, char *Buf, uint32_t BufSize )
{
	uint32_t len;
	char     tmpLine[32];
	int rc;

	if( tree == NULL || Buf == NULL || BufSize == 0 )
	{
		return( 1 );
	}

	len = BufSize;

	/* start down the left side */
	if( tree->left != NULL )
	{
		rc = AVL_list( tree->left, Buf, len );
		if( rc )
		{
			return( rc );
		}
		len -= strlen( Buf );
	}

	/* now go right */
	if( tree->right != NULL )
	{
		rc = AVL_list( tree->right, Buf, len );
		if( rc )
		{
			return( rc );
		}
		len -= strlen( Buf );
	}

	/* each entry can be a maximum of 21 bytes plus one for
	   NULL termination and one for a ','. We need at least 23 bytes to make
	   this work. (entry format XXX.XXX.XXX.XXX:XXXXX --
	   This does not work for IPV6 )*/
	if( len < 23 )
	{
	   return( -1 );
	}

	sprintf( tmpLine, "%ld.%ld.%ld.%ld:%d",
			 (tree->key & 0xFF000000) >> 24,
			 (tree->key & 0x00FF0000) >> 16,
			 (tree->key & 0x0000FF00) >> 8,
			 (tree->key & 0x000000FF),
			 tree->port);

	/* Buf must come in with at least the first byte set to NULL
	   initially. Every time after that append
	   a comma */
	if(Buf[0] == 0)
	  {
		strcpy(Buf, tmpLine);
	  }
	else
	  {
	  strcat(Buf, ",");
	  strcat(Buf, tmpLine);
	  }

	return( 0 );
} /* end AVL_list */

