#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "tcp.h"

time_t pbs_tcp_timeout;

char *dis_umax = NULL;
unsigned dis_umaxd = 0;

void disiui_(void) {}

int tcp_gets(tcp_chan *chan, char *str, size_t ct, unsigned int timeout)
  {
  fprintf(stderr, "The call to tcp_gets needs to be mocked!!\n");
  exit(1);
  }

int tcp_getc(tcp_chan *chan, unsigned int timeout)
  {
  fprintf(stderr, "The call to tcp_getc needs to be mocked!!\n");
  exit(1);
  }

int tcp_rcommit(tcp_chan *chan, int commit_flag)
  {
  fprintf(stderr, "The call to tcp_rcommit needs to be mocked!!\n");
  exit(1);
  }

double disp10d_(int expon)
  {
  fprintf(stderr, "The call to disp10d_ needs to be mocked!!\n");
  exit(1);
  }

int disrsi_(tcp_chan *chan, int *negate, unsigned *value, unsigned count, unsigned int timeout)
  {
  fprintf(stderr, "The call to disrsi_ needs to be mocked!!\n");
  exit(1);
  }

int tcp_rskip(tcp_chan *chan, size_t ct)
  {
  fprintf(stderr, "The call to tcp_rskip needs to be mocked!!\n");
  exit(1);
  }
