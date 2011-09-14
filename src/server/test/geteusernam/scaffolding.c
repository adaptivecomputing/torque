#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pwd.h> /* passwd */

#include "pbs_job.h" /* job */
#include "attribute.h" /* attribute_def, attribute  */
#include "server.h" /* server */

attribute_def job_attr_def[10];
struct server server;
char *server_host;


char *site_map_user(char *uname,  char *host) 
  {
  fprintf(stderr, "The call to site_map_user needs to be mocked!!\n");
  exit(1);
  }

char *get_variable(job *pjob, char *variable)
  {
  fprintf(stderr, "The call to get_variable needs to be mocked!!\n");
  exit(1);
  }

struct passwd * getpwnam_ext(char * user_name)
  {
  fprintf(stderr, "The call to getpwnam_ext needs to be mocked!!\n");
  exit(1);
  }

int site_check_user_map(job *pjob, char *luser, char *EMsg)
  {
  fprintf(stderr, "The call to site_check_user_map needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

struct group * getgrnam_ext(char * grp_name)
  {
  fprintf(stderr, "The call to getgrnam_ext needs to be mocked!!\n");
  exit(1);
  }

int acl_check(attribute *pattr, char   *name, int   type)
  {
  fprintf(stderr, "The call to acl_check needs to be mocked!!\n");
  exit(1);
  }

void get_jobowner(char *from, char *to)
  {
  fprintf(stderr, "The call to get_jobowner needs to be mocked!!\n");
  exit(1);
  }


