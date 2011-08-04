#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>


ssize_t write_nonblocking_socket( int fd,  const void *buf,  ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }
