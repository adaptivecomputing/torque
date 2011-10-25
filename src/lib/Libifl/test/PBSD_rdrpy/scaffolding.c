#include "license_pbs.h" /* See here for the softare license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* connect_handle */

struct connect_handle connection[10];
const char *dis_emsg[10];

void DIS_tcp_reset(int fd, int i)
  {
  fprintf(stderr, "The call to DIS_tcp_reset needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_replyCmd(int sock, struct batch_reply *reply)
  {
  fprintf(stderr, "The call to decode_DIS_replyCmd needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_istimeout(int sock)
  {
  fprintf(stderr, "The call to DIS_tcp_istimeout needs to be mocked!!\n");
  exit(1);
  }
