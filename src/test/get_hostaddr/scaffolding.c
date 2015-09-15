#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <pthread.h>

int pbs_errno;
pthread_mutex_t *log_mutex;

void log_event(int eventtype, int objclass, const char *objname, const char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

bool log_available(int eventtype)
  {
  return true;
  }

int pbs_getaddrinfo(

  const char       *pNode,
  struct addrinfo  *pHints,
  struct addrinfo **ppAddrInfoOut)

  {
  return(0);
  }
