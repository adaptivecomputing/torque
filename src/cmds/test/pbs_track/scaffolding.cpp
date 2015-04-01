#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

extern "C"
{
int tm_adopt(char *id, int adoptCmd, pid_t pid)
  { 
  fprintf(stderr, "The call to tm_adopt needs to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  { 
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  exit(1);
  }
}

int get_server(const char *job_id_in, char *job_id_out, int jobid_size, char *server_out, int server_size)
  { 
  fprintf(stderr, "The call to get_server needs to be mocked!!\n");
  exit(1);
  }

void initialize_network_info() {}
