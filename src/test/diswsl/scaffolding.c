#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "tcp.h"
#include <string>

std::string output;

int tcp_puts(tcp_chan *chan, const char *str, size_t ct)
  {
  output += str;
  return ct;
  }

int tcp_wcommit(tcp_chan *chan, int commit_flag)
  {
  return 0;
  }
