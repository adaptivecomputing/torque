#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "net_connect.h"

void log_event(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

void initialize_connections_table()
  {
  fprintf(stderr, "The call to initialize_connections_table needs to be mocked!!\n");
  exit(1);
  }

char *PAddrToString(pbs_net_t *Addr)
  {
  fprintf(stderr, "The call to PAddrToString needs to be mocked!!\n");
  exit(1);
  }

int get_max_num_descriptors(void)
  {
  fprintf(stderr, "The call to get_max_num_descriptors needs to be mocked!!\n");
  exit(1);
  }

int get_fdset_size(void)
  {
  fprintf(stderr, "The call to get_fdset_size needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }
