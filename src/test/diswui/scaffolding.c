#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "tcp.h"

int diswui_(tcp_chan *chan, unsigned value)
  {
  fprintf(stderr, "The call to diswui_ needs to be mocked!!\n");
  exit(1);
  }

int tcp_wcommit(tcp_chan *chan, int commit_flag)
  {
  fprintf(stderr, "The call to tcp_wcommit needs to be mocked!!\n");
  exit(1);
  }
