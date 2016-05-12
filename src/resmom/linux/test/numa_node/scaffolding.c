#include <stdlib.h>
#include <stdio.h> 
  
#include "numa_node.hpp"
#include "mom_memory.h"
#include "req.hpp"

int MOMConfigUseSMT;
char cpulist[1024];

/*const char *use_cores = "usecores";

const char *place_node = "node";
const char *place_socket = "socket";
const char *place_numa_node = "numanode";
const char *place_core = "core";
const char *place_thread = "thread";*/

proc_mem_t *get_proc_mem_from_path(const char *path)
  {
  proc_mem_t *pm = (proc_mem_t *)calloc(1, sizeof(proc_mem_t));

  pm->mem_total = 1024 * 1024 * 5;

  return(pm);
  }

void get_cpu_list(const char *jobid, char *cpuset_buf, int bufsize)
  {
  snprintf(cpuset_buf, bufsize, "%s", cpulist);
  }
  
bool is_physical_core(unsigned int index)
  {
  return(true);
  }

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

void log_err(int num, char const *a, char const *b) {}

char *threadsafe_tokenizer(

  char       **str,    /* M */
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



char *pbse_to_txt(

  int err)  /* I */

  {
  return (char*)0;
  }  /* END pbse_to_txt() */


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

void *get_next(

  list_link  pl,   /* I */
  char     *file, /* I */
  int      line) /* I */

  {
  return(NULL);
  }

#include "../../../src/lib/Libattr/req.cpp"
