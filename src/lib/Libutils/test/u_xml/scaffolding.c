#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

void log_err(int errnum, char *routine, char *text)
  { 
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

int is_whitespace(char c)
  { 
  fprintf(stderr, "The call to is_whitespace needs to be mocked!!\n");
  exit(1);
  }

