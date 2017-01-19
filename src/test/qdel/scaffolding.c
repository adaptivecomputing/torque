#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */ 
#include <vector>
#include <string>

#include "u_hash_map_structs.h"

int pbs_errno = 0;
char *pbs_server = NULL;
bool exit_called = false;

int cnt2server_conf_cnt = 0;
std::vector<long> cnt2server_conf_arg;

char * get_trq_param_ret = NULL;

extern "C" 
{
int pbs_disconnect(int connect)
  { 
  return(PBSE_NONE);
  }

char *pbs_default(void)
  { 
  fprintf(stderr, "The call to pbs_default needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  { 
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }

int cnt2server(const char *SpecServer, bool silence)
  { 
  return 10;
  }
}

int locate_job(char *job_id, char *parent_server, char *located_server)
  { 
  return(TRUE);
  }

int get_server(const char *job_id_in, char *job_id_out, int jobid_size, char *server_out, int server_size)
  { 
  return(PBSE_NONE);
  }

int pbs_deljob_err( int c, const char *jobid, char *extend, int *local_errno)
  { 
  return(0);
  }


int pbs_deljob( int c, char *jobid, char *extend)
  { 
  fprintf(stderr, "The call to pbs_deljob needs to be mocked!!\n");
  exit(1);
  }

int pbs_deljob_err( int c, char *jobid, char *extend, int *local_errno)
  { 
  return(PBSE_NONE);
  }

void prt_job_err(const char *cmd, int connect, const char *id)
  { 
  return;
  }

void initialize_network_info() {}

char *get_trq_param(const char *param, const char *config_buf)
  {
  return get_trq_param_ret;
  }

int cnt2server_conf(long retry)
  {
  cnt2server_conf_cnt++;
  cnt2server_conf_arg.push_back(retry);
  return 0;
  }

int hash_find(job_data_container *head, const char *name, job_data **env_var)
  {
  return 0;
  }

int load_config(char *config_buf, int BufSize)
  {
  return 0;
  }

void hash_add_or_exit(job_data_container *head, const char *name, const char *value, int var_type)
  {
  }

void set_env_opts(job_data_container *env_attr, char **envp)
  {
  }
      
int get_server_and_job_ids(
    
  const char *job_id,
  std::vector<std::string> &id_list,
  std::string &server_name)

  {
  return(0);
  }
