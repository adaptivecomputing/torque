#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#include "pbs_error.h"
#include "net_connect.h"
#include "lib_net.h"

bool  unix_socket_success = true;
bool  socket_connect_success = true;
bool  socket_write_success = true;
bool  socket_read_success = true;
bool  socket_read_code = true;

nfds_t global_poll_nfds = 0;
int global_poll_timeout_sec = 0;

const char *msg_daemonname = "unset";

void disiui_() {}

char *pbse_to_txt( int err)  /* I */

  {
  static char message[30];

  strcpy(message, "some pbse message");
  return(message);
  }  /* END pbse_to_txt() */

int  socket_get_unix()
  {
  if (unix_socket_success == true)
    return(10);
  else
    return(-1);
  }

int socket_connect_unix( int   local_socket, const char *sock_name, char **error_msg)
  {
  int rc;

  if (socket_connect_success == true)
    rc = PBSE_NONE;
  else
    rc = PBSE_DOMAIN_SOCKET_FAULT;

  return(rc);
  }

int parse_daemon_response(long long code, long long len, char *buf)
  {
  int rc = PBSE_NONE;
  if (code == 0)
    {
    /* Success */
    }
  else 
    {
    rc = code;
    } 

  return rc;
  } 

int socket_read_num(int socket, long long *the_num)
  {
  if (socket_read_success == false)
    {
    return(PBSE_SOCKET_READ);
    }

  if (socket_read_code == false)
    *the_num = PBSE_BADHOST;
  else
    *the_num = 0;

  return(PBSE_NONE);
  } 

int socket_write(int socket, const char *data, int data_len)
  {
  int data_len_actual = -1;

  if (data == NULL || socket_write_success == false)
    return(data_len_actual);

  data_len_actual = data_len;
  return(data_len_actual);
  }
  
int socket_close(int sd)
  {
  return(PBSE_NONE);
  }

char *get_cached_nameinfo(
    
  struct sockaddr_in  *sai)

  {
  return(NULL);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  return;
  }

void initialize_connections_table()
  {
  return;
  }

char *PAddrToString(pbs_net_t *Addr)
  {
  fprintf(stderr, "The call to PAddrToString needs to be mocked!!\n");
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}

void log_record(int eventtype, int objclass, const char *objname, const char *text) {}

int pbs_getaddrinfo(const char *pNode,struct addrinfo *pHints,struct addrinfo **ppAddrInfoOut)
  {
  return(1);
  }

char *get_cached_nameinfo(const struct sockaddr_in *sai)
  {
  return(NULL);
  }

bool log_available(int eventtype)
  {
  return true;
  }

char *netaddr_long(long ap, char *out)
  {
  return(NULL);
  }

int get_max_num_descriptors(void)
  {
  return(getdtablesize());
  }

int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout, const sigset_t *sigmask)
  {
  global_poll_nfds = nfds;
  global_poll_timeout_sec = timeout->tv_sec;
  return(0);
  }
