#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <pthread.h>

int pbs_errno;
pthread_mutex_t *log_mutex;

void log_event(int eventtype, int objclass, const char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

int get_addr_info(char *name, struct sockaddr_in *sa_info, int retry)
  {
  return(0);
  }

