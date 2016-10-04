#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>

int get_server(const char *job_id_in, char *job_id_out, int jobid_outsize, char *server_out, int server_outsize)
  {
  fprintf(stderr, "The call to get_server needs to be mocked!!\n");
  exit(1);
  }

int get_server_and_job_ids(
    
  const char *job_id,
  std::vector<std::string> &id_list,
  std::string &server_name)

  {
  return(0);
  }
