#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* connect_handle */

struct connect_handle connection[10];


void pbs_statfree(struct batch_status *bsp)
  {
  fprintf(stderr, "The call to pbs_statfree needs to be mocked!!\n");
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

int PBSD_status_put(int c, int function, char *id, struct attrl *attrib, char *extend)
  {
  fprintf(stderr, "The call to PBSD_status_put needs to be mocked!!\n");
  exit(1);
  }

