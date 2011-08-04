#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

int pbs_errno = 0;

char *pbs_geterrmsg(int connect)
  {
  fprintf(stderr, "The call to get_server needs to be mocked!!\n");
  exit(1);
  }
