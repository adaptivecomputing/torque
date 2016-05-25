#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* connect_handle, batch_reply */
#include "attribute.h" /* attropl */

int pbs_errno = 0;
struct connect_handle connection[10];

int PBSD_manager(int c, int function, int command, int objtype, const char *objname, struct attropl *aoplp, char *extend, int *local_errno)
  {
  fprintf(stderr, "The call to PBSD_manager needs to be mocked!!\n");
  exit(1);
  }

int PBSD_gpu_put(int c, char *node, char *gpuid, int gpumode, int reset_perm, int reset_vol, char *extend)
  {
  fprintf(stderr, "The call to PBSD_gpu_put needs to be mocked!!\n");
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

void DIS_tcp_cleanup(struct tcp_chan *chan)
  {
  }
