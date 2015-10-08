#include "license_pbs.h" /* See here for the software license */
    
#include <stdio.h>  /* sprintf */
#include <stdlib.h> /* calloc */
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.h"

struct passwd *getpwnam_wrapper(

  char      **user_buffer, /* getpwnam_r uses a buffer which must be freed later. This is a pointer to that buffer */
  const char *user_name)

  {
  struct passwd *pwent;
  char  *buf;
  long   bufsize;
  struct passwd *result;
  int rc;

  *user_buffer = NULL;

  bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize == -1)
    bufsize = 8196;

  buf = (char *)malloc(bufsize);
  if (buf == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "failed to allocate memory");
    return(NULL);
    }

  pwent = (struct passwd *)calloc(1, sizeof(struct passwd));
  if (pwent == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "could not allocate passwd structure");
    free(buf);
    return(NULL);
    }

  rc = getpwnam_r(user_name, pwent, buf, bufsize, &result);
  if ((rc) ||
      (result == NULL))
    {
    sprintf(buf, "getpwnam_r failed: %d", rc);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);

    free(buf);
    free(pwent);

    return (NULL);
    }
  
  *user_buffer = buf;
  return(pwent);
  }

struct passwd *getpwuid_wrapper(

  char **user_buf,
  uid_t uid)

  {
  struct passwd *pwent;
  char  *buf;
  long   bufsize;
  struct passwd *result;
  int rc;

  *user_buf = NULL;
  bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize == -1)
    bufsize = 8196;

  buf = (char *)malloc(bufsize);
  if (buf == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "failed to allocate memory");
    return(NULL);
    }

  pwent = (struct passwd *)calloc(1, sizeof(struct passwd));
  if (pwent == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "could not allocate passwd structure");
    return(NULL);
    }

  rc = getpwuid_r(uid, pwent, buf, bufsize, &result);
  if ((rc) ||
      (result == NULL))
    {
    sprintf(buf, "getpwnam_r failed: %d", rc);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);
    
    free(buf);
    free(pwent);
    return (NULL);
    }
  
  *user_buf = buf;
  return(pwent); 
  }
