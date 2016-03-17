#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* connect_handle */

struct connect_handle connection[10];


int PBSD_mgr_put(int c, int function, int command, int objtype, const char *objname, struct attropl *aoplp, char *extend)
  {
  fprintf(stderr, "The call to PBSD_mgr_put needs to be mocked!!\n");
  exit(1);
  }

struct batch_reply *PBSD_rdrpy(int *local_errno, int c)
  {
  fprintf(stderr, "The call to PBSD_rdrpy needs to be mocked!!\n");
  exit(1);
  }

void PBSD_FreeReply(struct batch_reply *reply)
  {
  fprintf(stderr, "The call to PBSD_FreeReply needs to be mocked!!\n");
  exit(1);
  }


