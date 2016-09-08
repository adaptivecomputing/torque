#include "license_pbs.h" /* See here for the software license */
    
#include <stdio.h>  /* sprintf */
#include <stdlib.h> /* calloc */
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "log.h"

#include "pbs_error.h"

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
  
  if ((rc != 0) && (errno == ERANGE))
    {
    do
      {
      free(buf);
      bufsize *= 2;
      buf = (char *)calloc(1, bufsize);
      rc = getpwnam_r(user_name, pwent, buf, bufsize, &result);
      }while((rc != 0) && (errno == ERANGE));
    }
  else if ((rc) ||
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
   if ((rc != 0) && (errno == ERANGE))
    {
    do
      {
      free(buf);
      bufsize *= 2;
      buf = (char *)calloc(1, bufsize);
      rc = getpwuid_r(uid, pwent, buf, bufsize, &result);
      }while((rc != 0) && (errno == ERANGE));
    }
   else if ((rc) ||
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


int rmdir_ext(

  const char *dir,
  int         retry_limit)

  {
  int rc;
  int retry_count = 0;

  while ((retry_count < retry_limit) &&
         (rc = remove(dir)))
    {
    switch (errno)
      {
      case EINTR:
      case EBUSY:
      case EPERM:

        retry_count++;
        sleep(1);
        rc = PBSE_NONE;

        break;

      case ENOENT:
      case EISDIR:

        rc = PBSE_NONE;
        retry_count += retry_limit;

        break;

      default:

        retry_count++;
        rc = -1;

        break;
      }
    }

  if (rc == 0)
    errno = 0;

  return(rc);
  } // END rmdir_ext()



int unlink_ext(

  const char *filename,
  int         retry_limit)

  {
  int rc;
  int retry_count = 0;

  while ((retry_count < retry_limit) &&
         (rc = unlink(filename)))
    {
    switch (errno)
      {
      case EINTR:
      case EBUSY:

        retry_count++;
        usleep(200000);
        rc = PBSE_NONE;

        break;

      case ENOENT:

        rc = PBSE_NONE;
        errno = 0;
        retry_count += retry_limit;

        break;

      default:
        
        retry_count += retry_limit;
        rc = -1;

        break;
      }
    }

  if (rc == 0)
    errno = 0;

  return(rc);
  } // END unlink_ext()



int mkdir_wrapper(

  const char *pathname,
  mode_t      mode)

  {
  struct stat buf;
  int         rc = mkdir(pathname, mode);

  if (rc != 0)
    {
    if (errno != EEXIST)
      return(rc);
    }

  rc = stat(pathname, &buf);
  if (rc != 0)
    return(rc);

  if (buf.st_mode != mode)
    rc = chmod(pathname, mode);

  return(rc);
  } // END mkdir_wrapper()
