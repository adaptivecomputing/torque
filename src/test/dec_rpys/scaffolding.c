#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "tcp.h"
#include "list_link.h" /* tlist_head, list_link */

int decode_DIS_svrattrl(tcp_chan *chan, tlist_head *phead)
  {
  fprintf(stderr, "The call to decode_DIS_svrattrl needs to be mocked!!\n");
  exit(1);
  }

int disrfst(tcp_chan *chan, size_t achars, char *value)
  {
  fprintf(stderr, "The call to disrfst needs to be mocked!!\n");
  exit(1);
  }

unsigned disrui(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrui needs to be mocked!!\n");
  exit(1);
  }

char *disrcs(tcp_chan *chan, size_t *nchars, int *retval)
  {
  fprintf(stderr, "The call to disrcs needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *newLink, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

int disrsi(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrsi needs to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, const char *objname, const char *text)
  {
  }

