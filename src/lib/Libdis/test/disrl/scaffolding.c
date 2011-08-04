#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "dis.h"

dis_long_double_t disp10l_(int expon)
  {
  fprintf(stderr, "The call to disp10l_ needs to be mocked!!\n");
  exit(1);
  }


int disrl_(int stream, dis_long_double_t *ldval, unsigned *ndigs, unsigned *nskips, unsigned sigd, unsigned count)
  {
  fprintf(stderr, "The call to disrl_ needs to be mocked!!\n");
  exit(1);
  }

int tcp_rcommit(int fd, int commit_flag)
  {
  fprintf(stderr, "The call to tcp_rcommit needs to be mocked!!\n");
  exit(1);
  }

int disrsi_(int stream, int *negate, unsigned *value, unsigned count)
  {
  fprintf(stderr, "The call to disrsi_ needs to be mocked!!\n");
  exit(1);
  }

