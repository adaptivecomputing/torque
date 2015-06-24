#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "tcp.h"

unsigned dis_dmx10 = 0;
double *dis_dp10 = NULL;
double *dis_dn10 = NULL;

int tcp_puts(tcp_chan *chan, const char *str, size_t ct)
  {
  fprintf(stderr, "The call to tcp_puts needs to be mocked!!\n");
  exit(1);
  }

int tcp_wcommit(tcp_chan *chan, int commit_flag)
  {
  fprintf(stderr, "The call to tcp_wcommit needs to be mocked!!\n");
  exit(1);
  }

double disp10d_(int expon)
  {
  fprintf(stderr, "The call to disp10d_ needs to be mocked!!\n");
  exit(1);
  }

char *discui_(char *cp, unsigned value, unsigned *ndigs)
  {
  fprintf(stderr, "The call to discui_ needs to be mocked!!\n");
  exit(1);
  }

void disi10d_() { }

int diswsi(tcp_chan *chan, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }
