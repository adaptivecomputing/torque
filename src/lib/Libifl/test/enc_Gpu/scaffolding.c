#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"
#include "list_link.h" /* tlist_head, list_link */

int diswcs(tcp_chan *chan, const char *value, size_t nchars)
  {
  fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
  exit(1);
  }

int diswui(tcp_chan *chan, unsigned value)
  {
  fprintf(stderr, "The call to diswui needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_ReqExtend(tcp_chan *chan, char *extend)
  {
  fprintf(stderr, "The call to encode_DIS_ReqExtend needs to be mocked!!\n");
  exit(1);
  }
