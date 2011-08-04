#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

int pbs_errno;

void log_event(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

