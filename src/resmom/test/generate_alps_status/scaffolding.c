#include <stdlib.h>
#include <stdio.h>


#include "dynamic_string.h"
#include "utils.h"



char log_buffer[16384];






void log_err(int errnum, const char *routine, char *msg) {}



/*
 * status stings are in the format str1\0str2\0...strn\0
 *
 * This search function returns 1 if str is in the entire
 * status string, 0 otherwise
 */

int search_dynamic_string_status(

  dynamic_string *status,
  char           *str)

  {
  unsigned int i;
  char         first_char = str[0];

  for (i = 0; i < status->used; i++)
    {
    if (status->str[i] == first_char)
      {
      if (!strncmp(str, status->str + i, strlen(str)))
        return(1);
      }
    }

  return(0);
  } /* END search_dynamic_string_status() */



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


