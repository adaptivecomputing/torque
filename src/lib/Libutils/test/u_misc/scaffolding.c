#include <stdlib.h>
#include <stdio.h>

#include "array.h"

int atexit(void (*func)(void)) throw()
{
  return 0;
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
  }
