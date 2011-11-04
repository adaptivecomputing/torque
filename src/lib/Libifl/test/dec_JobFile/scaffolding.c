#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

char *disrcs(int stream, size_t *nchars, int *retval)
  {
  fprintf(stderr, "The call to disrcs needs to be mocked!!\n");
  exit(1);
  }

unsigned disrui(int stream, int *retval)
  {
  fprintf(stderr, "The call to disrui needs to be mocked!!\n");
  exit(1);
  }

int disrfst(int stream, size_t achars, char *value)
  {
  fprintf(stderr, "The call to disrfst needs to be mocked!!\n");
  exit(1);
  }

