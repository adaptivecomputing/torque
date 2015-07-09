#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"

char *disrcs(tcp_chan *chan, size_t *nchars, int *retval)
  {
  fprintf(stderr, "The call to disrcs needs to be mocked!!\n");
  exit(1);
  }

unsigned disrui(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrui needs to be mocked!!\n");
  exit(1);
  }

int disrfst(tcp_chan *chan, size_t achars, char *value)
  {
  fprintf(stderr, "The call to disrfst needs to be mocked!!\n");
  exit(1);
  }

