
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "u_hash_map_structs.h"
#include "u_memmgr.h"
#include "port_forwarding.h"

int pbs_errno = 0; 
char *pbs_server = NULL;


char *pbs_geterrmsg(int connect)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int hash_find(job_data *head, char *name, job_data **env_var)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int TShowAbout_exit(void)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int pbs_disconnect(int connect)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int parse_destination_id(char *destination_in, char **queue_name_out, char **server_name_out)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int check_job_name(char *name, int chk_alpha)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int hash_add_hash(memmgr **mm, job_data **dest, job_data *src, int overwrite_existing)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int locate_job(char *job_id, char *parent_server, char *located_server)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

void port_forwarder( struct pfwdsock *socks, int (*connfunc)(char *, int, char *), char *phost, int pport, char *EMsg) 
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int cnt2server(char *SpecServer)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int x11_connect_display( char *display, int alsounused, char *EMsg) 
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int prepare_path( char *path_in,  char *path_out,  char *host) 
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int hash_del_item(memmgr **mm, job_data **head, char *name)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int csv_length(char *csv_str)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

char *csv_nth(char *csv_str, int n)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

time_t cvtdate(char *datestr)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

ssize_t write_nonblocking_socket( int fd,  const void *buf,  ssize_t count)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int parse_stage_list(char *list)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int add_verify_resources(memmgr **mm, job_data **res_attr, char *resources, int p_type)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

char *pbs_submit_hash( int c, memmgr **mm, job_data *job_attr, job_data *res_attr, char *script, char *destination, char *extend)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int parse_at_list(char *list, int use_count, int abs_path)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

char * csv_find_string(char *csv_str, char *search_str)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int get_fullhostname( char *shortname,  char *namebuf,  int bufsize,  char *EMsg) 
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int pbs_deljob( int c, char *jobid, char *extend)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

void calloc_or_fail( char **dest, int alloc_size, char *err_msg)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int parse_depend_list( char *list, char *rtn_list,  int rtn_size )
  {
  printf("Error, stub must be completed");
  exit(1);
  }

void parse_variable_list(job_data **dest_hash, job_data *user_env, int var_type,  int op_type,  char *the_list) 
  {
  printf("Error, stub must be completed");
  exit(1);
  }

int cnt2server_conf(long retry)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

char *pbs_strerror(int err)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

void hash_add_or_exit(memmgr **mm, job_data **head, char *name, char *value, int var_type)
  {
  printf("Error, stub must be completed");
  exit(1);
  }

void set_env_opts(job_data **env_attr, char **envp) 
  {
  printf("Error, stub must be completed");
  exit(1);
  }

char *pbs_default(void)
  {
  printf("Error, stub must be completed");
  exit(1);
  }
