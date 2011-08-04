#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "dis.h"

int pbs_errno;
const char *dis_emsg[10];

unsigned int get_svrport(char *service_name, char *ptype, unsigned int pdefault)
  { 
  fprintf(stderr, "The call to get_svrport needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(int fd)
  { 
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

int diswcs(int stream, const char *value, size_t nchars)
  { 
  fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
  exit(1);
  }

int get_max_num_descriptors(void)
  { 
  fprintf(stderr, "The call to get_max_num_descriptors needs to be mocked!!\n");
  exit(1);
  }

char *disrst(int stream, int *retval)
  { 
  fprintf(stderr, "The call to disrst needs to be mocked!!\n");
  exit(1);
  }

int get_fdset_size(void)
  { 
  fprintf(stderr, "The call to get_fdset_size needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  { 
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }

#ifdef diswsi
#undef diswsi
#endif
int diswsi(int stream, int value)
  { 
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }

#ifdef disrsi
#undef disrsi
#endif
int disrsi(int stream, int *retval)
  { 
  fprintf(stderr, "The call to disrsi needs to be mocked!!\n");
  exit(1);
  }

