#include <stdlib.h>
#include <stdio.h>

#include "tcp.h"
#include "dis_internal.h"

char *dis_umax = NULL;
unsigned dis_umaxd = 0;
time_t pbs_tcp_timeout = 300;

void disiui_() 
  {
  char   scratch[DIS_BUFSIZ];

  if ((dis_umax != NULL) ||
      (dis_umaxd != 0)) 
    return;

  sprintf(scratch, "%d", UINT_MAX);
  dis_umaxd = strlen(scratch);
  dis_umax = (char *)calloc(1, dis_umaxd + 1);
  memcpy(dis_umax, scratch, dis_umaxd);
  }

int tcp_gets(tcp_chan *chan, char *str, size_t ct, unsigned int timeout)
  {
  return(0);
  }

int tcp_getc(tcp_chan *chan, unsigned int timeout)
  {
  return(0);
  }

int tcp_rskip(tcp_chan *chan, size_t ct)
  {
  return(0);
  }
