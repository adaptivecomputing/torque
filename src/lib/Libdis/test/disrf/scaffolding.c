#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

char *dis_umax = NULL;
unsigned dis_umaxd = 0;

void disiui_(void)
  {
  fprintf(stderr, "The call to disiui_ needs to be mocked!!\n");
  exit(1);
  }

int tcp_gets(int fd, char *str, size_t ct)
  {
  fprintf(stderr, "The call to tcp_gets needs to be mocked!!\n");
  exit(1);
  }

int tcp_getc(int fd)
  {
  fprintf(stderr, "The call to tcp_getc needs to be mocked!!\n");
  exit(1);
  }

int tcp_rcommit(int fd, int commit_flag)
  {
  fprintf(stderr, "The call to tcp_rcommit needs to be mocked!!\n");
  exit(1);
  }

double disp10d_(int expon)
  {
  fprintf(stderr, "The call to disp10d_ needs to be mocked!!\n");
  exit(1);
  }

int disrsi_(int stream, int *negate, unsigned *value, unsigned count)
  {
  fprintf(stderr, "The call to disrsi_ needs to be mocked!!\n");
  exit(1);
  }

int tcp_rskip(int fd, size_t ct)
  {
  fprintf(stderr, "The call to tcp_rskip needs to be mocked!!\n");
  exit(1);
  }
