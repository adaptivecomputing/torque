#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include <pbs_ifl.h> /* batch_status */
#include "tcp.h"
#include "resmon.h"

char TRMEMsg[1024]; /* global rm error message */
int  flush_rc = 0;
char *string_read = NULL;

extern "C"
{
int openrm(char  *host, unsigned int port)
  {
  return(0);
  }
}

int pbs_connect(char *server_name_ptr)
  {
  return(0);
  }

int pbs_disconnect(int connect)
  {
  return(0);
  }

int IamRoot()
  {
  fprintf(stderr, "The call to IamRoot needs to be mocked!!\n");
  exit(1);
  }

int diswsl(struct tcp_chan *chan, long value)
  {
  return(0);
  }

void pbs_statfree(struct batch_status *bsp)
  {
  fprintf(stderr, "The call to pbs_statfree needs to be mocked!!\n");
  exit(1);
  }

struct tcp_chan *DIS_tcp_setup(int fd)
  {
  return((struct tcp_chan *)calloc(1, sizeof(tcp_chan)));
  }

struct batch_status *pbs_statnode_err(int c, char *id, struct attrl *attrib, char *extend, int *local_errno)
  {
  fprintf(stderr, "The call to pbs_statnode_err needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(tcp_chan *chan)
  {
  return(flush_rc);
  }

int diswcs(struct tcp_chan *chan, const char *value, size_t nchars)
  {
  return(0);
  }

char *disrst(struct tcp_chan *chan, int *retval)
  {
  return(string_read);
  }

long disrsl(struct tcp_chan *chan, int *retval)
  {
  *retval = 0;
  return(RM_RSP_OK);
  }

extern "C"
{
char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }
}

char *pbs_default(void)
  {
  fprintf(stderr, "The call to pbs_default needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_close(struct tcp_chan *chan) {}

void DIS_tcp_cleanup(struct tcp_chan *chan) {}
