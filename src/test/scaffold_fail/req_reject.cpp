/*
 *
 *  Created on: Jan 24, 2013
 *      Author: bdaw
 */

#include "scaffold_fail.h"

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

