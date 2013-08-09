#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <unistd.h>
#include <string.h>
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

int socket_read(int socket, char **the_str, long long *str_len)
  {
  fprintf(stderr, "The call to socket_read needs to be mocked!!\n");
  return(1);
  }

ssize_t write_ac_socket(int fd, const void *x, ssize_t count)
  {
  char buf2[128];
  char *buf = (char *)x;
  char savedChar = *(buf + count);
  *(buf + count) = '\0';
  snprintf(buf2, sizeof(buf2), "%s\n", (char *)buf);
  size_t len = strlen(buf2);
  int wcount = 0;
  wcount = write(fd, (void *)buf2, len);
  *(buf + count) = savedChar;
  if (wcount == (int)len)
    return count;
  else
    return -1;
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }
