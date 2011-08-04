#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

int pbs_connect(char *server_name_ptr)
  {
  fprintf(stderr, "The call to pbs_connect needs to be mocked!!\n");
  exit(1);
  }

char * pbs_locjob(int c, char *jobid, char *extend)
  {
  fprintf(stderr, "The call to pbs_locjob needs to be mocked!!\n");
  exit(1);
  }

int pbs_disconnect(int connect)
  {
  fprintf(stderr, "The call to pbs_disconnect needs to be mocked!!\n");
  exit(1);
  }


