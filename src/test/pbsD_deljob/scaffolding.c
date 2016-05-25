#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attropl */

int pbs_errno = 0;

int PBSD_manager(int c, int function, int command, int objtype, const char *objname, struct attropl *aoplp, char *extend, int *local_errno)
  {
  fprintf(stderr, "The call to PBSD_manager needs to be mocked!!\n");
  exit(1);
  }

