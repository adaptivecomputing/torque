#include "license_pbs.h" /* See here for the software license */
#include <grp.h>
#include "utils.h"


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

/**
 * getgrnam_ext - first calls getgrnam, and if this call doesn't return
 * anything, then it checks if the name is actually a group id by calling getgrgid
 * 
 * @param grp_name (I) - a string containing either the group's name or id
 * @return a pointer to the group, or NULL if the string represents neither
 * a valid group name nor a valid group id, or is NULL itself.
**/


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

  grp = (struct group *)calloc(1, sizeof(struct group));
  if (grp == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "could not allocate passwd structure");
    free(buf);
    return(NULL);
    }

  rc = getgrnam_r(grp_name, grp, buf, bufsize, &result);

  while ((rc != 0) &&
         (errno == ERANGE))
    {
    free(buf);
    bufsize *= 2;
    buf = (char *)calloc(1, bufsize);
    rc = getgrnam_r(grp_name, grp, buf, bufsize, &result);
    }

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
   while ((rc != 0) &&
         (errno == ERANGE))
    {
    free(buf);
    bufsize *= 2;
    buf = (char *)calloc(1, bufsize);
    rc = getgrgid_r(grp_id, grp, buf, bufsize, &result);
    }

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


