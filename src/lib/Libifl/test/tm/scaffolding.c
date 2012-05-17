#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"

time_t pbs_tcp_timeout = 20;


struct tcp_chan *DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_close(struct tcp_chan *chan)
  {
  }

int tcp_chan_has_data(struct tcp_chan *chan)
  {
  fprintf(stderr, "The call to tcp_chan_has_data needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

int diswcs(int stream, const char *value, size_t nchars)
  {
  fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
  exit(1);
  }

int diswui(int stream, unsigned value)
  {
  fprintf(stderr, "The call to diswui needs to be mocked!!\n");
  exit(1);
  }

char *disrcs(int stream, size_t *nchars, int *retval)
  {
  fprintf(stderr, "The call to disrcs needs to be mocked!!\n");
  exit(1);
  }

char *disrst(int stream, int *retval)
  {
  fprintf(stderr, "The call to disrst needs to be mocked!!\n");
  exit(1);
  }

int diswsi(int stream, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }

int disrsi(int stream, int *retval)
  {
  fprintf(stderr, "The call to disrsi needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan)
  {
  }

