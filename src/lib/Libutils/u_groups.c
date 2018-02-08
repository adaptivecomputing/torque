#include "license_pbs.h" /* See here for the software license */
#include <grp.h>
#include <pwd.h>
#include <string.h>
#include "utils.h"
#include "u_wrapper.h"


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
   * is_group_member - checks to see if user_name is a member of group_name
   *
   * @param user_name (I) - name of user being checked for group membership
   * @param group_name (I) - group name being checked for user membership
   * @return true if user_name is a member of group_name. Otherwise return false
   *
   **/
  bool is_group_member(char *user_name, char *group_name)
  {
    int j, ngroups;
    gid_t *groups;
    struct passwd *pw = NULL;
    struct group *gr = NULL;
    char   *user_buffer = NULL;
    char   log_buf[LOCAL_LOG_BUF_SIZE];

    ngroups = 0;

    // You can malloc a 0 byte buffer
    groups = (gid_t *)malloc(ngroups * sizeof(gid_t));
    if (groups == NULL)
    {
      log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "failed to allocate memory");
      return(false);
    }

    pw = getpwnam_wrapper(&user_buffer, user_name);
    if (pw == NULL)
    {
      free(groups);
      if (user_buffer != NULL)
      {
        free(user_buffer);
      }
      sprintf(log_buf, "user %s not found.", user_name);
      log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
      return(false);
    }

    // The first call to getgrouplist is intended to get the number of
    // groups for user_name. It will fail but set ngroups to the right number
    if (getgrouplist(user_name, pw->pw_gid, groups, &ngroups)  == -1)
    {
      free(groups);
      if (ngroups > 0)
      {
        groups = (gid_t*)malloc(ngroups * sizeof(gid_t));
        if (groups == NULL)
        {
          free(pw);
          free(user_buffer);
	  log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "failed to allocate memory for getgrouplist");
          return(false);
        }
        if (getgrouplist(user_name, pw->pw_gid, groups, &ngroups)  == -1)
        {
          sprintf(log_buf, " Call to getgrouplist failed: %s", strerror(errno));
          free(pw);
          free(user_buffer);
          log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
          return(false);
        }
      }
    }

    for (j = 0; j < ngroups; j++)
    {
      gr = getgrgid(groups[j]);
      if (gr == NULL)
      {
        continue;
      }

      if (!strcasecmp(gr->gr_name, group_name))
      {
        free(pw);
        free(user_buffer);
        free(groups);
        return(true);
      }
    }

    free(pw);
    free(user_buffer);
    free(groups);
    return(false);
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


