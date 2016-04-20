#include <stdlib.h>
#include <stdio.h>

#include "tcp.h"
#include "threadpool.h"

int LOGLEVEL = 10;
time_t pbs_tcp_timeout = 300;
time_t pbs_incoming_tcp_timeout = 600;
int    peek_count;
bool   busy_pool = false;

void log_err(int errnum, const char *routine, const char *text) {}

void log_event(int, int, const char *routine, const char *text) {}

int process_request(tcp_chan *chan)
  {
  return(0);
  }

unsigned disrui_peek(tcp_chan *chan, int *retval, unsigned int timeout)
  {
  if (peek_count++ % 2 == 0)
    return(2);
  else
    chan->IsTimeout = 1;

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

extern "C" 
{
void close_conn(int sd, int has_mutex)  { }
}

int enqueue_threadpool_request(void *(*func)(void *), void *arg, threadpool_t *tp)
  {
  return(0);
  }

threadpool_t *request_pool;

bool threadpool_is_too_busy(threadpool_t *tp, int perm)
  {
  return(busy_pool);
  }

int write_tcp_reply(

  struct tcp_chan *chan,
  int protocol,
  int version,
  int command,
  int exit_code)

  {
  return(0);
  }
