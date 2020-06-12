#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */ 
#include "pbs_error.h"
#include "net_connect.h"

int pbs_errno = 0;

extern "C"
{
char *pbs_geterrmsg(int connect)
  { 
  fprintf(stderr, "The call to pbs_geterrmsg needs to be mocked!!\n");
  exit(1);
  }

int pbs_disconnect(int connect)
  { 
  fprintf(stderr, "The call to pbs_disconnect needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }

int cnt2server(const char *SpecServer, bool silence)
  { 
  fprintf(stderr, "The call to cnt2server needs to be mocked!!\n");
  exit(1);
  }
}

int pbs_terminate(int c, int manner, char *extend) 
  {
  fprintf(stderr, "The call to pbs_terminate needs to be mocked!!\n");
  exit(1);
  }

int pbs_terminate_err(int c, int manner, char *extend, int *local_errno) 
  {
  fprintf(stderr, "The call to pbs_terminate_err needs to be mocked!!\n");
  exit(1);
  }

void initialize_network_info() {}

int get_fullhostname(

  char *shortname,  /* I */
  char *namebuf,    /* O */
  int   bufsize,    /* I */
  char *EMsg)       /* O (optional,minsize=MAXLINE - 1024) */

  {
  return(PBSE_NONE);
  }

pbs_net_t get_hostaddr(

  int        *local_errno, /* O */    
  const char *hostname)    /* I */

  {
  return(0);
  }

int get_active_pbs_server(
    
  char **active_server,
  int   *port)

  {
  return(0);
  }

