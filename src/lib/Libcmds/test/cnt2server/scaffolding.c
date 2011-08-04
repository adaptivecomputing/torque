#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

int pbs_errno = 0;

int pbs_connect(char *server_name_ptr)
  {
  fprintf(stderr, "The call to pbs_connect needs to be mocked!!\n");
  exit(1);
  }

char *pbs_default(void)
  {
  fprintf(stderr, "The call to pbs_default needs to be mocked!!\n");
  exit(1);
  }

char *pbs_fbserver(void)
  {
  fprintf(stderr, "The call to pbs_fbserver needs to be mocked!!\n");
  exit(1);
  }
