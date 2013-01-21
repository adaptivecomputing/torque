#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "tcp.h"

int tcp_puts(tcp_chan *chan, const char *str, size_t ct)
  {
  fprintf(stderr, "The call to tcp_puts needs to be mocked!!\n");
  exit(1);
  }

char *discui_(char *cp, unsigned value, unsigned *ndigs)
  {
  fprintf(stderr, "The call to discui_ needs to be mocked!!\n");
  exit(1);
  }
