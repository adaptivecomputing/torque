#include "license_pbs.h" /* See here for the software license */
    
#include <stdio.h>  /* sprintf */
#include <stdlib.h> /* calloc */
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


/**
 * getpwnam_wrapper - Get the passwd structure of a user
 *
 * getpwnam_wrapper allocates all the needed memory and makes a call to getpwnam_r. 
 * 
 * @param user_buffer - A pointer to the buffer containing the string fields pointed to by the passwd structure
 * @param user_name - The name of the user for which user information is being retrieved.
 *
 * @returns a struct passwd pointer. The string fields in the passwd structure point to strings in user_buffer
 */
struct passwd *getpwnam_wrapper(
  char **user_buffer, /* getpwnam_r uses a buffer which must be freed later. This is a pointer to that buffer */
  const char *user_name
  );

struct passwd *getpwuid_wrapper(
  char **user_buf,
  uid_t uid);

int rmdir_ext(
  const char *dir,
  int         retry_limit);

int unlink_ext(
  const char *filename,
  int         retry_limit);

int mkdir_wrapper(
  const char *pathname,
  mode_t      mode);
