#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "batch_request.h" /* batch_request */

int LOGLEVEL = 0;

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to log_ext to be mocked!!\n");
  exit(1);
  }

