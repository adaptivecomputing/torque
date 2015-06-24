#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1

void log_err(int errnum, const char *routine, const char *text)
  { 
  }

int is_whitespace(char c)
  { 
  if(isspace(c)) return 1;
  return 0;
  }

char *trim(

  char *str) /* M */

  {
  char *front_ptr;
  char *end_ptr;

  if (str == NULL)
    return(NULL);

  front_ptr = str;
  end_ptr = str + strlen(str) - 1;

  while ((is_whitespace(*front_ptr) == TRUE) &&
         (front_ptr <= end_ptr))
    {
    front_ptr++;
    }

  while ((is_whitespace(*end_ptr) == TRUE) &&
         (end_ptr > front_ptr))
    {
    *end_ptr = '\0';
    end_ptr--;
    }

  return(front_ptr);
  } /* END trim() */



