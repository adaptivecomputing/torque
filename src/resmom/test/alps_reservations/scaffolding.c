#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include "utils.h"
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
