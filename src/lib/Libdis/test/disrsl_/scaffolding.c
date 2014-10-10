#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "tcp.h"

unsigned dis_umaxd = 0;
time_t pbs_tcp_timeout;

void disiui_() {}

int tcp_gets(tcp_chan *chan, char *str, size_t ct,unsigned int to)
  { 
  fprintf(stderr, "The call to tcp_gets needs to be mocked!!\n");
  exit(1);
  }

int tcp_getc(tcp_chan *chan,unsigned int to)
  { 
  fprintf(stderr, "The call to tcp_getc needs to be mocked!!\n");
  exit(1);
  }

char *discul_(char *cp, unsigned long value, unsigned *ndigs)
  { 
  fprintf(stderr, "The call to discul_ needs to be mocked!!\n");
  exit(1);
  }

