#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"
#include "libpbs.h" /* connect_handle */
#include "pbs_ifl.h" /* PBS_MAXUSER */

struct connect_handle connection[10];
char pbs_current_user[PBS_MAXUSER];
time_t pbs_tcp_timeout = 20;

extern "C"
{
unsigned int get_svrport(const char *service_name, const char *ptype, unsigned int pdefault)
  {
  fprintf(stderr, "The call to get_svrport needs to be mocked!!\n");
  exit(1);
  }
}

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int socket_get_tcp()
  {
  fprintf(stderr, "The call to socket_get_tcp needs to be mocked!!\n");
  exit(1);
  }

int csv_length(const char *csv_str)
  {
  fprintf(stderr, "The call to csv_length needs to be mocked!!\n");
  exit(1);
  }

char *csv_nth(const char *csv_str, int n)
  {
  fprintf(stderr, "The call to csv_nth needs to be mocked!!\n");
  exit(1);
  }

tcp_chan *DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(tcp_chan *chan)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

int socket_read_str(int socket, char **the_str, long long *str_len)
  {
  fprintf(stderr, "The call to socket_read_str needs to be mocked!!\n");
  exit(1);
  }

int socket_read_num(int socket, long long *the_num)
  {
  fprintf(stderr, "The call to socket_read_num needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan)
  {
  }

int encode_DIS_ReqHdr(struct tcp_chan *chan, int reqt, char *user)
  {
  fprintf(stderr, "The call to encode_DIS_ReqHdr needs to be mocked!!\n");
  exit(1);
  }

int socket_connect(int *local_socket, char *dest_addr, int dest_addr_len, int dest_port, int family, int is_privileged, char **error_msg)
  {
  fprintf(stderr, "The call to socket_connect needs to be mocked!!\n");
  exit(1);
  }

int socket_write(int socket, const char *data, int data_len)
  {
  fprintf(stderr, "The call to socket_write needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

char *trq_get_if_name()
  {
  fprintf(stderr, "The call to trq_get_if_name needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }

int get_hostaddr_hostent_af(int *local_errno, const char *hostname, unsigned short *af_family, char **host_addr, int *host_addr_len)
  {
  fprintf(stderr, "The call to get_hostaddr_hostent needs to be mocked!!\n");
  exit(1);
  }

int socket_close(int sock)
  {
  return(0);
  }

size_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

int socket_get_tcp_client()
  {
  return(0);
  }

int socket_get_unix()
  {
  return(0);
  }

int socket_connect_unix(int local_socket, const char *sock_name, char **error_msg)
  {
  return(0);
  }

extern "C"
{
char *pbse_to_txt(int err)
  {
  return(NULL);
  }
}

int pbs_getaddrinfo(const char *hostname, struct addrinfo *in, struct addrinfo **out)
  {
  return(0);
  }

int socket_wait_for_write(int sock)
  {
  return(0);
  }
