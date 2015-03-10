#include "license_pbs.h" /* See here for the software license */
#include <grp.h>
#include "utils.h"


/**
 * getgrnam_ext - first calls getgrnam, and if this call doesn't return
 * anything, then it checks if the name is actually a group id by calling getgrgid
 * 
 * @param grp_name (I) - a string containing either the group's name or id
 * @return a pointer to the group, or NULL if the string represents neither
 * a valid group name nor a valid group id, or is NULL itself.
**/


struct group *getgrnam_ext( 

  char *grp_name) /* I */

  {
  struct group * grp;

  /* bad argument check */
  if (grp_name == NULL)
    return NULL;

  grp = getgrnam(grp_name);

  /* if the group wasn't found by name, check if the name */
  /* was the group's id */
  if (grp == NULL)
    {
    if (isdigit(grp_name[0]))
      grp = getgrgid(atoi(grp_name));
    }
  
  return grp;
  } /* END getgrnam_ext() */


