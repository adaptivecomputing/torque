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
#include "dynamic_string.h"
#include "log.h" /* for MAXLINE */


char log_buffer[LOG_BUF_SIZE];



/* dynamic string stuff */

/*
 * checks whether or not the dynamic string needs to grow
 * @return number of bytes to grow, or 0 if no need to grow
 */

size_t need_to_grow(

  dynamic_string *ds,
  const char     *to_check)

  {
  size_t to_add = strlen(to_check) + 1;
  size_t to_grow = 0;

  if (ds->size < ds->used + to_add)
    {
    to_grow = to_add + ds->size;

    if (to_grow < (ds->size * 4))
      to_grow = ds->size * 4;
    }

  return(to_grow);
  } /* END need_to_grow() */




int resize_if_needed(

  dynamic_string *ds,
  const char     *to_check)

  {
  size_t  new_size = need_to_grow(ds, to_check);
  size_t  difference;
  char   *tmp;

  if (new_size > 0)
    {
    /* need to resize */
    difference = new_size - ds->size;

    if ((tmp = realloc(ds->str, new_size)) == NULL)
      return(ENOMEM);

    ds->str = tmp;
    /* zero out the new space as well */
    memset(ds->str + ds->size, 0, difference);
    ds->size = new_size;
    }

  return(PBSE_NONE);
  } /* END resize_if_needed() */




int size_to_dynamic_string(
    
  dynamic_string    *ds,   /* O */
  struct size_value *szv)  /* I */

  {
  char buffer[MAXLINE];
  int  add_one = FALSE;

  if (ds->used == 0)
    add_one = TRUE;

  sprintf(buffer, "%lukb", szv->atsv_num);
  resize_if_needed(ds, buffer);

  sprintf(buffer, "%lu", szv->atsv_num);
  strcat(ds->str, buffer);
  
  switch (szv->atsv_shift)
    {
    case 10:

      strcat(ds->str, "kb");

      break;

    case 20:

      strcat(ds->str, "mb");

      break;

    case 30: 

      strcat(ds->str, "gb");

      break;

    case 40:

      strcat(ds->str, "tb");

      break;

    case 50:

      strcat(ds->str, "pb");

      break;
    }

  ds->used += strlen(buffer) + 2;
  if (add_one == TRUE)
    ds->used += 1;

  return(PBSE_NONE);
  } /* END size_to_dynamic_string() */




/*
 * appends to a string, resizing if necessary
 *
 * @param ds - the string to append to
 * @param to_append - the string being appended
 */
int append_dynamic_string(
    
  dynamic_string *ds,        /* M */
  const char     *to_append) /* I */

  {
  int len = strlen(to_append);
  int add_one = FALSE;
  int offset = ds->used;

  if (ds->used == 0)
    add_one = TRUE;
  else
    offset -= 1;

  resize_if_needed(ds, to_append);
  strcat(ds->str + offset, to_append);
    
  ds->used += len;

  if (add_one == TRUE)
    ds->used += 1;

  return(PBSE_NONE);
  } /* END append_dynamic_string() */





int append_dynamic_string_xml(
    
  dynamic_string *ds,
  const char     *str)

  {
  int i;
  int len = strlen(str);
  int empty_at_start = (ds->used == 0);

  if (empty_at_start == FALSE)
    ds->used -= 1;

  for (i = 0; i < len; i++)
    {
    /* Resize if needed
     * note - QUOT_ESCAPED_LEN is the most we could be adding right now */
    resize_if_needed(ds, QUOT_ESCAPED);

    switch (str[i])
      {
      case '<':

        strcat(ds->str, LT_ESCAPED);
        ds->used += LT_ESCAPED_LEN;

        break;

      case '>':

        strcat(ds->str, GT_ESCAPED);
        ds->used += GT_ESCAPED_LEN;

        break;

      case '&':

        strcat(ds->str, AMP_ESCAPED);
        ds->used += AMP_ESCAPED_LEN;

        break;

      case '"':

        strcat(ds->str, QUOT_ESCAPED);
        ds->used += QUOT_ESCAPED_LEN;

        break;

      case '\'':

        strcat(ds->str, APOS_ESCAPED);
        ds->used += APOS_ESCAPED_LEN;

        break;

      default:

        /* copy one character */
        ds->str[ds->used] = str[i];
        ds->used += 1;

        break;
      }
    }

  ds->str[ds->used] = '\0';
  ds->used += 1;

  return(PBSE_NONE);
  } /* END append_dynamic_string_xml() */





/*
 * performs a strcpy at the end of the string (used for node status strings)
 * the difference is this function places strings with their terminating 
 * characters one after another:
 * string_one'\0'string_two'\0'...
 *
 * @param ds - the dynamic string we're copying to
 * @param str - the string being copied
 */
int copy_to_end_of_dynamic_string(

  dynamic_string *ds,
  const char     *to_copy)

  {
  int     len = strlen(to_copy) + 1;

  resize_if_needed(ds, to_copy);
    
  strcpy(ds->str + ds->used,to_copy);
  ds->used += len;

  return(PBSE_NONE);
  } /* END copy_to_end_of_dynamic_string() */




/* 
 * initializes a dynamic string and returns it, or NULL if there is no memory 
 *
 * @param initial_size - the initial size of the string, use default if -1
 * @param str - the initial string to place in the dynamic string if not NULL
 * @return - the dynamic string object or NULL if no memory
 */
dynamic_string *get_dynamic_string(
    
  int         initial_size, /* I (-1 means default) */
  const char *str)          /* I (optional) */

  {
  dynamic_string *ds = calloc(1, sizeof(dynamic_string));

  if (ds == NULL)
    return(ds);

  if (initial_size > 0)
    ds->size = initial_size;
  else
    ds->size = DS_INITIAL_SIZE;
    
  ds->str = calloc(1, ds->size);

  if (ds->str == NULL)
    {
    free(ds);
    return(NULL);
    }
    
  /* initialize empty str */
  ds->used = 0;

  /* add the string if it exists */
  if (str != NULL)
    {
    if (append_dynamic_string(ds,str) != PBSE_NONE)
      {
      free_dynamic_string(ds);
      return(NULL);
      }
    }

  return(ds);
  } /* END get_dynamic_string() */




int delete_last_word_from_dynamic_string(

  dynamic_string *ds)

  {
  char *curr;

  if (ds->used <= 0)
    return(-1);

  /* used is always strlen(str) + 1 + 1 */
  curr = ds->str + ds->used - 1;

  if ((curr > ds->str) &&
      (*curr == '\0'))
    {
    curr--;
    ds->used -= 1;
    }

  /* walk backwards across str until you hit a null terminator */
  while ((curr >= ds->str) &&
         (*curr != '\0'))
    {
    *curr = '\0';
    curr--;
    ds->used -= 1;
    }

  return(PBSE_NONE);
  } /* END delete_last_word_from_dynamic_string() */




/* 
 * frees a dynamic string 
 *
 * @param ds - the dynamic string being freed 
 */
void free_dynamic_string(
    
  dynamic_string *ds) /* M */

  {
  free(ds->str);
  free(ds);
  } /* END free_dynamic_string() */



/*
 * reset the dynamic string to empty, but do not resize
 *
 * @param ds - the dynamic string being cleared
 */
void clear_dynamic_string(

  dynamic_string *ds) /* M */

  {
  ds->used = 0;
  memset(ds->str, 0, ds->size);
  } /* END clear_dynamic_string() */




char *get_string(

  dynamic_string *ds)

  {
  return(ds->str);
  } /* END get_string() */




int append_char_to_dynamic_string(
    
  dynamic_string *ds,
  char            c)

  {
  char str[2];
  str[0] = c;
  str[1] = '\0';

  return(append_dynamic_string(ds, str));
  } /* END append_char_to_dynamic_string() */


char *threadsafe_tokenizer(

  char **str,    /* M */
  char  *delims) /* I */

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


void log_err(int errnum, const char *routine, char *text) {}




















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


