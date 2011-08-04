#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

int parse_jobid(char *job_id, char **arg_seq_number, char **arg_parent_server, char **arg_current_server)
  {
  fprintf(stderr, "The call to parse_jobid needs to be mocked!!\n");
  exit(1);
  }

int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg)
  {
  fprintf(stderr, "The call to get_fullhostname needs to be mocked!!\n");
  exit(1);
  }

char *pbs_default(void)
  {
  fprintf(stderr, "The call to pbs_default needs to be mocked!!\n");
  exit(1);
  }

