#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <netinet/in.h>
#include "pbs_error.h"

int insert_addr_name_info(
    
  char               *hostname,
  char               *full_hostname,
  struct sockaddr_in *sai)

  {
  return(0);
  }

char *get_cached_fullhostname(

  char               *hostname,
  struct sockaddr_in *sai)

  {
  return(NULL);
  }

int pbs_getaddrinfo(
    
    const char *pNode,
    struct addrinfo *pHints,
    struct addrinfo **ppAddrInfoOut)

  {
  return(PBSE_NONE);
  }
