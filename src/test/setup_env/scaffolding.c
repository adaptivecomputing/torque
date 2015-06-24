#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

void log_err(int errnum, const char *routine, const char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }
