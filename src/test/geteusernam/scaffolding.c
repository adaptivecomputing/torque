#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pwd.h> /* passwd */
#include <sys/types.h>
#include <grp.h>

#include "pbs_job.h" /* job */
#include "attribute.h" /* attribute_def, pbs_attribute  */
#include "server.h" /* server */
#include "log.h"
#include "acl_special.hpp"
#include "pbs_nodes.h"

attribute_def job_attr_def[10];
struct server server;
char *server_host;
char *msg_orighost = strdup("Couldn't find orighost");
int LOGLEVEL;
bool exists = true;

acl_special limited_acls;


char *site_map_user(char *uname,  char *host) 
  {
  fprintf(stderr, "The call to site_map_user needs to be mocked!!\n");
  exit(1);
  }

char *get_variable(job *pjob, const char *variable)
  {
  return(NULL);
  }

struct passwd * getpwnam_ext(char **user_buf, char * user_name)
  {
  fprintf(stderr, "The call to getpwnam_ext needs to be mocked!!\n");
  exit(1);
  }

int site_check_user_map(job *pjob, char *luser, char *EMsg, int logging)
  {
  fprintf(stderr, "The call to site_check_user_map needs to be mocked!!\n");
  exit(1);
  }

struct group * getgrnam_ext(char **grp_buf, char * grp_name)
  {
  fprintf(stderr, "The call to getgrnam_ext needs to be mocked!!\n");
  exit(1);
  }

int acl_check(pbs_attribute *pattr, char   *name, int   type)
  {
  return(0);
  }

void get_jobowner(char *from, char *to)
  {
  fprintf(stderr, "The call to get_jobowner needs to be mocked!!\n");
  exit(1);
  }

int acl_check_my_array_string(struct array_strings *pas, char *name, int type)
  {
  return(0);
  }

int svr_get_privilege(

  char *user,  /* I */
  char *host)  /* I */

  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

void free_pwnam(struct passwd *pwdp, char *buf)
  {}

void free_grname(struct group *grpp, char *buf)
  {}

struct group *getgrgid_ext(

  char **user_buf,
  gid_t  grp_id) /* I */

  {
  struct group *grp;
  char  *buf;
  long   bufsize;
  struct group *result;
  int rc;

  bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
  if (bufsize == -1)
    bufsize = 8196;

  buf = (char *)malloc(bufsize);
  if (buf == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "failed to allocate memory");
    return(NULL);
    }

  grp = (struct group *)calloc(1, sizeof(struct group));
  if (grp == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "could not allocate passwd structure");
    free(buf);
    return(NULL);
    }

  rc = getgrgid_r(grp_id, grp, buf, bufsize, &result);
  if ((rc) ||
      (result == NULL))
    {
    sprintf(buf, "getgrnam_r failed: %d", rc);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);

    *user_buf = NULL;
    free(buf);
    free(grp);
    return (NULL);
    }

  *user_buf = buf;
  return(grp);
  } /* END getgrnam_ext() */


int pbsnode::unlock_node(const char *caller, const char *msg, int loglevel)
  {
  return(0);
  }

bool acl_special::is_authorized(const std::string &host, const std::string &user) const
  {
  return(true);
  }

acl_special::acl_special() {}

int get_svr_attr_l(

  int   index,
  long *l)

  {
  *l = 1;
  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  *b = true;
  return(0);
  }

int get_svr_attr_arst(

  int  index,
  struct array_strings **arst_ptr)

  {
  size_t need = sizeof(struct array_strings) + 3 * sizeof(char *);
  struct array_strings *arst = (struct array_strings *)calloc(1, need);
  char *buf = (char *)calloc(100, sizeof(char));
  strcpy(buf, "napali");
  strcpy(buf + 7, "waimea");
  arst->as_buf = buf;
  arst->as_usedptr = 2;
  arst->as_bufsize = 100;
  arst->as_next = arst->as_buf + 14;
  arst->as_string[0] = arst->as_buf;
  arst->as_string[1] = arst->as_buf + 7;

  *arst_ptr = arst;

  return(0);
  }

bool node_exists(const char *node_name)
  {
  return(exists);
  }

