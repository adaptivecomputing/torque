
#include <stdio.h>
#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <time.h>
#include "u_hash_map_structs.h"
#include "port_forwarding.h"

int pbs_errno = 0; 
char *pbs_server = NULL;
bool exit_called = false;


char *pbs_geterrmsg(int connect)
  {
  fprintf(stderr, "The call to pbs_geterrmsg to be mocked!!\n");
  exit(1);
  }

int hash_find(job_data_container *head, const char *name, job_data **env_var)
  {
  fprintf(stderr, "The call to hash_find to be mocked!!\n");
  exit(1);
  }

int TShowAbout_exit(void)
  {
  fprintf(stderr, "The call to TShowAbout_exit to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket to be mocked!!\n");
  exit(1);
  }

int parse_destination_id(char *destination_in, char **queue_name_out, char **server_name_out)
  {
  fprintf(stderr, "The call to parse_destination_id to be mocked!!\n");
  exit(1);
  }

int check_job_name(char *name, int chk_alpha)
  {
  fprintf(stderr, "The call to check_job_name to be mocked!!\n");
  exit(1);
  }

int hash_add_hash(job_data_container *dest, job_data_container *src, int overwrite_existing)
  {
  fprintf(stderr, "The call to hash_add_hash to be mocked!!\n");
  exit(1);
  }

int locate_job(char *job_id, char *parent_server, char *located_server)
  {
  fprintf(stderr, "The call to locate_job to be mocked!!\n");
  exit(1);
  }

void port_forwarder( struct pfwdsock *socks, int (*connfunc)(char *, long, char *), char *phost, int pport, char *EMsg)
  {
  fprintf(stderr, "The call to port_forwarder to be mocked!!\n");
  exit(1);
  }

int x11_connect_display( char *display, long alsounused, char *EMsg)
  {
  fprintf(stderr, "The call to x11_connect_display to be mocked!!\n");
  exit(1);
  }

int prepare_path( char *path_in,  char *path_out,  char *host) 
  {
  fprintf(stderr, "The call to prepare_path to be mocked!!\n");
  exit(1);
  }

int hash_del_item(job_data_container *jdc, const char *name)
  {
  fprintf(stderr, "The call to hash_del_item to be mocked!!\n");
  exit(1);
  }

int csv_length(const char *csv_str)
  {
  fprintf(stderr, "The call to csv_length to be mocked!!\n");
  exit(1);
  }

char *csv_nth(const char *csv_str, int n)
  {
  fprintf(stderr, "The call to csv_nth to be mocked!!\n");
  exit(1);
  }

time_t cvtdate(char *datestr)
  {
  fprintf(stderr, "The call to cvtdate to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket( int fd,  const void *buf,  ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket to be mocked!!\n");
  exit(1);
  }

int parse_stage_list(char *list)
  {
  fprintf(stderr, "The call to parse_stage_list to be mocked!!\n");
  exit(1);
  }

int add_verify_resources(job_data_container *res_attr, char *resources, int p_type)
  {
  fprintf(stderr, "The call to add_verify_resources to be mocked!!\n");
  exit(1);
  }

int parse_at_list(char *list, int use_count, int abs_path)
  {
  fprintf(stderr, "The call to parse_at_list to be mocked!!\n");
  exit(1);
  }

char * csv_find_string(const char *csv_str, const char *search_str)
  {
  fprintf(stderr, "The call to csv_find_string to be mocked!!\n");
  exit(1);
  }

int pbs_deljob( int c, char *jobid, char *extend)
  {
  fprintf(stderr, "The call to pbs_deljob to be mocked!!\n");
  exit(1);
  }

int pbs_deljob_err( int c, char *jobid, char *extend, int *local_errno)
  {
  fprintf(stderr, "The call to pbs_deljob_err to be mocked!!\n");
  exit(1);
  }

void calloc_or_fail(char **dest, int alloc_size, const char *err_msg)
  {
  fprintf(stderr, "The call to calloc_or_fail to be mocked!!\n");
  exit(1);
  }

int parse_depend_list( char *list, char *rtn_list,  int rtn_size )
  {
  fprintf(stderr, "The call to parse_depend_list to be mocked!!\n");
  exit(1);
  }

void parse_variable_list(

  job_data_container *dest_hash, /* This is the dest hashmap for vars found */
  job_data_container *user_env,  /* This is the source hashmap */
  int                var_type,  /* Type for vars not pulled from the source hash */
  int                op_type,   /* Op for vars not pulled from the source hash */
  char               *the_list)  /* name=value,name1=value1,etc to be parsed */

  {
  fprintf(stderr, "The call to parse_variable_list to be mocked!!\n");
  exit(1);
  }

int cnt2server_conf(long retry)
  {
  fprintf(stderr, "The call to cnt2server_conf to be mocked!!\n");
  exit(1);
  }

void hash_add_or_exit(

  job_data_container *head,          /* M - hashmap */
  const char         *name,               /* I - The item being added to the hashmap */
  const char         *val,                /* I - Sets the value of variable */
  int                 var_type)             /* I - Sets the type of the variable */

  {
  fprintf(stderr, "The call to hash_add_or_exit to be mocked!!\n");
  exit(1);
  }

void set_env_opts(job_data_container *env_attr, char **envp)

  {
  fprintf(stderr, "The call to set_env_opts to be mocked!!\n");
  exit(1);
  }

extern "C"
{
char *pbs_strerror(int err)
  {
  fprintf(stderr, "The call to pbs_strerror to be mocked!!\n");
  exit(1);
  }

char *pbs_default(void)
  {
  fprintf(stderr, "The call to pbs_default to be mocked!!\n");
  exit(1);
  }

int get_fullhostname( char *shortname,  char *namebuf,  int bufsize,  char *EMsg) 
  {
  fprintf(stderr, "The call to get_fullhostname to be mocked!!\n");
  exit(1);
  }

int pbs_disconnect(int connect)
  {
  fprintf(stderr, "The call to pbs_disconnect to be mocked!!\n");
  exit(1);
  }

int cnt2server(const char *SpecServer)
  {
  fprintf(stderr, "The call to cnt2server to be mocked!!\n");
  exit(1);
  }
}

int pbs_submit_hash(

  int                 socket,
  job_data_container *job_attr,
  job_data_container *res_attr,
  char               *script,
  char               *destination,
  char               *extend,  /* (optional) */
  char               **return_jobid,
  char               **msg)

  {
  fprintf(stderr, "The call to pbs_submit_hash to be mocked!!\n");
  exit(1);
  }

int hash_count(job_data_container *head)
  {
  fprintf(stderr, "The call to hash_count to be mocked!!\n");
  exit(1);
  }

int hash_strlen(job_data_container *src)
  {
  fprintf(stderr, "The call to hash_strlen to be mocked!!\n");
  exit(1);
  }

int safe_strncat(char *str, const char *to_append, size_t space_remaining)
  {
  fprintf(stderr, "The call to safe_strncat to be mocked!!\n");
  exit(1);
  }

int load_config(

  char *config_buf, /* O */
  int   BufSize)    /* I */

  {
  return(0);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

char *get_trq_param(const char *param, const char *config_buf)
  {
  fprintf(stderr, "The call to get_trq_param to be mocked!!\n");
  exit(1);
  }
