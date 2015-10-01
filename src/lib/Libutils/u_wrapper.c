#include "license_pbs.h" /* See here for the software license */
    
#include <stdio.h>  /* sprintf */
#include <stdlib.h> /* calloc */
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.h"

struct passwd *getpwnam_wrapper(

  const char *user_name)

  {
  struct passwd *pwent;
  char  *buf;
  long   bufsize;
  struct passwd *result;
  int rc;

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
  
  return(pwent);
  }

struct passwd *getpwuid_wrapper(

  uid_t uid)

  {
  struct passwd *pwent;
  char  *buf;
  long   bufsize;
  struct passwd *result;
  int rc;

  bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize == -1)
    bufsize = 8196;

  buf = (char *)calloc(1, bufsize);
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
    
  return(pwent); 
  }
