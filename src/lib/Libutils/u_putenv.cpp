#include "utils.h"

/*
 * put_env_var - put the name=value string into the process environment
 *
 * @return = 0 - SUCCESS - successful insertion into environment
 * @return < 0 - FAILURE - general internal failure 
 *   -1 name or value are NULL
 *   -2 calloc failed
 *   -3 putenv() failed
 */
int put_env_var(const char *name, const char *value)
  {
  char *envstr;

  if (name == NULL || value == NULL)
    {
    return(-1);
    }

  /* grab memory for environment variable string "name=value" */
  envstr = (char *)calloc((strlen(name) + strlen(value) + 2), sizeof(char));

  if (envstr == NULL)
    {
    /* calloc() failure */
    return(-2);
    }

  /* build the env. var string */
  sprintf(envstr, "%s=%s", name, value);

  /* do _not_ free the string when using putenv */

  if (putenv(envstr))
    {
    /* putenv() failure */
    return(-3);
    }

  return(PBSE_NONE);
  } /* END put_env_var() */
