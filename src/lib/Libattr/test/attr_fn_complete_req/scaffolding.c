#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "utils.h"
#include "attribute.h"

int         called_encode;

int is_whitespace(

  char c)

  {
  if ((c == ' ')  ||
      (c == '\n') ||
      (c == '\t') ||
      (c == '\r') ||
      (c == '\f'))
    return(TRUE);
  else
    return(FALSE);
  } /* END is_whitespace */

void move_past_whitespace(

  char **str)

  {
  if ((str == NULL) ||
      (*str == NULL))
    return;

  char *current = *str;

  while (is_whitespace(*current) == TRUE)
    current++;

  *str = current;
  } // END move_past_whitespace()


extern "C"
{
char *pbse_to_txt(

  int err)

  {
  return(NULL);
  }
}

int encode_str(
    
  pbs_attribute *pattr,
  tlist_head    *phead,
  const char    *name,
  const char    *rsname,
  int            mode,
  int            perm)

  {
  called_encode++;
  return(PBSE_NONE);
  }

char *threadsafe_tokenizer(

  char       **str,    /* M */
  const char  *delims) /* I */

  {
  return(NULL);
  }

void log_event(

  int         eventtype,
  int         objclass,
  const char *objname,
  const char *text)

  {
  }



void *get_next(

  list_link  pl,   /* I */
  char     *file, /* I */
  int      line) /* I */

  {
  return(NULL);
  }



void translate_range_string_to_vector(

  const char       *range_string,
  std::vector<int> &indices)

  {
  char *str = strdup(range_string);
  char *ptr = str;
  int   prev;
  int   curr;

  while (*ptr != '\0')
    {
    prev = strtol(ptr, &ptr, 10);
    
    if (*ptr == '-')
      {
      ptr++;
      curr = strtol(ptr, &ptr, 10);

      while (prev <= curr)
        {
        indices.push_back(prev);

        prev++;
        }

      if ((*ptr == ',') ||
          (is_whitespace(*ptr)))
        ptr++;
      }
    else
      {
      indices.push_back(prev);

      if ((*ptr == ',') ||
          (is_whitespace(*ptr)))
        ptr++;
      }
    }

  free(str);
  } /* END translate_range_string_to_vector() */
