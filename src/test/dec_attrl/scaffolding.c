#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"
#include "attribute.h" /* attropl */

unsigned disrui(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrui needs to be mocked!!\n");
  exit(1);
  }

char *disrst(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrst needs to be mocked!!\n");
  exit(1);
  }

void PBS_free_aopl(struct attropl *aoplp)
  {
  fprintf(stderr, "The call to PBS_free_aopl needs to be mocked!!\n");
  exit(1);
  }

