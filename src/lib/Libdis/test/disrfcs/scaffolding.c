#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "tcp.h"

int tcp_rcommit(struct tcp_chan *chan, int commit_flag)
  {
  fprintf(stderr, "The call to tcp_rcommit needs to be mocked!!\n");
  exit(1);
  }

int disrsi_(int stream, int *negate, unsigned *value, unsigned count)
  {
  fprintf(stderr, "The call to disrsi_ needs to be mocked!!\n");
  exit(1);
  }

int tcp_gets(struct tcp_chan *chan, char *str, size_t ct)
  {
  fprintf(stderr, "The call to tcp_gets needs to be mocked!!\n");
  exit(1);
  }
