#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "dis.h"
#include "tcp.h"

int pbs_errno;
const char *dis_emsg[10];

time_t pbs_tcp_timeout;

#if 0

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

#endif

extern "C"
{
unsigned int get_svrport(const char *service_name, const char *ptype, unsigned int pdefault)
  {
  fprintf(stderr, "The call to get_svrport needs to be mocked!!\n");
  exit(1);
  }

int socket(int ns,int style,int protocol) throw()
  {
  static int fd = 32;
  return fd++;
  }

int bindresvport(int stream,struct sockaddr_in *addr) throw()
  {
  return 0;
  }

int connect(int sock,const sockaddr *addr,socklen_t addrLen)
  {
  return 0;
  }

int get_max_num_descriptors()
  {
  return(getdtablesize());
  }

char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }
}

int pbs_getaddrinfo(const char *pNode,struct addrinfo *pHints,struct addrinfo **ppAddrInfoOut)
  {
  *ppAddrInfoOut = (struct addrinfo *)calloc(1,sizeof(struct addrinfo));
  (*ppAddrInfoOut)->ai_addr = (struct sockaddr *)calloc(1,sizeof(struct sockaddr_in));
  return(0);
  }

int poll(struct pollfd *fds, nfds_t nfds, int timeout)
  {
  return(0);
  }
