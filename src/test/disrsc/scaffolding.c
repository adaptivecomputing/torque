#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "tcp.h"

time_t pbs_tcp_timeout;

int tcp_rcommit(tcp_chan *chan, int commit_flag)
  {
  fprintf(stderr, "The call to tcp_rcommit needs to be mocked!!\n");
  exit(1);
  }

int disrsi_(tcp_chan *chan, int *negate, unsigned *value, unsigned count, unsigned int timeout)
  {
  fprintf(stderr, "The call to disrsi_ needs to be mocked!!\n");
  exit(1);
  }
