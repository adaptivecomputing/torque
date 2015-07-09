#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

char *pbs_locjob_err(int c, char *jobid, char *extend, int *local_errno)
  {
  fprintf(stderr, "The call to pbs_locjob needs to be mocked!!\n");
  exit(1);
  }

char * pbs_locjob(int c, char *jobid, char *extend)
  {
  fprintf(stderr, "The call to pbs_locjob needs to be mocked!!\n");
  exit(1);
  }

extern "C"
{
int pbs_disconnect(int connect)
  {
  return(0);
  }

int pbs_connect(char *server_name_ptr)
  {
  return(1);
  }
}

