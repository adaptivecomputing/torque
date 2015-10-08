#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <sys/types.h>
#include <pwd.h>

#include "log.h" /* LOG_BUF_SIZE */

char log_buffer[LOG_BUF_SIZE];
int  uid_i = 1;
int  nam_i;

void log_ext(int errnum, const char *routine, const char *text, int severity)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

struct passwd *getpwuid_wrapper(char **buf, uid_t uid)
  {
  if (uid_i++ % 5 == 0)
    return(getpwuid(uid));
  else
    return(NULL);
  }

struct passwd *getpwnam_wrapper(char **user_buf, const char *user_name)
  {
  char *buf;

  buf = (char *)malloc(1024);
  if (buf == NULL)
    return(NULL);

  *user_buf = buf;
  return(getpwnam(user_name));
  }
