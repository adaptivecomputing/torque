#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

void log_record(int eventtype, int objclass, char *objname, char *text)
  { 
  fprintf(stderr, "The call to log_record needs to be mocked!!\n");
  exit(1);
  }
