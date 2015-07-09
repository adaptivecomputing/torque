#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

int LOGLEVEL = 7; /* force logging code to be exercised as tests run */

void log_err(int errnum, const char *routine, const char *text)
  { 
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text)
  { 
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }
