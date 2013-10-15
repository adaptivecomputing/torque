#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

int pbs_errno = 0;

extern "C"
{
int pbs_connect(char *server_name_ptr)
  {
  fprintf(stderr, "The call to pbs_connect needs to be mocked!!\n");
  exit(1);
  }

char *pbs_default(void)
  {
  fprintf(stderr, "The call to pbs_default needs to be mocked!!\n");
  exit(1);
  }

char *pbs_fbserver(void)
  {
  fprintf(stderr, "The call to pbs_fbserver needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(

  int err)  /* I */

  {
  return(NULL);
  }
}

int validate_active_pbs_server(char **active_server)
  {
  return(0);
  }

char *PBS_get_server(

  const char         *server,  /* I (NULL|'\0' for not set,modified) */
  unsigned int *port)    /* O */

  {
  return(NULL);
  }

int get_active_pbs_server(char **server)
  {
  return(0);
  }
