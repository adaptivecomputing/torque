#include <stdio.h>
#include <stdlib.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

int pam_get_user(const pam_handle_t *pamh, const char **user, const char *prompt)
  {
  return(PAM_SUCCESS);
  }
