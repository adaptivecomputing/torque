#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"
#include "libpbs.h" /* batch_reply */

time_t pbs_tcp_timeout;
typedef unsigned int socklen_t;

bool    connect_success = true;
bool    getaddrinfo_success = true;
bool    socket_success = true;
bool    setsockopt_success = true;
bool    close_success = true;
bool    write_success = true;
bool    socket_read_success = true;

char my_active_server[PBS_MAXHOSTNAME] = "eris";
char error_text[100] = "some error text";


int socket_close(int socket)
  {
  fprintf(stderr, "The call to socket_close needs to be mocked!!\n");
  return(1);
  }

int socket_write(int socket, char *data, int data_len)
  {
  if (write_success == true)
    {
    return(data_len);
    }
  else
    return(0);
  }

int decode_DIS_replyCmd(struct tcp_chan *chan, struct batch_reply *reply)
  {
  fprintf(stderr, "The call to decode_DIS_replyCmd needs to be mocked!!\n");
  return(1);
  }

int socket_get_tcp_priv()
  {
  fprintf(stderr, "The call to socket_get_tcp_priv needs to be mocked!!\n");
  return(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  return;
  }

int socket_read_str(int socket, char **the_str, long long *str_len)
  {
  int rc;
  if (socket_read_success == true)
    {
    *the_str = my_active_server;
    rc = PBSE_NONE;
    }
  else
    {
    rc = PBSE_SOCKET_FAULT;
    }

  return(rc);
  }

int socket_read_num(int socket, long long *the_num)
  {
  fprintf(stderr, "The call to socket_read_num needs to be mocked!!\n");
  return(1);
  }

int socket_connect(int *local_socket, char *dest_addr, int dest_addr_len, int dest_port, int family, int is_privileged, char **error_msg)
  {
  fprintf(stderr, "The call to socket_connect needs to be mocked!!\n");
  return(1);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  return(error_text);
  }

char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  return(error_text);
  }

void DIS_tcp_cleanup(tcp_chan *chan)
  {
  fprintf(stderr, "The call to DIS_tcp_cleanup needs to be mocked!!\n");
  return;
  }

int get_hostaddr_hostent(int *local_errno, char *hostname, char **host_addr, int *host_addr_len)
  {
  fprintf(stderr, "The call to get_hostaddr_hostent needs to be mocked!!\n");
  return(1);
  }

int  get_hostaddr_hostent_af(

  int             *rc,
  char            *hostname,
  unsigned short  *af_family,
  char           **host_addr,
  int             *host_addr_len)

  {
  return(0);
  }

void log_record(int eventtype, int objclass, const char *objname, const char *text) {}

int socket_get_unix()
  {
  if (socket_success == true)
    return(10);
  else
    return(PBSE_SOCKET_FAULT * -1);
  }

char *pbs_get_server_list(void)
  {
  return(NULL);
  }

int socket_connect_unix(int sock, const char *unix_sockname, char **err_msg)
  {
  if (connect_success == true)
    return(0);
  else
    return(PBSE_SYSTEM);
  }

void log_event(int a, int b, const char *c, const char *d) {}

char *csv_nth(const char *csv_str, int n)
  {
  return(NULL);
  }

int csv_length(const char *csv_str)
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

int getaddrinfo(

  const char *node,
  const char *service,
  const struct addrinfo *hints,
  struct addrinfo **res)

  {
  if (getaddrinfo_success == true)
    return(0);
  else
    return(-1);
  }

int setsockopt(

  int socket,
  int level,
  int option_name,
  const void *option_value,
  socklen_t option_len) __THROW

  {
  if (setsockopt_success == true)
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

