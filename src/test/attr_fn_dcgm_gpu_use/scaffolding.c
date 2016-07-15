#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "utils.h"
#include "attribute.h"
#include "complete_req.hpp"

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

void log_err(int errnum, const char *routine, const char *text) {}

