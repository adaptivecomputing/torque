#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* connect_handle, batch_reply */
#include "pbs_ifl.h" /* PBS_MAXUSER */

int pbs_errno = 0;
struct connect_handle connection[10];

int PBSD_sig_put(int c, const char *jobid, const char *signal, char *extend)
  {
  fprintf(stderr, "The call to PBSD_sig_put needs to be mocked!!\n");
  exit(1);
  }

int PBSD_async_sig_put( int   c, const char *jobid, const char *signal, char *extend)
  {
  fprintf(stderr, "The call to PBSD_async_sig_put needs to be mocked!!\n");
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
