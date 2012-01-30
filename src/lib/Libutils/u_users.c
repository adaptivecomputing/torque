#include "license_pbs.h" /* See here for the software license */
#include <pwd.h>
#include "utils.h"
#include "errno.h"
#include "log.h"
#include "../Liblog/pbs_log.h"


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

  while ((pwent == NULL) && (retrycnt != -1) && (retrycnt < 5))
    {
    pwent = getpwnam( user_name );

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
