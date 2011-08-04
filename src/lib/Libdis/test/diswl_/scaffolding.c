#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "dis.h"

unsigned dis_dmx10 = 0;
double *dis_dp10 = NULL;
double *dis_dn10 = NULL;

unsigned dis_lmx10 = 0;
dis_long_double_t *dis_lp10 = NULL;
dis_long_double_t *dis_ln10 = NULL;


int tcp_puts(int fd, const char *str, size_t ct)
  {
  fprintf(stderr, "The call to tcp_puts needs to be mocked!!\n");
  exit(1);
  }

int tcp_wcommit(int fd, int commit_flag)
  {
  fprintf(stderr, "The call to tcp_wcommit needs to be mocked!!\n");
  exit(1);
  }

char *discui_(char *cp, unsigned value, unsigned *ndigs)
  {
  fprintf(stderr, "The call to discui_ needs to be mocked!!\n");
  exit(1);
  }

#undef diswsi
int diswsi(int stream, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }

dis_long_double_t disp10l_(int expon)
  {
  fprintf(stderr, "The call to disp10l_ needs to be mocked!!\n");
  exit(1);
  }

void disi10l_(void)
  {
  fprintf(stderr, "The call to disi10l_ needs to be mocked!!\n");
  exit(1);
  }

