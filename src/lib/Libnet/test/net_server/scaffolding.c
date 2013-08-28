#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

#include "net_connect.h"

const char *msg_daemonname = "unset";

void disiui_() {}

char *get_cached_nameinfo(
    
  struct sockaddr_in  *sai)

  {
  return(NULL);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

void initialize_connections_table()
  {
  fprintf(stderr, "The call to initialize_connections_table needs to be mocked!!\n");
  exit(1);
  }

char *PAddrToString(pbs_net_t *Addr)
  {
  fprintf(stderr, "The call to PAddrToString needs to be mocked!!\n");
  exit(1);
  }

int get_max_num_descriptors(void)
  {
  fprintf(stderr, "The call to get_max_num_descriptors needs to be mocked!!\n");
  exit(1);
  }

int get_fdset_size(void)
  {
  fprintf(stderr, "The call to get_fdset_size needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, const char *routine, const char *text) {}

void log_record(int eventtype, int objclass, const char *objname, const char *text) {}

int pbs_getaddrinfo(const char *pNode,struct addrinfo *pHints,struct addrinfo **ppAddrInfoOut)
  {
  return(0);
  }

char *get_cached_nameinfo(const struct sockaddr_in *sai)
  {
  return(NULL);
  }
