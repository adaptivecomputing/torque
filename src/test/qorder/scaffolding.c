#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */ 

int pbs_errno = 0;
char *pbs_server = NULL;

extern "C"
{
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

int get_fullhostname( char *shortname, char *namebuf, int bufsize, char *EMsg)
  { 
  fprintf(stderr, "The call to get_fullhostname needs to be mocked!!\n");
  exit(1);
  }

char *pbs_default(void)
  {
  fprintf(stderr, "The call to pbs_default needs to be mocked!!\n");
  exit(1);
  }

int cnt2server(const char *SpecServer, bool silence)
  { 
  fprintf(stderr, "The call to cnt2server needs to be mocked!!\n");
  exit(1);
  }
}

int get_server(const char *job_id_in, char *job_id_out, int jobid_size, char *server_out, int server_size)
  {
  fprintf(stderr, "The call to get_server needs to be mocked!!\n");
  exit(1);
  }

int pbs_orderjob(int c, char *job1, char *job2, char *extend)
  { 
  fprintf(stderr, "The call to pbs_orderjob needs to be mocked!!\n");
  exit(1);
  }

int pbs_orderjob_err(int c, char *job1, char *job2, char *extend, int *local_errno)
  { 
  fprintf(stderr, "The call to pbs_orderjob_err needs to be mocked!!\n");
  exit(1);
  }

void prt_job_err(const char *cmd, int connect, const char *id)
  { 
  fprintf(stderr, "The call to prt_job_err needs to be mocked!!\n");
  exit(1);
  }

void initialize_network_info() {}
