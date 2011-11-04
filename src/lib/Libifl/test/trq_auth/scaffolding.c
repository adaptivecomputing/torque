#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* batch_reply */
int socket_close(int socket)
  {
  fprintf(stderr, "The call to socket_close needs to be mocked!!\n");
  exit(1);
  }

int socket_write(int socket, char *data, int data_len)
  {
  fprintf(stderr, "The call to socket_write needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_replyCmd(int sock, struct batch_reply *reply)
  {
  fprintf(stderr, "The call to decode_DIS_replyCmd needs to be mocked!!\n");
  exit(1);
  }

 int socket_get_tcp_priv()
  {
  fprintf(stderr, "The call to socket_get_tcp_priv needs to be mocked!!\n");
  exit(1);
  }

 void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
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

 int DIS_tcp_istimeout(int sock)
  {
  fprintf(stderr, "The call to DIS_tcp_istimeout needs to be mocked!!\n");
  exit(1);
  }

 int socket_connect(int *local_socket, char *dest_addr, int dest_addr_len, int dest_port, int family, int is_privileged, char **error_msg)
  {
  fprintf(stderr, "The call to socket_connect needs to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  exit(1);
  }

 char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }

 void DIS_tcp_shutdown(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_shutdown needs to be mocked!!\n");
  exit(1);
  }

 int get_hostaddr_hostent(int *local_errno, char *hostname, char **host_addr, int *host_addr_len)
  {
  fprintf(stderr, "The call to get_hostaddr_hostent needs to be mocked!!\n");
  exit(1);
  }


