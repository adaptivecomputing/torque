#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#include "pbs_nodes.h"
#include "resizable_array.h"


int lock_node(struct pbsnode *pnode, const char *caller, char *msg, int level) {return(0);}
int unlock_node(struct pbsnode *pnode, const char *caller, char *msg, int level) {return(0);}

void *next_thing(

  resizable_array *ra,
  int             *iter)

  {
  void *thing;
  int   i = *iter;

  if (i == -1)
    {
    /* initialize first */
    i = ra->slots[ALWAYS_EMPTY_INDEX].next;
    }

  thing = ra->slots[i].item;
  *iter = ra->slots[i].next;

  return(thing);
  } /* END next_thing() */




int hasprop(

  struct pbsnode *pnode,
  struct prop    *props)

  {
  struct  prop    *need;

  for (need = props;need;need = need->next)
    {

    struct prop *pp;

    if (need->mark == 0) /* not marked, skip */
      continue;

    for (pp = pnode->nd_first;pp != NULL;pp = pp->next)
      {
      if (strcmp(pp->name, need->name) == 0)
        break;  /* found it */
      }

    if (pp == NULL)
      {
      return(0);
      }
    }

  return(1);
  }  /* END hasprop() */


resizable_array *initialize_resizable_array(

  int               size)

  {
  resizable_array *ra = calloc(1, sizeof(resizable_array));
  size_t           amount = sizeof(slot) * size;

  ra->max       = size;
  ra->num       = 0;
  ra->next_slot = 1;
  ra->last      = 0;

  ra->slots = calloc(1, amount);

  return(ra);
  } /* END initialize_resizable_array() */


/*
 * checks if the array needs to be resized, and resizes if necessary
 *
 * @return PBSE_NONE or ENOMEM
 */
int check_and_resize(

  resizable_array *ra)

  {
  slot        *tmp;
  size_t       remaining;
  size_t       size;

  if (ra->max == ra->num + 1)
    {
    /* double the size if we're out of space */
    size = (ra->max * 2) * sizeof(slot);

    if ((tmp = realloc(ra->slots,size)) == NULL)
      {
      return(ENOMEM);
      }

    remaining = ra->max * sizeof(slot);

    memset(tmp + ra->max, 0, remaining);

    ra->slots = tmp;

    ra->max = ra->max * 2;
    }

  return(PBSE_NONE);
  } /* END check_and_resize() */




/* 
 * updates the next slot pointer if needed \
 */
void update_next_slot(

  resizable_array *ra) /* M */

  {
  while ((ra->next_slot < ra->max) &&
         (ra->slots[ra->next_slot].item != NULL))
    ra->next_slot++;
  } /* END update_next_slot() */




int insert_thing(
    
  resizable_array *ra,
  void             *thing)

  {
  int rc;
  
  /* check if the array must be resized */
  if ((rc = check_and_resize(ra)) != PBSE_NONE)
    {
    return(-1);
    }
    
  ra->slots[ra->next_slot].item = thing;
 
  /* save the insertion point */
  rc = ra->next_slot;

  /* handle the backwards pointer, next pointer is left at zero */
  ra->slots[rc].prev = ra->last;

  /* make sure the empty slot points to the next occupied slot */
  if (ra->last == ALWAYS_EMPTY_INDEX)
    {
    ra->slots[ALWAYS_EMPTY_INDEX].next = rc;
    }

  /* update the last index */
  ra->slots[ra->last].next = rc;
  ra->last = rc;

  /* update the new item's next index */
  ra->slots[rc].next = ALWAYS_EMPTY_INDEX;

  /* increase the count */
  ra->num++;

  update_next_slot(ra);

  return(rc);
  } /* END insert_thing() */




int insert_thing_before(

  resizable_array *ra,
  void            *thing,
  int              index)

  {
  int rc;
  int prev;
  
  /* check if the array must be resized */
  if ((rc = check_and_resize(ra)) != PBSE_NONE)
    {
    return(-1);
    }

  /* insert this element */
  ra->slots[ra->next_slot].item = thing;
 
  /* save the insertion point */
  rc = ra->next_slot;

  /* move pointers around */
  prev = ra->slots[index].prev;
  ra->slots[rc].next = index;
  ra->slots[rc].prev = prev;
  ra->slots[index].prev = rc;
  ra->slots[prev].next = rc;

  /* increase the count */
  ra->num++;

  update_next_slot(ra);

  return(rc);
  } /* END insert_thing_before() */




void *next_thing_from_back(

  resizable_array *ra,
  int             *iter)

  {
  void *thing;
  int   i = *iter;

  if (i == -1)
    {
    /* initialize first */
    i = ra->last;
    }

  thing = ra->slots[i].item;
  *iter = ra->slots[i].prev;

  return(thing);
  } /* END next_thing_from_back() */


int number(

  char **ptr,
  int   *num)

  {
  char  holder[80];
  int   i = 0;
  char *str = *ptr;

  while (isdigit(*str) && (unsigned int)(i + 1) < sizeof holder)
    holder[i++] = *str++;

  if (i == 0)
    {
    return(1);
    }

  holder[i] = '\0';

  if ((i = atoi(holder)) <= 0)
    {
    return(-1);
    }

  *ptr = str;

  *num = i;

  return(0);
  }  /* END number() */



int property(

  char **ptr,
  char **prop)

  {
  char        *str = *ptr;
  char        *dest = *prop;
  int          i = 0;

  while (isalnum(*str) || *str == '-' || *str == '.' || *str == '=' || *str == '_')
    dest[i++] = *str++;

  dest[i] = '\0';

  /* skip over "/vp_number" */

  if (*str == '/')
    {
    do
      {
      str++;
      }
    while (isdigit(*str));
    }

  *ptr = str;

  return(0);
  }  /* END property() */

int proplist(char **str, struct prop **plist, int *node_req, int *gpu_req)
  {
  struct prop *pp;
  char         name_storage[80];
  char        *pname;
  char        *pequal;

  *node_req = 1; /* default to 1 processor per node */

  pname  = name_storage;
  *pname = '\0';

  for (;;)
    {
    if (property(str, &pname))
      {
      return(1);
      }

    if (*pname == '\0')
      break;

    if ((pequal = strchr(pname, (int)'=')) != NULL)
      {
      /* special property */

      /* identify the special property and place its value */
      /* into node_req       */

      *pequal = '\0';

      if (strcmp(pname, "ppn") == 0)
        {
        pequal++;

        if ((number(&pequal, node_req) != 0) || (*pequal != '\0'))
          {
          return(1);
          }
        }
      else if (strcmp(pname, "gpus") == 0)
        {
        pequal++;

        if ((number(&pequal, gpu_req) != 0) || (*pequal != '\0'))
          {
          return(1);
          }
        }
      else
        {
        return(1); /* not recognized - error */
        }
      }
    else
      {
      pp = (struct prop *)calloc(1, sizeof(struct prop));

      pp->mark = 1;
      pp->name = strdup(pname);
      pp->next = *plist;

      *plist = pp;
      }

    if (**str != ':')
      break;

    (*str)++;
    }  /* END for(;;) */

  return(PBSE_NONE);
  } /* END proplist() */

void free_resizable_array(resizable_array *ra) {}
