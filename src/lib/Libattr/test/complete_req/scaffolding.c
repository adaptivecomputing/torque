#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "resource.h"

int called_log_event;

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
  called_log_event++;
  }

void *get_next(

  list_link  pl,   /* I */
  char     *file, /* I */
  int      line) /* I */

  {
  static int gn_count = 0;
  static struct resource_def rd;

  resource *r = (resource *)calloc(1, sizeof(resource));
  r->rs_defin = &rd;

  switch (gn_count)
    {
    case 0:

      rd.rs_name = strdup("nodes");
      r->rs_value.at_val.at_str = strdup("1:ppn=2+2:gpus=1");

      break;
    
    case 1:

      rd.rs_name = strdup("mem");
      r->rs_value.at_val.at_size.atsv_num = 40;
      r->rs_value.at_val.at_size.atsv_shift = 20;
      
      break;

    case 2:

      r = NULL;
      break;

    case 3: 

      rd.rs_name = strdup("size");
      r->rs_value.at_val.at_long = 20;
      break;

    case 4:

      rd.rs_name = strdup("mem");
      r->rs_value.at_val.at_size.atsv_num = 40;

      break;

    case 5:

      r = NULL;
      break;

    case 6:
      
      rd.rs_name = strdup("ncpus");
      r->rs_value.at_val.at_long = 16;
      break;

    case 7:

      rd.rs_name = strdup("mem");
      r->rs_value.at_val.at_size.atsv_num = 40;

      break;

    default:

      r = NULL;
      break;
    }

  gn_count++;

  return(r);
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
