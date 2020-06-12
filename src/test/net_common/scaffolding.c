#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>

bool socket_success = true;
bool close_success = true;
bool connect_success = true;

int   global_poll_rc = 0;
short global_poll_revents = 0;
int   global_getsockopt_rc = 0;
int   global_sock_errno = 0;

struct addrinfo * insert_addr_name_info(struct addrinfo *pAddrInfo,const char *host)

  {
  return(NULL);
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

int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout, const sigset_t *sigmask)
  {
  fds->revents = global_poll_revents;

  return(global_poll_rc);
  }

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) throw()
  {
  int *iptr;

  iptr = (int *)optval;
  *iptr = global_sock_errno;

  return(global_getsockopt_rc);
  }
