#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <netinet/in.h>

bool socket_success = true;
bool close_success = true;
bool connect_success = true;

struct addrinfo * insert_addr_name_info(struct addrinfo *pAddrInfo,const char *host)

  {
  return(pAddrInfo);
  }

char *get_cached_fullhostname(

  char               *hostname,
  struct sockaddr_in *sai)

  {
  return(NULL);
  }

struct sockaddr_in *get_cached_addrinfo_full(const char *hostname)
  {
  return(NULL);
  }

struct sockaddr_in *get_cached_addrinfo(const char *hostname)
  
  {
  return(NULL);
  }


time_t pbs_tcp_timeout;

void log_record(int eventtype, int eventclass, const char *caller, const char *msg)
  {
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count) 
  {
  return(0);
  }

int socket (int __domain, int __type, int __protocol) __THROW
  {
  if (socket_success == true)
    return(10); /* don't return a 0, 1, or 2 because we may end up closing them */
  else
    return(-1);
  }

int close(

  int filedes)

  {
  if (close_success == true)
    return(0);
  else
    return(-1);
  }

int connect(

  int socket,
  const struct sockaddr *address,
  socklen_t address_len)

  {
  if (connect_success == true)
    return(0);
  else
    return(-1);
  }


