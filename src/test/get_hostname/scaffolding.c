#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

bool pbs_getaddrinfo_got_af_inet = false;

int insert_addr_name_info(
    
  char               *hostname,
  char               *full_hostname,
  struct sockaddr_in *sai)

  {
  return(0);
  }

char *get_cached_fullhostname(const char *hostname,const struct sockaddr_in *sai)

  {
  return(NULL);
  }

int pbs_getaddrinfo(

  const char       *pNode,
  struct addrinfo  *pHints,
  struct addrinfo **ppAddrInfoOut)

  {
  if ((pHints != NULL) && (pHints->ai_family == AF_INET))
    pbs_getaddrinfo_got_af_inet = true;

  return(1);
  }
