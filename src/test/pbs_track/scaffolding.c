#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tm.h"
#include <string.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>

#include <vector>
#include <string>

extern "C"
{
int tm_adopt(char *id, int adoptCmd, pid_t pid)
  { 
  /* Assume that there are two jobs, 1.localhost and 2.localhost
   * We'll also pretend that all processes with pid under 2000 are owned
   * by other users
   */

  if (strcmp(id, "1.localhost") == 0 ||
      strcmp(id, "2.localhost") == 0)
    {
    if (pid == getpid())
      return 0;

    if (pid < 2000)
      return TM_EPERM;

    return 0;
    }

  return 1;
  }

const char *pbse_to_txt(int err)
  { 
  std::string message = "There are no bugs here";

  return message.c_str();
  }
}
      
int get_server_and_job_ids(
    
  const char *job_id,
  std::vector<std::string> &id_list,
  std::string &server_name)

  {
  return(0);
  }

int get_server(const char *job_id_in, char *job_id_out, int jobid_size, char *server_out, int server_size)
  { 
  fprintf(stderr, "The call to get_server needs to be mocked!!\n");
  exit(1);
  }

void initialize_network_info() {}
