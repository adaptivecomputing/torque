#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <netinet/in.h>

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


struct sockaddr_in *get_cached_addrinfo(
    
  char               *hostname)
  
  {
  return(NULL);
  }


time_t pbs_tcp_timeout;

void log_record(int eventtype, int eventclass, const char *caller, char *msg)
  {
  }
