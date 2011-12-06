#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */


ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

int save_struct(char *pobj, unsigned int objsize, int fds, char *buf, size_t *buf_remaining, size_t buf_size)
  {
  fprintf(stderr, "The call to save_struct needs to be mocked!!\n");
  exit(1);
  }
