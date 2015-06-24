#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"

time_t pbs_tcp_timeout = 20;
const char *dis_emsg[] =
  {
  "No error",
  "Input value too large to convert to this type",
  "Tried to write floating point infinity",
  "Negative sign on an unsigned datum",
  "Input count or value has leading zero",
  "Non-digit found where a digit was expected",
  "Input string has an embedded ASCII NUL",
  "Premature end of message",
  "Unable to calloc enough space for string",
  "Supporting protocol failure",
  "Protocol failure in commit",
  "End of File",
  "Invalid condition in code"
  };

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

int DIS_tcp_wflush(tcp_chan *c)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

int diswcs(tcp_chan*, char const*, unsigned long)
  {
  fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
  exit(1);
  }

int diswui(tcp_chan* c, unsigned int i)
  {
  fprintf(stderr, "The call to diswui needs to be mocked!!\n");
  exit(1);
  }

char *disrcs(tcp_chan* c, unsigned long* ul, int* i)
  {
  fprintf(stderr, "The call to disrcs needs to be mocked!!\n");
  exit(1);
  }

char *disrst(tcp_chan* c, int* i)
  {
  fprintf(stderr, "The call to disrst needs to be mocked!!\n");
  exit(1);
  }

int diswsi(tcp_chan *c,int val)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }

int disrsi(tcp_chan* c, int* i)
  {
  fprintf(stderr, "The call to disrsi needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan)
  {
  }

int pbs_getaddrinfo(const char *,struct addrinfo *,struct addrinfo **)
  {
  return -1;
  }

