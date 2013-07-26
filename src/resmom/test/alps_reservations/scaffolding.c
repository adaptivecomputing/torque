#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include "utils.h"
#include "resizable_array.h"
#include "pbs_error.h"
#include "attribute.h"
#include "pbs_error.h"
#include "log.h" /* for MAXLINE */


char log_buffer[LOG_BUF_SIZE];
char mom_host[1];
char mom_alias[1];
int  LOGLEVEL = 10;



char *threadsafe_tokenizer(

  char **str,    /* M */
  const char  *delims) /* I */

  {
  char *current_char;
  char *start;
  
  if ((str == NULL) ||
      (*str == NULL))
    return(NULL);
  
  /* save start position */
  start = *str;

  /* return NULL at the end of the string */
  if (*start == '\0')
    return(NULL);

  /* begin at the start */
  current_char = start;

  /* advance to the end of the string or until you find a delimiter */
  while ((*current_char != '\0') &&
      (!strchr(delims, *current_char)))
    current_char++;

  /* advance str */
  if (*current_char != '\0')
    {
    /* not at the end of the string */
    *str = current_char + 1;
    *current_char = '\0';
    }
  else
    {
    /* at the end of the string */
    *str = current_char;
    }

  return(start);
  } /* END threadsafe_tokenizer() */


void log_err(int errnum, const char *routine, const char *text) {}




















/* resizable array stuff */





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

    if ((tmp = (slot *)realloc(ra->slots,size)) == NULL)
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




/*
 * inserts an item, resizing the array if necessary
 *
 * @return the index in the array or ENOMEM
 */
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





/* 
 * inserts a thing after the thing in index
 * NOTE: index must represent a valid index
 */
int insert_thing_after(

    resizable_array *ra,
    void            *thing,
    int              index)

  {
  int rc;
  int next;

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
  ra->slots[rc].prev = index;
  next = ra->slots[index].next;
  ra->slots[rc].next = next;
  ra->slots[index].next = rc;

  if (next != 0)
    {
    ra->slots[next].prev = rc;
    }

  /* update the last index if needed */
  if (ra->last == index)
    ra->last = rc;

  /* increase the count */
  ra->num++;

  update_next_slot(ra);

  return(rc);
  } /* END insert_thing_after() */





/* 
 * inserts a thing befire the thing in index
 * NOTE: index must represent a valid index
 */
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





int is_present(

    resizable_array *ra,
    void            *thing)

  {
  int i = ra->slots[ALWAYS_EMPTY_INDEX].next;

  while (i != 0)
    {
    if (ra->slots[i].item == thing)
      return(TRUE);

    i = ra->slots[i].next;
    }

  return(FALSE);
  } /* END is_present() */





/*
 * fix the next pointer for the box pointing to this index 
 *
 * @param ra - the array we're fixing
 * @param index - index of the slot we're unlinking
 */
void unlink_slot(

    resizable_array *ra,
    int              index)

  {
  int prev = ra->slots[index].prev;
  int next = ra->slots[index].next;

  ra->slots[index].prev = ALWAYS_EMPTY_INDEX;
  ra->slots[index].next = ALWAYS_EMPTY_INDEX;
  ra->slots[index].item = NULL;

  ra->slots[prev].next = next;

  /* update last if necessary, otherwise update prev's next index */
  if (ra->last == index)
    ra->last = prev;
  else 
    ra->slots[next].prev = prev;
  } /* END unlink_slot() */





/* 
 * remove a thing from the array
 *
 * @param thing - the thing to remove
 * @return PBSE_NONE if the thing is removed 
 */

int remove_thing(

    resizable_array *ra,
    void            *thing)

  {
  int i = ra->slots[ALWAYS_EMPTY_INDEX].next;
  int found = FALSE;

  /* find the thing */
  while (i != ALWAYS_EMPTY_INDEX)
    {
    if (ra->slots[i].item == thing)
      {
      found = TRUE;
      break;
      }

    i = ra->slots[i].next;
    }

  if (found == FALSE)
    return(THING_NOT_FOUND);

  unlink_slot(ra,i);

  ra->num--;

  /* reset the next_slot index if necessary */
  if (i < ra->next_slot)
    {
    ra->next_slot = i;
    }

  return(PBSE_NONE);
  } /* END remove_thing() */




/*
 * pop the first thing from the array
 *
 * @return the first thing in the array or NULL if empty
 */

void *pop_thing(

    resizable_array *ra)

  {
  void *thing = NULL;
  int   i = ra->slots[ALWAYS_EMPTY_INDEX].next;

  if (i != ALWAYS_EMPTY_INDEX)
    {
    /* get the thing we're returning */
    thing = ra->slots[i].item;

    /* handle the deletion and removal */
    unlink_slot(ra,i);

    ra->num--;

    /* reset the next slot index if necessary */
    if (i < ra->next_slot)
      {
      ra->next_slot = i;
      }
    }

  return(thing);
  } /* END pop_thing() */




int remove_thing_from_index(

    resizable_array *ra,
    int              index)

  {
  int rc = PBSE_NONE;

  if (ra->slots[index].item == NULL)
    rc = THING_NOT_FOUND;
  else
    {
    /* FOUND */
    unlink_slot(ra,index);

    ra->num--;

    if (index < ra->next_slot)
      ra->next_slot = index;
    }

  return(rc);
  } /* END remove_thing_from_index() */




int remove_last_thing(

    resizable_array *ra)

  {
  return(remove_thing_from_index(ra, ra->last));
  } /* END remove_last_thing() */




/*
 * callocs and returns a resizable array with initial size size
 *
 * @param size - the initial size of the resizable array
 * @return a pointer to the resizable array 
 */
resizable_array *initialize_resizable_array(

    int               size)

  {
  resizable_array *ra = (resizable_array *)calloc(1, sizeof(resizable_array));
  size_t           amount = sizeof(slot) * size;

  ra->max       = size;
  ra->num       = 0;
  ra->next_slot = 1;
  ra->last      = 0;

  ra->slots = (slot *)calloc(1, amount);

  return(ra);
  } /* END initialize_resizable_array() */




/*
 * returns the next available item and increments *iter
 */
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




/* 
 * returns the next available item from the back and decrements *iter
 */
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




/* 
 * initialize the iterator for this array 
 */
void initialize_ra_iterator(

    resizable_array *ra,
    int             *iter)

  {
  *iter = ra->slots[ALWAYS_EMPTY_INDEX].next;
  } /* END initialize_ra_iterator() */




/*
 * searches the array for thing, finding the index
 *
 * @param ra - the array to be searched
 * @param thing - the thing we're looking for
 * @return index if present, THING_NOT_FOUND otherwise
 */
int get_index(

    resizable_array *ra,
    void            *thing)

  {
  int i = ra->slots[ALWAYS_EMPTY_INDEX].next;

  for (i = ra->slots[ALWAYS_EMPTY_INDEX].next; i != ALWAYS_EMPTY_INDEX; i = ra->slots[i].next)
    {
    if (ra->slots[i].item == thing)
      break;
    }

  if (i != ALWAYS_EMPTY_INDEX)
    return(i);
  else
    return(THING_NOT_FOUND);
  } /* END get_index() */



void free_resizable_array(

    resizable_array *ra)

  {
  free(ra->slots);
  free(ra);
  } /* END resizable_array() */



void *get_thing_from_index(

    resizable_array *ra,
    int              index)

  {
  if (index == -1)
    index = ra->slots[ALWAYS_EMPTY_INDEX].next;

  if (index >= ra->max)
    return(NULL);
  else
    return(ra->slots[index].item);
  } /* END get_thing_from_index() */



ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

void log_event(int a, int b, char const* c, char const* d)
{
}
