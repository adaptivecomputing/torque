#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* strcpy */

#define SERVER_NAME "hosta"
#define PBS_BATCH_SERVICE_PORT 15001

char *pbs_default(void)
  {
  char *server_name;

  server_name = (char *)malloc(20);
  if (server_name == NULL)
    {
    return(NULL);
    }

  strcpy(server_name, SERVER_NAME);
  return(server_name);
  }

char *PBS_get_server(

    char *server,
    unsigned int *port)
  {
  *port = PBS_BATCH_SERVICE_PORT;
  return(server);
  }

void *process_svr_conn(void *sock)
  {
  fprintf(stderr, "The call to process_svr_conn needs to be mocked!!\n");
  return(0);
  }

int start_listener(const char *server_ip, int server_port, void *(*process_meth)(void *))
  {
  fprintf(stderr, "The call to start_listener needs to be mocked!!\n");
  return(0);
  }

