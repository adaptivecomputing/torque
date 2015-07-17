#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "tcp.h"

int tcp_puts(tcp_chan *chan, const char *str, size_t ct)
  {
  fprintf(stderr, "The call to tcp_puts needs to be mocked!!\n");
  exit(1);
  }

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

