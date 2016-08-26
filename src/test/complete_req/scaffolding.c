#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "resource.h"

int gn_count = 0;

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
      r->rs_value.at_val.at_size.atsv_shift = 10;

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
      r->rs_value.at_val.at_size.atsv_shift = 10;

      break;

    case 8:

      r = NULL;
      break;


    case 9:
      rd.rs_name = strdup("nodes");
      r->rs_value.at_val.at_str = strdup("1:ppn=2");
      break;

    case 10:

      rd.rs_name = strdup("pmem");
      r->rs_value.at_val.at_size.atsv_num = 80;
      r->rs_value.at_val.at_size.atsv_shift = 10;
      break;

    case 11:

      r = NULL;
      break;

    case 12:
      rd.rs_name = strdup("nodes");
      r->rs_value.at_val.at_str = strdup("1:ppn=2");
      break;

    case 13:

      rd.rs_name = strdup("pvmem");
      r->rs_value.at_val.at_size.atsv_num = 80;
      r->rs_value.at_val.at_size.atsv_shift = 10;
      break;

    case 14:

      r = NULL;
      break;

    case 15:
      rd.rs_name = strdup("nodes");
      r->rs_value.at_val.at_str = strdup("1:ppn=2");
      break;

    case 16:

      rd.rs_name = strdup("vmem");
      r->rs_value.at_val.at_size.atsv_num = 80;
      r->rs_value.at_val.at_size.atsv_shift = 10;
      break;

    case 17:

      r = NULL;
      break;

    case 18:

      rd.rs_name = strdup("vmem");
      r->rs_value.at_val.at_size.atsv_num = 2048;
      r->rs_value.at_val.at_size.atsv_shift = 0;
      break;

    case 19:

      r = NULL;
      break;

    case 20:

      rd.rs_name = strdup("procs");
      r->rs_value.at_val.at_long = 2;
      break;

    case 21:

      rd.rs_name = strdup("pmem");
      r->rs_value.at_val.at_size.atsv_num = 1024;
      r->rs_value.at_val.at_size.atsv_shift = 10;
      break;

    case 22:

      r = NULL;
      break;

    case 23:

      rd.rs_name = strdup("procs");
      r->rs_value.at_val.at_long = 1;
      break;

    case 24:

      rd.rs_name = strdup("mem");
      r->rs_value.at_val.at_size.atsv_num = 4096;
      r->rs_value.at_val.at_size.atsv_shift = 10;
      break;

    case 25:

      rd.rs_name = strdup("pmem");
      r->rs_value.at_val.at_size.atsv_num = 5;
      r->rs_value.at_val.at_size.atsv_shift = 10;
      break;

    case 26:

      rd.rs_name = strdup("vmem");
      r->rs_value.at_val.at_size.atsv_num = 8192;
      r->rs_value.at_val.at_size.atsv_shift = 10;
      break;
    
    case 27:

      r = NULL;
      break;

    case 28:

      rd.rs_name = strdup("procs");
      r->rs_value.at_val.at_long = 1;
      break;

    case 29:

      rd.rs_name = strdup("mem");
      r->rs_value.at_val.at_size.atsv_num = 100;
      r->rs_value.at_val.at_size.atsv_shift = 10;
      break;

    case 30:

      rd.rs_name = strdup("pmem");
      r->rs_value.at_val.at_size.atsv_num = 5000;
      r->rs_value.at_val.at_size.atsv_shift = 10;
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

void log_err(

  int         errnum,  /* I (errno or PBSErrno) */
  const char *routine, /* I */
  const char *text)    /* I */

  {
  }
