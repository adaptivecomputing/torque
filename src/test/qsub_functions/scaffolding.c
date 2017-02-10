
#include <stdio.h>
#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <grp.h>
#include <time.h>
#include <poll.h>
#include "u_hash_map_structs.h"
#include "port_forwarding.h"
#include "req.hpp"
#include "complete_req.hpp"
#include "pbs_ifl.h"

#include "utils.h"
#include "log.h"

#include "utils.h"
#include "log.h"

int pbs_errno = 0; 
char *pbs_server = NULL;
bool  exit_called = false;
bool  submission_string_fail = false;
bool  added_req = false;
bool  stored_complete_req = false;
bool  find_mpp = false;
bool  find_nodes = false;
bool  find_size = false;
bool  validate_path = false;
bool  mem_fail = false;
int   req_val = 0;
bool  init_work_dir = false;

int   global_poll_rc = 0;
short global_poll_revents = 0;
int   global_poll_errno = 0;

std::string added_value;
std::string added_name;
std::vector<std::string> in_hash;

void log_event(int event, int event_class, const char *func_name, const char *buf)
  {}

char *pbs_geterrmsg(int connect)
  {
  return(NULL);
  }

int hash_find(job_data_container *head, const char *name, job_data **env_var)
  {
  static job_data ev("a", "b", 0, 0);
  if ((find_mpp == true) &&
      (!strcmp(name, "mppwidth")))
    return(1);
  else if ((find_nodes == true) &&
           (!strcmp(name, "nodes")))
    return(1);
  else if ((find_size == true) &&
           (!strcmp(name, "size")))
    return(1);
  else if ((validate_path == true) &&
           (!strcmp(name, "validate_path")))
    return(1);
  else if ((init_work_dir == true) &&
           (!strcmp(name, "init_work_dir")))
    {
    // set env_var to point to a legit directory, ".."
    *env_var = new job_data(strdup("foo"), strdup(".."), 0, 0);
    return(1);
    }
  else if (mem_fail == true)
    {
    ev.value = "1";
    *env_var = &ev;
    return(1);
    }
  
  for (unsigned int i = 0; i < in_hash.size(); i++)
    {
    if (in_hash[i] == name)
      return(TRUE);
    }

  return(FALSE);
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

int pbs_deljob_err( int c, const char *jobid, char *extend, int *local_errno)
  {
  return(0);
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

int parse_depend_list(char *list, std::vector<std::string> &dep_list)
  { 
  return(PBSE_NONE);
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
  if (!strcmp(name, ATTR_req_information))
    stored_complete_req = true;

  if (val != NULL)
    added_value = val;

  if (name != NULL)
    added_name = name;
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

int cnt2server(const char *SpecServer, bool silence)
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

complete_req::complete_req() {}

void complete_req::toString(

  std::string &output) const

  {
  char buf[10];
  sprintf(buf, "%d", req_val);
  output = buf;
  }

int complete_req::req_count() const
  {
  return(req_val);
  }

void complete_req::add_req(req &r)
  {
  added_req = true;
  }

req::req()

  {
  }

bool req::cgroup_preference_set() const
  {
  return(false);
  }

int req::set_from_submission_string(

  char        *submission_str,
  std::string &error)

  {
  if (submission_string_fail == true)
    return(-1);

  return(0);
  }

char *get_trq_param(const char *param, const char *config_buf)
  {
  fprintf(stderr, "The call to get_trq_param to be mocked!!\n");
  exit(1);
  }

int req::getPlaceCores() const
  {
  return(4);
  }

int req::get_cores() const
  {
  return(4);
  }


void free_grname(

  struct group *grp,
  char         *user_buf)

  {
  if (user_buf)
    {
    free(user_buf);
    user_buf = NULL;
    }

  if (grp)
    {
    free(grp);
    grp = NULL;
    }

  }


struct group *getgrnam_ext(

  char **user_buf,
  char *grp_name) /* I */

  {
  struct group *grp;
  char  *buf;
  long   bufsize;
  struct group *result;
  int rc;

  *user_buf = NULL;
  if (grp_name == NULL)
    return(NULL);

  bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
  if (bufsize == -1)
    bufsize = 8196;

  buf = (char *)malloc(bufsize);
  if (buf == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "failed to allocate memory");
    return(NULL);
    }

  int alloc_size = sizeof(struct group);
  grp = (struct group *)calloc(1, alloc_size);
  if (grp == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "could not allocate passwd structure");
    free(buf);
    return(NULL);
    }

  rc = getgrnam_r(grp_name, grp, buf, bufsize, &result);
  if ((rc) ||
      (result == NULL))
    {
    /* See if a number was passed in instead of a name */
    if (isdigit(grp_name[0]))
      {
      rc = getgrgid_r(atoi(grp_name), grp, buf, bufsize, &result);
      if ((rc == 0) &&
          (result != NULL))
        {
        *user_buf = buf;
        return(grp);
        }
      }

    sprintf(buf, "getgrnam_r failed: %d", rc);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);

    free(buf);
    free(grp);

    return (NULL);
    }

  *user_buf = buf;
  return(grp);
  } /* END getgrnam_ext() */

int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout, const sigset_t *sigmask)
  {
  fds->revents = global_poll_revents;
  errno = global_poll_errno;

  return(global_poll_rc);
  }

int read_mem_value(const char *value, unsigned long &parsed)
  {
  static int count = 0;

  if (mem_fail)
    {
    if (count++ % 2 == 0)
      parsed = 1;
    else
      parsed = 2;
    }
  else
    parsed = 0;

  return(0);
  }
