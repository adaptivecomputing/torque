#include <stdlib.h>
#include <stdio.h>

#include "tcp.h"
#include "threadpool.h"

int LOGLEVEL = 10;

int process_request(tcp_chan *chan)
  {
  return(0);
  }

unsigned disrui_peek(tcp_chan *chan, int *retval)
  {
  return(0);
  }

void *svr_is_request(void *vp)
  {
  return(NULL);
  }

void DIS_tcp_cleanup(tcp_chan *chan) {}

void netcounter_incr() {}

void close_conn(int sd, int has_mutex) {}

int enqueue_threadpool_request(void *(*func)(void *), void *arg, threadpool_t *tp)
  {
  return(0);
  }
