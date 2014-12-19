#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int lock_tcp_table()
  {
  fprintf(stderr, "The call to lock_tcp_table needs to be mocked!!\n");
  exit(1);
  }

int unlock_tcp_table()
  {
  fprintf(stderr, "The call to unlock_tcp_table needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, const char *routine, const char *text) {}

int socket_read(int socket, char **the_str, long long *str_len, unsigned int timeout)
  {
  fprintf(stderr, "The call to socket_read needs to be mocked!!\n");
  return(1);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }
