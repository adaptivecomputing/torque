#include <stdlib.h>
#include <stdio.h>
#include "tcp.h"

char *dis_umax = NULL;
unsigned dis_umaxd = 0;

void disiui_() {}

int tcp_gets(

  struct tcp_chan *chan,
  char            *str,
  size_t           ct,
  unsigned int     timeout)

  {
  fprintf(stderr, "The call to tcp_gets needs to be mocked!!\n");
  exit(1);
  }

int tcp_getc(

  struct tcp_chan *chan,
  unsigned int     timeout)

  {
  fprintf(stderr, "The call to tcp_getc needs to be mocked!!\n");
  exit(1);
  }

