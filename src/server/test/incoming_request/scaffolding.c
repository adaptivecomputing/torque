#include <stdlib.h>
#include <stdio.h>

#include "tcp.h"
#include "threadpool.h"

int LOGLEVEL = 10;

void log_err(int errnum, const char *routine, const char *text) 
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

void log_event(int, int, const char *routine, const char *text) 
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

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

char * netaddr(struct sockaddr_in *ap)
  {
  fprintf(stderr, "The call to netaddr needs to be mocked!!\n");
  exit(1);
  return 0;
  }

struct tcp_chan *DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  return 0;
  }

void DIS_tcp_cleanup(tcp_chan *chan)
  {
  fprintf(stderr, "The call to DIS_tcp_cleanup needs to be mocked!!\n");
  exit(1);
  }

void netcounter_incr() {}

void close_conn(int sd, int has_mutex) 
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

int enqueue_threadpool_request(void *(*func)(void *), void *arg, threadpool_t *tp)
  {
  return(0);
  }
