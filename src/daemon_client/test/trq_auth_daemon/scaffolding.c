#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

char *pbs_default(void)
  {
  fprintf(stderr, "The call to pbs_default needs to be mocked!!\n");
  exit(1);
  }

void *process_svr_conn(void *sock)
  {
  fprintf(stderr, "The call to process_svr_conn needs to be mocked!!\n");
  exit(1);
  }

int start_listener(char *server_ip, int server_port, void *(*process_meth)(void *))
  {
  fprintf(stderr, "The call to start_listener needs to be mocked!!\n");
  exit(1);
  }

