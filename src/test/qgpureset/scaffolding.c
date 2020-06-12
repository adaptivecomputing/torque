#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */ 

int pbs_errno = 0;

extern "C"
{
char *pbs_geterrmsg(int connect)
  { 
  fprintf(stderr, "The call to pbs_geterrmsg needs to be mocked!!\n");
  exit(1);
  }

int pbs_disconnect(int connect)
  { 
  fprintf(stderr, "The call to pbs_disconnect needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  { 
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }

int cnt2server(const char *SpecServer, bool silence)
  { 
  fprintf(stderr, "The call to cnt2server needs to be mocked!!\n");
  exit(1);
  }
}

int pbs_gpureset(int c, char *node, char *gpuid, int permanent, int vol)
  {
  fprintf(stderr, "The call to pbs_gpureset needs to be mocked!!\n");
  exit(1);
  }

int pbs_gpureset_err(int c, char *node, char *gpuid, int permanent, int vol, int *local_errno)
  {
  fprintf(stderr, "The call to pbs_gpureset_err needs to be mocked!!\n");
  exit(1);
  }

void initialize_network_info() {}
