#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */ 

#include "pbs_ifl.h" /* attropl */

int pbs_errno = 0;

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

int parse_destination_id(char *destination_in, char **queue_name_out, char **server_name_out)
  { 
  fprintf(stderr, "The call to parse_destination_id needs to be mocked!!\n");
  exit(1);
  }

int locate_job(char *job_id, char *parent_server, char *located_server)
  { 
  fprintf(stderr, "The call to locate_job needs to be mocked!!\n");
  exit(1);
  }

extern "C"
{
int cnt2server(const char *SpecServer, bool silence)
  { 
  fprintf(stderr, "The call to cnt2server needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }
}

int pbs_manager(int c, int command, int objtype, char *objname, struct attropl *attrib, char *extend)
  {
  fprintf(stderr, "The call to pbs_manager needs to be mocked!!\n");
  exit(1);
  }

int pbs_manager_err(int c, int command, int objtype, char *objname, struct attropl *attrib, char *extend, int *local_errno)
  {
  fprintf(stderr, "The call to pbs_manager_err needs to be mocked!!\n");
  exit(1);
  }


void initialize_network_info() {}
