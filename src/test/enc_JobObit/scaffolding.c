#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"
#include "list_link.h" /* list_link */
#include "attribute.h" /* svrattrl */

int diswcs(tcp_chan *chan, const char *value, size_t nchars)
  {
  fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_svrattrl(tcp_chan *chan, svrattrl *psattl)
  {
  fprintf(stderr, "The call to encode_DIS_svrattrl needs to be mocked!!\n");
  exit(1);
  }


int diswsi(tcp_chan *chan, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }
