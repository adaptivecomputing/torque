#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"
#include "list_link.h" /* tlist_head, list_link */

unsigned disrui(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrui needs to be mocked!!\n");
  exit(1);
  }

int disrfcs(tcp_chan *chan, size_t *nchars, size_t achars, char *value)
  {
  fprintf(stderr, "The call to disrfcs needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *newLink, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }
