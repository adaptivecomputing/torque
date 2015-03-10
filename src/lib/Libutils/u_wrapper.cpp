#include "license_pbs.h" /* See here for the software license */
    
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

struct passwd *getpwnam_wrapper(

  const char *user_name)

  {
  return(getpwnam(user_name));
  }

struct passwd *getpwuid_wrapper(

  uid_t uid)

  {
  return(getpwuid(uid));
  }
