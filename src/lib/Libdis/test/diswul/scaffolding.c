#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

int tcp_puts(int fd, const char *str, size_t ct)
  {
  fprintf(stderr, "The call to tcp_puts needs to be mocked!!\n");
  exit(1);
  }

int tcp_wcommit(int fd, int commit_flag)
  {
  fprintf(stderr, "The call to tcp_wcommit needs to be mocked!!\n");
  exit(1);
  }

char *discui_(char *cp, unsigned value, unsigned *ndigs)
  {
  fprintf(stderr, "The call to discui_ needs to be mocked!!\n");
  exit(1);
  }

char *discul_(char *cp, unsigned long value, unsigned *ndigs)
  {
  fprintf(stderr, "The call to discul_ needs to be mocked!!\n");
  exit(1);
  }
