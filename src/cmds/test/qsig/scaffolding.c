#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */ 

int pbs_errno = 0;
char *pbs_server = NULL;

int pbs_disconnect(int connect)
  { 
  fprintf(stderr, "The call to pbs_disconnect needs to be mocked!!\n");
  exit(1);
  }

int locate_job(char *job_id, char *parent_server, char *located_server)
  { 
  fprintf(stderr, "The call to locate_job needs to be mocked!!\n");
  exit(1);
  }

int cnt2server(char *SpecServer)
  { 
  fprintf(stderr, "The call to cnt2server needs to be mocked!!\n");
  exit(1);
  }

int pbs_sigjobasync(int c, char *jobid, char *signal, char *extend)
  {
  fprintf(stderr, "The call to pbs_sigjobasync needs to be mocked!!\n");
  exit(1);
  }

int pbs_sigjob(int   c, char *jobid, char *signal, char *extend)
  {
  fprintf(stderr, "The call to pbs_sigjob needs to be mocked!!\n");
  exit(1);
  }

int pbs_sigjobasync_err(int c, char *jobid, char *signal, char *extend, int *local_err)
  {
  fprintf(stderr, "The call to pbs_sigjobasync_err needs to be mocked!!\n");
  exit(1);
  }

int pbs_sigjob_err(int   c, char *jobid, char *signal, char *extend, int *local_err)
  {
  fprintf(stderr, "The call to pbs_sigjob_err needs to be mocked!!\n");
  exit(1);
  }

int get_server(char *job_id_in, char *job_id_out, char *server_out)
  {
  fprintf(stderr, "The call to get_server needs to be mocked!!\n");
  exit(1);
  }

void prt_job_err(char *cmd, int connect, char *id)
  { 
  fprintf(stderr, "The call to prt_job_err needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }
