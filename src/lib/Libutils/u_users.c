#include "license_pbs.h" /* See here for the software license */

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"
#include "errno.h"
#include "log.h"
#include "../Liblog/pbs_log.h"

#define LDAP_RETRIES 5

struct passwd *getpwnam_wrapper(const char *user_name);
struct passwd *getpwuid_wrapper(uid_t uid);


/**
 * getpwnam_ext - calls getpwnam, then logs any error occurrances. 
 * 
 * @param user_name (I) - a string containing either the user's name or id
 * @return a pointer to the group, or NULL if the string is not a valid user id.
**/

struct passwd *getpwnam_ext( 

  char *user_name) /* I */

  {
  struct passwd *pwent = NULL;
  int            retrycnt = 0;

  /* bad argument check */
  if (user_name == NULL)
    return NULL;

  errno = 0;

  while ((pwent == NULL) && (retrycnt != -1) && (retrycnt < LDAP_RETRIES))
    {
    pwent = getpwnam_wrapper( user_name );

    /* if the user wasn't found check for any errors to log */
    if (pwent == NULL)
      {
      switch (errno)
        {
        case EINTR:
        case EIO:
        case EMFILE:
        case ENFILE:
        case ENOMEM:
        case ERANGE:
          sprintf(log_buffer, "ERROR: getpwnam() error %d (%s)",
                  errno,
                  strerror(errno));

          log_ext(-1, __func__, log_buffer, LOG_ERR);
          retrycnt++;
          break;

        default:
          retrycnt = -1;
          break;
        }
      }
    }

  return(pwent);
  } /* END getpwnam_ext() */



/*
 * get_password_entry_by_uid()
 *
 * calls the getpwuid_wrapper in a loop with 5 retries to 
 * handle transient LDAP failures.
 */
struct passwd *get_password_entry_by_uid(

  uid_t uid)

  {
  struct passwd *user_pwd = NULL;
  
  for (int i = 0; i < LDAP_RETRIES; i++)
    {
    if ((user_pwd = getpwuid_wrapper(uid)) != NULL)
      break;
    }

  return(user_pwd);
  } /* get_password_entry_by_uid() */



/*
 * @param uid - the uid to set
 * @param set_euid - if TRUE, call seteuid instead of setuid
 */

int setuid_ext(

  uid_t uid,      /* I */
  int   set_euid) /* I */

  {
  int count = 0;
  int rc = PBSE_NONE;

  errno = 0;

  while (count < LDAP_RETRIES)
    {
    if (set_euid == TRUE)
      rc = seteuid(uid);
    else
      rc = setuid(uid);

    if (rc == 0)
      break;
    else
      {
      switch (errno)
        {
        case EAGAIN:
        case EINTR:

          /* transient failure */
          usleep(200);
          count++;

          break;

        default:

          /* permanent failure */
          count += LDAP_RETRIES;
          break;

        }
      }
    } /* end retry loop */

  return(rc);
  } /* END setuid_ext() */

