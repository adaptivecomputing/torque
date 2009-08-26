#include "utils.h"

extern int LOGLEVEL;

/**
 * inserts the key into the tree if it isn't there already
 * @param key - the key we are inserting (input)
 * @param nodep - what the new node's nodep should be (optional input)
 * @param rootp - the address of the tree's root
 * NOTE:  tinsert cannot report failure 
*/

void tinsert(

  const u_long   key, /* I */
  struct pbsnode  *nodep, /* I (optional) */  
  tree         **rootp) /* I */

  {
  register tree *q;

  if (rootp == NULL)
    {
    /* invalid tree address - failure */

    return;
    }

  while (*rootp != NULL)
    {
    /* Knuth's T1: */

    if (key == (*rootp)->key) /* T2: */
      {
      /* key already exists */

      return;   /* we found it! */
      }

    rootp = (key < (*rootp)->key) ?

            &(*rootp)->left : /* T3: follow left branch */
            &(*rootp)->right; /* T4: follow right branch */
    }

  /* create new tree node */

  q = (tree *)malloc(sizeof(tree)); /* T5: key not found */

  if (q == NULL)
    {
    /* cannot allocate memory - failure */

    return;
    }

  /* make new tree */
  /* link new node to old */
  *rootp = q;
  /* initialize new tree node */
  q->key = key;   
  q->left = NULL;
  q->right = NULL;
  /* if nodep is null then this doesn't do anything */
  q->nodep = nodep;

  /* success */

  return;
  }  /* END tinsert() */




/** 
 * delete the node with the given key 
 * @param key - the key of the node to be deleted
 * @param rootp - the address of the root of the tree
*/

void *tdelete(

  const u_long   key, /* I */
  tree         **rootp) /* I */

  {
  tree  *p;
  register tree *q;
  register tree *r;

  if (LOGLEVEL >= 6)
    {
    sprintf(log_buffer, "deleting key %lu",
            key);

    log_record(
      PBSEVENT_SCHED,
      PBS_EVENTCLASS_REQUEST,
      "tdelete",
      log_buffer);
    }

  if ((rootp == NULL) || ((p = *rootp) == NULL))
    {
    return(NULL);
    }

  while (key != (*rootp)->key)
    {
    p = *rootp;

    rootp = (key < (*rootp)->key) ?
            &(*rootp)->left :  /* left branch */
            &(*rootp)->right;  /* right branch */

    if (*rootp == NULL)
      {
      return(NULL);  /* key not found */
      }
    }

  r = (*rootp)->right;    /* D1: */

  if ((q = (*rootp)->left) == NULL)  /* Left */
    {
    q = r;
    }
  else if (r != NULL)
    {
    /* Right is null? */

    if (r->left == NULL)
      {
      /* D2: Find successor */
      r->left = q;

      q = r;
      }
    else
      {
      /* D3: Find (struct tree_t *)0 link */

      for (q = r->left;q->left != NULL;q = r->left)
        {
        r = q;
        }

      r->left = q->right;

      q->left = (*rootp)->left;
      q->right = (*rootp)->right;
      }
    }

  free((struct tree_t *)*rootp);     /* D4: Free node */

  *rootp = q;                         /* link parent to new node */

  return(p);
  }  /* END tdelete() */


/*
 * Tree search generalized from Knuth (6.2.2) Algorithm T just like
 * the AT&T man page says.
 *
 * The tree_t structure is for internal use only, lint doesn't grok it.
 *
 * Written by reading the System V Interface Definition, not the code.
 *
 * Totally public domain.
 */
/*LINTLIBRARY*/

/** 
 * finds a pointer to the specified node in the tree
 * @param key - the key of the node to find
 * @param rootp - the address of the tree's root
 * @return a pointer to the node with the specified key or NULL if not found
 *
 * NOTE: on the mom nodep is never used, but it expects a non-NULL value or
 * else it thinks the key wasn't found
 * WARNING: this means that pointer can't be used on the mom and that you 
 * should always use 
*/

struct pbsnode *tfind(

        const u_long   key, /* I */
        tree         **rootp) /* I / O */

  {
  if (rootp == NULL)
    {
    return(NULL);
    }

  while (*rootp != NULL)
    {
    /* Knuth's T1: */

    if (key == (*rootp)->key) /* T2: */
      {
      /* found */

      /* this if logic prevents mom errors, see above note */
      if ((*rootp)->nodep != NULL)
        return((*rootp)->nodep);
      else
        return((struct pbsnode *)1);
      }

    rootp = (key < (*rootp)->key) ?

            &(*rootp)->left : /* T3: follow left branch */
            &(*rootp)->right; /* T4: follow right branch */
    }

  return(NULL);
  }  /* END tfind() */




/** 
 * lists all of the keys in the tree
 * NOTE:  recursive.  Buf not initialized
 * @param rootp - the root node of the tree
 * @param Buf - the output buffer
 * @param BufSize - the size of the output buffer 
 * @return 1 if rootp or Buf are NULL, -1 if the buffer is too small, -1 otherwise
*/

int tlist(

  tree *rootp,   /* I */
  char *Buf,     /* O (modified) */
  int   BufSize) /* I */

  {
  char tmpLine[32];

  int  BSize;

  /* check for bad inputs */
  if ((rootp == NULL) || (Buf == NULL))
    {
    /* empty tree - failure */

    return(1);
    }

  if (BufSize <= 16)
    {
    /* inadequate space to append data */

    return(-1);
    }

  BSize = BufSize;

  if (rootp->left != NULL)
    {
    tlist(rootp->left, Buf, BSize);

    BSize -= strlen(Buf);
    }

  if (rootp->right != NULL)
    {
    tlist(rootp->right, Buf, BSize);

    BSize -= strlen(Buf);
    }

  if (BSize <= 16)
    {
    /* inadequate space to append data */

    return(-1);
    }

  sprintf(tmpLine, "%ld.%ld.%ld.%ld",

          (rootp->key & 0xff000000) >> 24,
          (rootp->key & 0x00ff0000) >> 16,
          (rootp->key & 0x0000ff00) >> 8,
          (rootp->key & 0x000000ff));

  if ((Buf[0] != '\0') && (BSize > 1))
    {
    strcat(Buf, ",");

    BSize--;
    }

  if (BSize > (int)strlen(tmpLine))
    {
    strcat(Buf, tmpLine);
    }

  return(-1);
  }  /* END tlist() */




/**
 * frees the tree from memory
 * @param rootp - the address of the root of the tree
*/
void tfree(

  tree **rootp) /* I */

  {
  if (rootp == NULL || *rootp == NULL)
    {
    return;
    }

  tfree(&(*rootp)->left);

  tfree(&(*rootp)->right);

  free(*rootp);

  *rootp = NULL;

  return;
  }


