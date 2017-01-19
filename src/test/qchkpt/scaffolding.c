#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */ 
#include <vector>
#include <string>

int pbs_errno = 0;
char *pbs_server = NULL;

extern "C"
{
int pbs_disconnect(int connect)
  { 
  fprintf(stderr, "The call to pbs_disconnect needs to be mocked!!\n");
  exit(1);
  }

int cnt2server(const char *SpecServer, bool silence)
  { 
  fprintf(stderr, "The call to cnt2server needs to be mocked!!\n");
  exit(1);
  }
}

int locate_job(char *job_id, char *parent_server, char *located_server)
  { 
  fprintf(stderr, "The call to locate_job needs to be mocked!!\n");
  exit(1);
  }

int get_server(const char *job_id_in, char *job_id_out, int job_size, char *server_out, int server_size)
  { 
  fprintf(stderr, "The call to get_server needs to be mocked!!\n");
  exit(1);
  }

void prt_job_err(const char *cmd, int connect, const char *id)
  { 
  fprintf(stderr, "The call to prt_job_err needs to be mocked!!\n");
  exit(1);
  }

int pbs_checkpointjob(int c, char *jobid, char *extend)
  { 
  fprintf(stderr, "The call to pbs_checkpointjob needs to be mocked!!\n");
  exit(1);
  }

int pbs_checkpointjob_err(int c, char *jobid, char *extend, int *local_err)
  { 
  fprintf(stderr, "The call to pbs_checkpointjob_err needs to be mocked!!\n");
  exit(1);
  }

void initialize_network_info() {}
      
int get_server_and_job_ids(
    
  const char *job_id,
  std::vector<std::string> &id_list,
  std::string &server_name)

  {
  return(0);
  }
