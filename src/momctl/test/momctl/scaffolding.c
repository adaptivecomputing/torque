#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include <pbs_ifl.h> /* batch_status */

char TRMEMsg[1024]; /* global rm error message */


int openrm(char  *host, unsigned int port)
  {
  fprintf(stderr, "The call to openrm needs to be mocked!!\n");
  exit(1);
  }

int pbs_connect(char *server_name_ptr)
  {
  fprintf(stderr, "The call to pbs_connect needs to be mocked!!\n");
  exit(1);
  }

int pbs_disconnect(int connect)
  {
  fprintf(stderr, "The call to pbs_disconnect needs to be mocked!!\n");
  exit(1);
  }

int IamRoot()
  {
  fprintf(stderr, "The call to IamRoot needs to be mocked!!\n");
  exit(1);
  }

int diswsl(int stream, long value)
  {
  fprintf(stderr, "The call to diswsl needs to be mocked!!\n");
  exit(1);
  }

void pbs_statfree(struct batch_status *bsp)
  {
  fprintf(stderr, "The call to pbs_statfree needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

struct batch_status *pbs_statnode_err(int c, char *id, struct attrl *attrib, char *extend, int *local_errno)
  {
  fprintf(stderr, "The call to pbs_statnode_err needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

int diswcs(int stream, const char *value, size_t nchars)
  {
  fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
  exit(1);
  }

char *disrst(int stream, int *retval)
  {
  fprintf(stderr, "The call to disrst needs to be mocked!!\n");
  exit(1);
  }

long disrsl(int stream, int *retval)
  {
  fprintf(stderr, "The call to disrsl needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }

char *pbs_default(void)
  {
  fprintf(stderr, "The call to pbs_default needs to be mocked!!\n");
  exit(1);
  }
