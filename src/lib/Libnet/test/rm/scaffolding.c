#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "dis.h"
#include "tcp.h"

int pbs_errno;
const char *dis_emsg[10];

int DIS_tcp_wflush(tcp_chan *chan)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

int diswcs(tcp_chan *chan, const char *value, size_t nchars)
  {
  fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
  exit(1);
  }

char *disrst(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrst needs to be mocked!!\n");
  exit(1);
  }

#ifdef diswsi
#undef diswsi
#endif

int diswsi(tcp_chan *chan, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }

#ifdef disrsi
#undef disrsi
#endif

int disrsi(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrsi needs to be mocked!!\n");
  exit(1);
  }

tcp_chan *DIS_tcp_setup(int sock)
  {
  return(NULL);
  }

void DIS_tcp_cleanup(tcp_chan *chan) {}

extern "C"
{
unsigned int get_svrport(const char *service_name, const char *ptype, unsigned int pdefault)
  {
  fprintf(stderr, "The call to get_svrport needs to be mocked!!\n");
  exit(1);
  }

int get_max_num_descriptors()
  {
  fprintf(stderr, "The call to get_max_num_descriptors needs to be mocked!!\n");
  exit(1);
  }

int get_fdset_size()
  {
  fprintf(stderr, "The call to get_fdset_size needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }
}

int pbs_getaddrinfo(const char *pNode,struct addrinfo *pHints,struct addrinfo **ppAddrInfoOut)
  {
  return(0);
  }
