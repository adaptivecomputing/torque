#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

void log_err(int errnum, const char *routine, const char *text)
  { 
  }

int is_whitespace(char c)
  { 
  if(isspace(c)) return 1;
  return 0;
  }

