#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "tcp.h"

int tcp_rcommit(tcp_chan *chan, int commit_flag)
  {
  fprintf(stderr, "The call to tcp_rcommit needs to be mocked!!\n");
  exit(1);
  }

int disrsl_(tcp_chan *chan, int *negate, unsigned long *value, unsigned long count)
  {
  fprintf(stderr, "The call to disrsl_ needs to be mocked!!\n");
  exit(1);
  }
