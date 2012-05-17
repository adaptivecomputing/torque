#include "license_pbs.h" /* See here for the softare license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* connect_handle */

struct connect_handle connection[10];
const char *dis_emsg[10];

int decode_DIS_replyCmd(struct tcp_chan *chan, struct batch_reply *reply)
  {
  fprintf(stderr, "The call to decode_DIS_replyCmd needs to be mocked!!\n");
  exit(1);
  }

struct tcp_chan *DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan)
  {
  }
